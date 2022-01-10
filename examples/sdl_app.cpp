#include <actor.h>
#include <adapters/sdl2/adapter.h>
#include <adapters/sdl2/handler.h>
#include <app.h>
#include <asset.h>
#include <asset_handler.h>
#include <components/animator_component.h>
#include <components/indexer_component.h>
#include <components/input_component.h>
#include <components/material_component.h>
#include <components/prefab_component.h>
#include <components/renderer2d_component.h>
#include <components/resource_component.h>
#include <components/shape_component.h>
#include <components/transform_component.h>
#include <finder.h>
#include <geometry.h>
#include <helpers/actor_actions.h>
#include <helpers/actor_modifiers.h>
#include <helpers/actor_triggers.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
using namespace tte;

namespace player {
	inline bool hasCollider(int32_t mapCode) {
		return mapCode >= 0x20;
	}

	auto setSpeedX = [](Actor &a) {
		bool isJumping = a.props().get<bool>("jumping.value", false);
		PTree::Property<float> speedX(a.props(), "speed.value.x", 0.0f);
		auto speedX_ = speedX.get();
		auto accelX = a.props().get<float>(isJumping ? "accel.xj" : "accel.xr", 0.0f);
		auto registX = a.props().get<float>(isJumping ? "accel.xjRegist" : "accel.xrRegist", 0.0f);
		auto dir = (onButtonOn("left")(a) ? -1 : 0) + (onButtonOn("right")(a) ? +1 : 0);
		auto dirs = (abs(speedX_) < registX) ? 0 : (speedX_ < 0) ? -1 : +1;

		if (dir != 0) {
			accelX = dir * accelX;
			if (!isJumping) {
				string replayName = (dir * dirs < 0) ? "turning" : "running";
				replayName += (dirs == 0) && (dir < 0) || (dirs < 0) ? "-l" : "-r";
				a.props().put<string>("replay", replayName);
			}
		} else if (dirs == 0) {
			speedX_ = accelX = 0.0f;
			if (!isJumping) {
				a.props().put<string>("replay", "standing");
			}
		} else {
			accelX = -dirs * accelX;
		}

		speedX_ += accelX;
		speedX_ = clamp(speedX_, a.props().get<float>("speed.min.x", 0.0f), a.props().get<float>("speed.max.x", 0.0f));
		speedX.set(speedX_);
	};

	auto moveX = [](Actor &a) {
		auto speedX = a.props().get<float>("speed.value.x", 0.0f);
		PTree::Property<float> wx(a.props(), "wp.x", 0.0f);
		auto wx_ = wx.get();

		wx.set(wx_ += speedX);
	};

	auto adjustX = [](Actor &a) {
		Finder<Actor>::find<ShapeTilemap>("levelmap", [&a](auto &tilemap) {
			PTree::Property<float> speedX(a.props(), "speed.value.x", 0.0f);
			PTree::PropertyV<vector2> wp(a.props(), "wp", 0.0f);
			auto wp_ = wp.get();
			PTree::PropertyV<vector2> c2(a.props(), "collider2d", 0.0f);
			auto c2_ = c2.get();

			unordered_map<string, vector2> offsets = {
				{ "LT", vector2{ X(wp_) - (X(c2_) / 2 + 1), Y(wp_) - Y(c2_), }, },
				{ "LB", vector2{ X(wp_) - (X(c2_) / 2 + 1), Y(wp_) - 0, }, },
				{ "RT", vector2{ X(wp_) + (X(c2_) / 2 + 1), Y(wp_) - Y(c2_), }, },
				{ "RB", vector2{ X(wp_) + (X(c2_) / 2 + 1), Y(wp_) - 0, }, },
			};
			unordered_map<string, vector2i> mapAddresses;
			for (auto &offset : offsets) {
				mapAddresses.insert({ offset.first, tilemap.getAddress(scalar_cast<int32_t>(offset.second)), });
			}
			bool isHitL = hasCollider(tilemap.peek(mapAddresses.at("LT"))) || hasCollider(tilemap.peek(mapAddresses.at("LB")));
			bool isHitR = hasCollider(tilemap.peek(mapAddresses.at("RT"))) || hasCollider(tilemap.peek(mapAddresses.at("RB")));

			if (isHitL && !isHitR) {
				// hit lefthand
				auto posL = tilemap.getWp(mapAddresses.at("LB") + vector2i{ 1, 0, });
				X(wp_) = X(posL) + (X(c2_) / 2 + 1);
				wp.set(wp_);
				speedX.set(0.0f);
			}
			if (!isHitL && isHitR) {
				// hit righthand
				auto posR = tilemap.getWp(mapAddresses.at("RB"));
				X(wp_) = X(posR) - (X(c2_) / 2 + 1);
				wp.set(wp_);
				speedX.set(0.0f);
			}
			if (X(wp_) < X(tilemap.viewOffset.get()) + X(c2_) / 2) {
				// adjust screen left
				X(wp_) = X(tilemap.viewOffset.get()) + X(c2_) / 2;
				wp.set(wp_);
			}
		});
	};

	auto adjustLevel = [](Actor &a, Transform &transform) {
		Finder<Actor>::find<ShapeTilemap>("levelmap", [&a, &transform](auto &tilemap) {
			auto viewOffset_ = tilemap.viewOffset.get();
			PTree::PropertyV<vector2> wp(a.props(), "wp", 0.0f);
			auto wp_ = wp.get();

			// set relative pos from world pos
			auto pos = transform.translation.get();
			X(pos) = X(wp_) - X(viewOffset_);
			Y(pos) = Y(wp_) - Y(viewOffset_);

			// adjust screen right
			int32_t overflowX = static_cast<int32_t>(X(pos) - 15 * 8);
			if (overflowX > 0) {
				X(viewOffset_) += overflowX;
				tilemap.viewOffset.set(viewOffset_);
				X(pos) -= static_cast<float>(overflowX);
			}
			transform.translation.set(pos);
		});
	};

	auto setSpeedY = [](Actor &a) {
		auto accelY = a.props().get<float>("accel.yjGravity", 0.0f);
		PTree::Property<float> speedY(a.props(), "speed.value.y", 0.0f);
		auto speedY_ = speedY.get();
		PTree::Property<bool> isJumping(a.props(), "jumping.value", false);
		PTree::Property<int32_t> frameLeft(a.props(), "jumping.frame.left", 0);

		if (isJumping.get()) {
			// jumping
			if (onButtonOn("z")(a) && (frameLeft.get() > 0)) {
				// jumping up
				accelY = 0.0f;
				frameLeft.set(frameLeft.get() - 1);
			} else {
				frameLeft.set(0);
			}
		} else {
			// standing
			if (onButtonPressed("z")(a)) {
				// start jumping
				accelY = 0.0f;
				speedY_ = a.props().get<float>("speed.yj0", 0.0f);
				isJumping.set(true);
				frameLeft.set(a.props().get<int32_t>("jumping.frame.max", 0));
				a.props().put<string>("replay", "jumping");
			} else {
				accelY = 0.0f;
				speedY_ = 0.0f;
			}
		}

		speedY_ += accelY;
		speedY_ = clamp(speedY_, a.props().get<float>("speed.min.y", 0.0f), a.props().get<float>("speed.max.y", 0.0f));
		speedY.set(speedY_);
	};

	auto moveY = [](Actor &a) {
		auto speedY = a.props().get<float>("speed.value.y", 0.0f);
		PTree::Property<float> wy(a.props(), "wp.y", 0.0f);
		auto wy_ = wy.get();

		wy.set(wy_ += speedY);
	};

	auto adjustY = [](Actor &a) {
		Finder<Actor>::find<ShapeTilemap>("levelmap", [&a](auto &tilemap) {
			PTree::Property<bool> isJumping(a.props(), "jumping.value", false);
			PTree::Property<float> speedY(a.props(), "speed.value.y", 0.0f);
			PTree::PropertyV<vector2> wp(a.props(), "wp", 0.0f);
			auto wp_ = wp.get();
			PTree::PropertyV<vector2> c2(a.props(), "collider2d", 0.0f);
			auto c2_ = c2.get();

			unordered_map<string, vector2> offsets = {
				{ "TC", vector2{ X(wp_), Y(wp_) - (Y(c2_) + 1), }, },
				{ "BL", vector2{ X(wp_) - X(c2_) / 2, Y(wp_) + 1, }, },
				{ "BR", vector2{ X(wp_) + X(c2_) / 2, Y(wp_) + 1, }, },
			};
			unordered_map<string, vector2i> mapAddresses;
			for (auto &offset : offsets) {
				mapAddresses.insert({ offset.first, tilemap.getAddress(scalar_cast<int32_t>(offset.second)), });
			}
			bool isHitT = hasCollider(tilemap.peek(mapAddresses.at("TC")));
			bool isHitB = hasCollider(tilemap.peek(mapAddresses.at("BL"))) || hasCollider(tilemap.peek(mapAddresses.at("BR")));

			if (isJumping.get()) {
				// jumping
				if ((speedY.get() < 0.0f) && isHitT) {
					auto posT = tilemap.getWp(mapAddresses.at("TC") + vector2i{ 0, 1, });
					Y(wp_) = static_cast<float>(Y(posT) + (Y(c2_) + 1));
					wp.set(wp_);

					speedY.set(0.0f);
					a.props().put<int32_t>("jumping.frame.left", 0);
				} else if (isHitB) {
					auto posB = tilemap.getWp(mapAddresses.at("BL"));
					Y(wp_) = static_cast<float>(Y(posB) - 1);
					wp.set(wp_);

					isJumping.set(false);
					a.props().put<string>("replay", "standing");
				}
			} else {
				// standing
				if (!isHitB) {
					isJumping.set(true);
					a.props().put<int32_t>("jumping.frame.left", 0);
				}
			}
		});
	};

	function<void(Actor &, Animator &)> replay(Asset &playerAnim) {
		return [&playerAnim](Actor &a, Animator &animator) {
			auto replayName = a.set<string>("replay", "");
			auto replayName_ = a.set<string>("replay_", replayName);
			if (replayName.empty() && (replayName_.find("running") == 0)) {
				animator.getTimeline("moving", [](auto &timeline) {
					timeline.abort();
				});
			}
			if (!replayName.empty() && (replayName != replayName_)) {
				animator.replay(playerAnim, replayName, "moving");
			}
		};
	}

	auto onDead = [](Actor &a) -> bool {
		PTree::PropertyV<vector2> wp(a.props(), "wp", 0.0f);
		auto wp_ = wp.get();
		return Y(wp_) >= 16 * 16;
	};

	auto action = [](Asset &assetBase, function<void(Asset &)> transition) {
		auto &playerAnim = assetBase.find(L"ingame/player.anim");
		return (
			player::setSpeedX + player::moveX + player::adjustX +
			player::setSpeedY + player::moveY + player::adjustY +
			withComponent<Transform>(player::adjustLevel) +
			withComponent<Animator>(player::replay(playerAnim)) +
			player::onDead * (
				findThen("prefab:stage", withComponent<Prefab>([](Actor &, auto &prefab) {
					prefab.unload();
					cout << "dead" << endl;
				})) +
				findThen("sys:root", appendAction([&assetBase, transition](Actor &) {
					transition(assetBase);
				}))
			)
		);
	};
}

namespace topmenu {
	auto action = [](Asset &assetBase, function<void(Asset &)> transition) {
		return (
			onButtonPressed("z") * (
				findThen("prefab:topmenu", withComponent<Prefab>([](Actor &, auto &prefab) {
					prefab.unload();
				})) +
				findThen("sys:root", appendAction([&assetBase, transition](Actor &) {
					transition(assetBase);
				}))
			)
		);
	};
}

namespace transition {
	void beginIngame(Asset &assetBase);

	void beginMenu(Asset &assetBase) {
		appendChildTo("sys:root", loadPrefab(assetBase.find(L"topmenu.json"), assetBase));
		appendActionTo("guide", topmenu::action(assetBase, beginIngame));
	}

	void beginIngame(Asset &assetBase) {
		appendChildTo("sys:root", loadPrefab(assetBase.find(L"ingame.json"), assetBase));
		appendActionTo("p1", player::action(assetBase, beginMenu));
	}
}

class SdlApp : public App {
private:
	std::unique_ptr<sdl2::Adapter> m_adapter;
	std::unique_ptr<Asset> m_assets;
	std::unique_ptr<Actor> m_actors;

public:
	explicit SdlApp()
		: App(), m_adapter(), m_assets(), m_actors()
	{
		cout << __FUNCTION__ << endl;
		AssetHandler assetHandler;
		assetHandler.clear();
		assetHandler.append({ L"<undef>", AssetHandler::typeUnknown, });
		assetHandler.append({ L".json", AssetHandler::typeJson, });
		assetHandler.append({ L".anim", AnimationSet::typeAnim, });
		assetHandler.append({ L".png", sdl2::Adapter::typePng, });
		assetHandler.append({ L"", AssetHandler::typeDir, });
		m_assets = std::make_unique<Asset>(L"asset/sdl", assetHandler);
	}

	virtual ~SdlApp() override {
		cout << __FUNCTION__ << endl;
		m_assets.reset();
		AssetHandler assetHandler;
		assetHandler.clear();
		m_adapter.reset();
	}

	virtual void initialize() override {
		cout << __FUNCTION__ << endl;
		initializeActors();
		m_assets->find(L"common").load();
		transition::beginMenu(*m_assets);
	}

	virtual void finalize() override {
		cout << __FUNCTION__ << endl;
		m_assets->find(L"common").unload();
		m_actors.reset();
	}

	virtual bool isRunning() override {
		bool bQuit = Finder<Actor>::find<bool>("sys:input", true, [](Actor &a) -> bool {
			return a.get<bool>("quit", true);
		});
		return !bQuit;
	}

	virtual void tick() override {
		if (auto root = m_actors.get()) {
			root->act();
			for_each(root->begin(true), root->end(), [](Actor &a) {
				a.act();
			});
		}
	}

private:
	void initializeActors() {
		auto &config = m_assets->find(L"config.json");
		config.load();
		m_adapter.reset(new sdl2::Adapter(config));
		config.unload();

		m_actors.reset(m_adapter->initializeActors());
		auto appGlobal = new Actor(Actor::noAction, Indexer::append("global"));
		m_actors->appendChild(appGlobal);

		appendActionTo(
			"sys:inputKbd",
			onButtonPressed("escape") * findThen("sys:input", put<bool>("quit", true))
		);
	}
};

extern "C" int sdl_app() {
	cout << __FUNCTION__ << endl;
	{
		auto app = make_unique<SdlApp>();
		App::Runner runner(std::move(app));
		runner.run();
	}
	return 0;
}
