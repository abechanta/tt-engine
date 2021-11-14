#include <actor.h>
#include <actor_modifiers.h>
#include <actor_triggers.h>
#include <adapters/sdl2/adapter.h>
#include <components/animator_component.h>
#include <app.h>
#include <asset.h>
#include <asset_handler.h>
#include <finder.h>
#include <geometry.h>
#include <adapters/sdl2/handler.h>
#include <components/indexer_component.h>
#include <components/input_component.h>
#include <components/material_component.h>
#include <components/primitive_component.h>
#include <components/renderer2d_component.h>
#include <components/resource_component.h>
#include <timeline.h>
#include <components/transform_component.h>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>
using namespace tte;

class Showcase1 : public App {
private:
	std::unique_ptr<sdl2::Adapter> &m_adapter;
	std::unique_ptr<Asset> m_assets;
	std::unique_ptr<Actor> m_actors;

public:
	Showcase1() : App(), m_adapter(sdl2::Adapter::create()), m_assets(), m_actors() {
		cout << __FUNCTION__ << endl;
		AssetHandler::clear();
		AssetHandler::appendInitializer({ AssetHandler::extensionUnknown, AssetHandler::initializerUnknown, });
		AssetHandler::appendInitializer({ L".json", AssetHandler::initializerJson, });
		AssetHandler::appendInitializer({ L".png", sdl2::Adapter::initializerPng(*m_adapter), });
		AssetHandler::appendInitializer({ L"", AssetHandler::initializerDir, });
		m_assets = std::make_unique<Asset>(L"asset", AssetHandler::factory(L"asset:"));
	}

	virtual ~Showcase1() override {
		cout << __FUNCTION__ << endl;
		m_assets.reset();
		m_adapter.reset();
		AssetHandler::clear();
	}

	virtual void initialize() override {
		cout << __FUNCTION__ << endl;

		auto &config = m_assets->find(L"config.json");
		config.load();
		auto root = new Actor(actChildren(), Indexer::append("sys:root"));
		auto resource = new Actor(Actor::noAction, Indexer::append("sys:resource"));
		auto window = m_adapter->createWindowActor(config, Indexer::append("window:"));
		auto input = m_adapter->createInputActor(config, Indexer::append("input:"));
		auto inputKbd = m_adapter->createKeyboardActor(config, Actor::noAction);
		auto scene = new Actor(Actor::noAction, Indexer::append("sys:scene"));
		auto renderer = m_adapter->createRendererActor(config, Indexer::append("renderer:"));
		config.unload();

		root->appendChild(window);
		root->appendChild(input->appendChild(inputKbd));
		root->appendChild(resource);
		root->appendChild(scene);
		root->appendChild(renderer);
		m_actors.reset(root);

		auto &showcase = m_assets->find(L"showcase");
		resource->appendChild(new Actor(
			Actor::noAction,
			Indexer::append("asset:showcase") + Resource::append(showcase)
		));

		scene->appendChild(new Actor(
			inState("1") * onButtonPressed("left") * (
				changeState("2") +
				componentModifier<Animator>("chara0", play("left")) +
				componentModifier<Animator>("chara1", play("left")) +
				componentModifier<Animator>("chara2", play("left")) +
				componentModifier<Animator>("chara3", play("left"))
			) +
			inState("0") * onButtonPressed("right") * (
				changeState("1") +
				componentModifier<Animator>("chara0", play("right")) +
				componentModifier<Animator>("chara1", play("right")) +
				componentModifier<Animator>("chara2", play("right")) +
				componentModifier<Animator>("chara3", play("right"))
			) +
			inState("2") * changeState("0"),
			put<string>("state", "0")
		));
		scene->appendChild(new Actor(
			renderAsTilemap() +
			[](Actor &a) {
				auto x = a.props().get<int32_t>("tilemap.viewOffset.x");
				a.props().put<int32_t>("tilemap.viewOffset.x", ++x);
			},
			loadProps(showcase.find(L"tilemap1.json")) + Transform::append() + Material::append(showcase.find(L"SMB_BANK0@16.png"))
		));
		scene->appendChild(new Actor(
			renderAsTilemap(),
			loadProps(showcase.find(L"tilemap0.json")) + Transform::append() + Material::append(showcase.find(L"font8x8.png"))
		));
		scene->appendChild(new Actor(
			Transform::apply() + renderAsSprite(),
			loadProps(showcase.find(L"chara0.json")) + Indexer::append("chara0") + Transform::append() + Material::append(showcase.find(L"hedgehog.png")) + Animator::append(showcase.find(L"chara0.anim.json"))
		));
		scene->appendChild(new Actor(
			Transform::apply() + renderAsSprite(),
			loadProps(showcase.find(L"chara1.json")) + Indexer::append("chara1") + Transform::append() + Material::append(showcase.find(L"hedgehog.png")) + Animator::append(showcase.find(L"chara1.anim.json"))
		));
		scene->appendChild(new Actor(
			Transform::apply() + renderAsSprite(),
			loadProps(showcase.find(L"chara2.json")) + Indexer::append("chara2") + Transform::append() + Material::append(showcase.find(L"hedgehog.png")) + Animator::append(showcase.find(L"chara2.anim.json"))
		));
		scene->appendChild(new Actor(
			Transform::apply() + renderAsSprite(),
			loadProps(showcase.find(L"chara3.json")) + Indexer::append("chara3") + Transform::append() + Material::append(showcase.find(L"hedgehog.png")) + Animator::append(showcase.find(L"chara3.anim.json"))
		));
	}

	virtual void finalize() override {
		cout << __FUNCTION__ << endl;
		m_actors.reset();
	}

	virtual bool isRunning() override {
		bool bQuit = Finder<Actor>::find<bool>("input:", true, [&](Actor &a) -> bool {
			return a.get<bool>("quit", true);
		});
		return !bQuit;
	}
	
	virtual void tick() override {
		m_actors->act();
	}

private:
	function<void(Actor &)> renderAsSprite() {
		return componentModifier<Renderer2d>("renderer:", [](Actor &a, auto &renderer) {
			renderer.render(a);
		});
	}

	function<void(Actor &)> renderAsTilemap() {
		return componentModifier<Renderer2d>("renderer:", [](Actor &a, auto &renderer) {
			a.findComponent<Transform, Material>([&a, &renderer](auto &transform, auto &material) {
				const vector3 translation = transform.translation();
				const vector2 uv0 = material.uv0();
				const vector2 uv1 = material.uv1();
				{
					const vector2i cellSize = Geometry::get<vec, int32_t, 2>(a.props("tilemap.cellSize"), 8);
					const vector2i blitSize = Geometry::get<vec, int32_t, 2>(a.props("tilemap.blitSize"), 8);
					const int32_t cellBounds = X(material.size()) / X(cellSize);
					const vector2i viewPort = Geometry::get<vec, int32_t, 2>(a.props("tilemap.size"), 8);
					const vector2i viewOffset = Geometry::get<vec, int32_t, 2>(a.props("tilemap.viewOffset"), 0);
					const bool transpose = a.get<bool>("tilemap.transpose", false);
					const vector2i mapSize_ = Geometry::get<vec, int32_t, 2>(a.props("tilemap.mapSize"), 8);
					const vector2i mapSize = transpose ? vector2i{ Y(mapSize_), X(mapSize_), } : mapSize_;

					if (transpose) {
						for (int32_t rp = -(X(viewOffset) % X(blitSize)); rp < X(viewPort); rp += X(blitSize)) {
							int32_t ri = (X(viewOffset) + rp) / X(blitSize);
							if (ri >= X(mapSize)) {
								break;
							}
							transform.translation() = translation + X00<int32_t>(rp);
							const vector<int32_t> vertical = Geometry::get<vector, int32_t>(a.props("tilemap.tiles." + to_string(ri)));
							for (int32_t cp = -(Y(viewOffset) % Y(blitSize)); cp < Y(viewPort); cp += Y(blitSize)) {
								int32_t ci = (Y(viewOffset) + cp) / Y(blitSize);
								if (ci >= Y(mapSize)) {
									break;
								}
								auto code = vertical[ci];
								material.uv0() = material.to_vector2({ ((code % cellBounds) + 0) * X(cellSize), ((code / cellBounds) + 0) * Y(cellSize), });
								material.uv1() = material.to_vector2({ ((code % cellBounds) + 1) * X(cellSize), ((code / cellBounds) + 1) * Y(cellSize), });
								renderer.render(a);
								Y(transform.translation()) += Y(blitSize);
							}
						}
					} else {
						for (int32_t rp = -(Y(viewOffset) % Y(blitSize)); rp < Y(viewPort); rp += Y(blitSize)) {
							int32_t ri = (Y(viewOffset) + rp) / Y(blitSize);
							if (ri >= Y(mapSize)) {
								break;
							}
							transform.translation() = translation + _0X0<int32_t>(rp);
							const vector<int32_t> holizontal = Geometry::get<vector, int32_t>(a.props("tilemap.tiles." + to_string(ri)));
							for (int32_t cp = -(X(viewOffset) % X(blitSize)); cp < X(viewPort); cp += X(blitSize)) {
								int32_t ci = (X(viewOffset) + cp) / X(blitSize);
								if (ci >= X(mapSize)) {
									break;
								}
								auto code = holizontal[ci];
								material.uv0() = material.to_vector2({ ((code % cellBounds) + 0) * X(cellSize), ((code / cellBounds) + 0) * Y(cellSize), });
								material.uv1() = material.to_vector2({ ((code % cellBounds) + 1) * X(cellSize), ((code / cellBounds) + 1) * Y(cellSize), });
								renderer.render(a);
								X(transform.translation()) += X(blitSize);
							}
						}
					}
				}
				transform.translation() = translation;
				material.uv0() = uv0;
				material.uv1() = uv1;
			});
		});
	}

	function<void(Actor &, Animator &)> play(const string &arg1) {
		return [arg1](Actor &, Animator &animator) {
			animator.play(arg1);
		};
	}
};

extern "C" int showcase1() {
	cout << __FUNCTION__ << endl;
	{
		auto app = make_unique<Showcase1>();
		App::Runner runner(std::move(app));
		runner.run();
	}
	return 0;
}
