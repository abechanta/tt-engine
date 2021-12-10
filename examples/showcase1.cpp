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
#include <components/renderer2d_component.h>
#include <components/resource_component.h>
#include <components/shape_component.h>
#include <timeline.h>
#include <components/transform_component.h>
#include <algorithm>
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
		AssetHandler::append({ AssetHandler::extensionUnknown, AssetHandler::typeUnknown, });
		AssetHandler::append({ L".json", AssetHandler::typeJson, });
		AssetHandler::append({ L".anim", AssetHandler::typeAnim, });
		AssetHandler::append({ L".png", sdl2::Adapter::typePng(*m_adapter), });
		AssetHandler::append({ L"", AssetHandler::typeDir, });
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
		initializeActors();
		initializeScene("ingame");
	}

	virtual void finalize() override {
		cout << __FUNCTION__ << endl;
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
		auto root = new Actor(Actor::noAction, Indexer::append("sys:root") + put<bool>("_.reset", true));
		auto window = m_adapter->createWindowActor(config, Indexer::append("sys:window"));
		auto input = m_adapter->createInputActor(config, Indexer::append("sys:input"));
		auto inputKbd = m_adapter->createKeyboardActor(config, [](Actor &a) {
			a.appendAction(onButtonPressed("escape") * findThen("sys:input", put<bool>("quit", true)));
		});
		auto resource = new Actor(Actor::noAction, Indexer::append("sys:resource"));
		auto scene = new Actor(Actor::noAction, Indexer::append("sys:scene"));
		auto renderer = m_adapter->createRendererActor(config, Indexer::append("sys:renderer2d"));
		config.unload();

		auto appGlobal = new Actor(Actor::noAction, Indexer::append("global"));

		root->appendChild(appGlobal);
		root->appendChild(window);
		root->appendChild(input->appendChild(inputKbd));
		root->appendChild(resource);
		root->appendChild(scene);
		root->appendChild(renderer);
		m_actors.reset(root);
	}

	void initializeScene(const string &sceneName) {
		cout << sceneName << endl;

		auto &showcase = m_assets->find(L"showcase");

		Actor *newScene = new Actor(
			onButtonPressed("up") * findThen("sys:root", [&](Actor &a) {
				a.appendAction(findThen("asset:showcase", exit()) + findThen("scene:showcase", exit()));
			}),
			findThen("global", changeState(sceneName)) +
			findThen("sys:resource", [&](Actor &resource) {
				resource.appendChild(new Actor(
					Actor::noAction,
					Indexer::append("asset:showcase") + Resource::append(showcase)
				));
			}) +
			Indexer::append("scene:showcase")
		);

		newScene->appendChild(new Actor(
			inState("1") * onButtonPressed("left") * (
				changeState("2") +
				findThen("chara0", withComponent<Animator>(replay(showcase.find(L"chara0.anim"), "left", "moving"))) +
				findThen("chara1", withComponent<Animator>(replay(showcase.find(L"chara1.anim"), "left", "moving"))) +
				findThen("chara2", withComponent<Animator>(replay(showcase.find(L"chara2.anim"), "left", "moving"))) +
				findThen("chara3", withComponent<Animator>(replay(showcase.find(L"chara3.anim"), "left", "moving")))
			) +
			inState("0") * onButtonPressed("right") * (
				changeState("1") +
				findThen("chara0", withComponent<Animator>(replay(showcase.find(L"chara0.anim"), "right", "moving"))) +
				findThen("chara1", withComponent<Animator>(replay(showcase.find(L"chara1.anim"), "right", "moving"))) +
				findThen("chara2", withComponent<Animator>(replay(showcase.find(L"chara2.anim"), "right", "moving"))) +
				findThen("chara3", withComponent<Animator>(replay(showcase.find(L"chara3.anim"), "right", "moving")))
			) +
			inState("2") * changeState("0"),
			put<string>("state", "0")
		));
		newScene->appendChild(new Actor(
			[](Actor &a) {
				auto x = a.props().get<int32_t>("tilemap.viewOffset.x");
				a.props().put<int32_t>("tilemap.viewOffset.x", ++x);
			},
			loadProps(showcase.find(L"tilemap1.json")) + Transform::append() + Material::append(showcase.find(L"SMB_BANK0@16.png")) + Shape::append<ShapeTilemap>("sys:renderer2d")
		));
		newScene->appendChild(new Actor(
			Actor::noAction,
			loadProps(showcase.find(L"hud/score.json")) + Transform::append() + Material::append(showcase.find(L"font8x8.png")) + Shape::append<ShapeText>("sys:renderer2d")
		));
		newScene->appendChild(new Actor(
			Actor::noAction,
			loadProps(showcase.find(L"hud/coins.json")) + Transform::append() + Material::append(showcase.find(L"font8x8.png")) + Shape::append<ShapeText>("sys:renderer2d")
		));
		newScene->appendChild(new Actor(
			Actor::noAction,
			loadProps(showcase.find(L"hud/world.json")) + Transform::append() + Material::append(showcase.find(L"font8x8.png")) + Shape::append<ShapeText>("sys:renderer2d")
		));
		newScene->appendChild(new Actor(
			Actor::noAction,
			loadProps(showcase.find(L"hud/time.json")) + Transform::append() + Material::append(showcase.find(L"font8x8.png")) + Shape::append<ShapeText>("sys:renderer2d")
		));
		newScene->appendChild(new Actor(
			Actor::noAction,
			loadProps(showcase.find(L"chara0.json")) + Indexer::append("chara0") + Transform::append() + Material::append(showcase.find(L"hedgehog.png")) + Animator::append() + Shape::append<ShapeSprite>("sys:renderer2d")
		));
		newScene->appendChild(new Actor(
			Actor::noAction,
			loadProps(showcase.find(L"chara1.json")) + Indexer::append("chara1") + Transform::append() + Material::append(showcase.find(L"hedgehog.png")) + Animator::append() + Shape::append<ShapeSprite>("sys:renderer2d")
		));
		newScene->appendChild(new Actor(
			Actor::noAction,
			loadProps(showcase.find(L"chara2.json")) + Indexer::append("chara2") + Transform::append() + Material::append(showcase.find(L"hedgehog.png")) + Animator::append() + Shape::append<ShapeSprite>("sys:renderer2d")
		));
		newScene->appendChild(new Actor(
			Actor::noAction,
			loadProps(showcase.find(L"chara3.json")) + Indexer::append("chara3") + Transform::append() + Material::append(showcase.find(L"hedgehog.png")) + Animator::append() + Shape::append<ShapeSprite>("sys:renderer2d")
		));

		Finder<Actor>::find("sys:scene", [&](Actor &scene) {
			scene.appendChild(newScene);
		});
	}

	function<void(Actor &, Animator &)> replay(Asset &asset, const string &animname, const string &slotname) {
		return [&asset, animname, slotname](Actor &, Animator &animator) {
			animator.replay(asset, animname, slotname);
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
