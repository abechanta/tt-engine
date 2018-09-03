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
#include <iostream>
#include <memory>
using namespace tte;

class TutorialSdl : public App {
private:
	std::unique_ptr<sdl2::Adapter> &m_adapter;
	std::unique_ptr<Asset> m_assetRoot;
	std::unique_ptr<Actor> m_actors;

public:
	TutorialSdl() : App(), m_adapter(sdl2::Adapter::create()), m_assetRoot(), m_actors() {
		cout << __FUNCTION__ << endl;
		AssetHandler::clear();
		AssetHandler::appendInitializer({ AssetHandler::extensionUnknown, AssetHandler::initializerUnknown, });
		AssetHandler::appendInitializer({ L".json", AssetHandler::initializerJson, });
		AssetHandler::appendInitializer({ L".png", sdl2::Adapter::initializerPng(*m_adapter), });
		AssetHandler::appendInitializer({ L"", AssetHandler::initializerDir, });
		m_assetRoot = std::make_unique<Asset>(L"asset", AssetHandler::factory(L"asset:"));
	}

	virtual ~TutorialSdl() override {
		cout << __FUNCTION__ << endl;
		m_assetRoot.reset();
		m_adapter.reset();
		AssetHandler::clear();
	}

	virtual void initialize() override {
		cout << __FUNCTION__ << endl;

		auto &config = m_assetRoot->find(L"config.json");
		config.load();
		auto root = new Actor(Actor::noAction, Indexer::append("sys:root"));
		auto assetInstance = new Actor(Actor::noAction, Indexer::append("sys:assetInstance"));
		auto window = m_adapter->createWindowActor(config, Indexer::append("window:"));
		auto input = m_adapter->createInputActor(config, Indexer::append("input:"));
		auto inputKbd = m_adapter->createKeyboardActor(config, Actor::noAction);
		auto gameSequence = new Actor(Actor::noAction, Indexer::append("sys:gameSequence"));
		auto renderer = m_adapter->createRendererActor(config, Indexer::append("renderer:"));
#if 1
		auto timeline = new Timeline(config.props().get_child("animation.0"));
		timeline->play();
		property_tree::ptree out;
		for (int i = 0; i < 30; i++) {
			timeline->tick(10 + i, out);
			cout << "i:" << i << " ";
			write_json(cout, out);
		}
		delete timeline;
#endif
		config.unload();

		root->appendChild(window);
		root->appendChild(input->appendChild(inputKbd));
		root->appendChild(assetInstance);
		root->appendChild(gameSequence);
		root->appendChild(renderer);
		m_actors.reset(root);

		auto boot = new Actor(Actor::noAction, Indexer::append("asset:boot") + Resource::append(m_assetRoot->find(L"boot")));
		assetInstance->appendChild(boot);
		auto splashImage = new Actor(
			onEvent("pressed") * [](Actor &a) {
				cout << "pressed" << endl;
			} +
			onButtonPressed("up") * notifyEvent("pressed") +
			onButtonPressed("left") * componentModifier<Animator>([](Actor &a, auto &animator) {
				animator.play("left");
			}) +
			onButtonPressed("right") * componentModifier<Animator>([](Actor &a, auto &animator) {
				animator.play("right");
			}) +
			//[](Actor &a) {
			//	Finder<Actor>::find<Input>("input:", [&a](auto &input) {
			//		if (input.buttons("left").on()) {
			//			Transform::find(a, [](auto &transform) {
			//				Z(transform.rotation()) -= 0.1f;
			//			});
			//		}
			//		if (input.buttons("right").on()) {
			//			Transform::find(a, [](auto &transform) {
			//				Z(transform.rotation()) += 0.1f;
			//			});
			//		}
			//	});
			//} +
			onButtonOn("up") * componentModifier<Transform>([](Actor &, auto &transform) {
				Y(transform.translation()) -= 1;
			}) +
			onButtonOn("down") * componentModifier<Transform>([](Actor &, auto &transform) {
				Y(transform.translation()) += 1;
			}) +
			[](Actor &a) {
				a.transform<bool, float>("transform.rotation.z", false, [&a](float z) -> bool {
					a.findComponent<Transform>([z](auto &transform) {
						Z(transform.rotation()) = z;
					});
					return true;
				});
			} +
			[](Actor &a) {
				Finder<Actor>::find<Renderer2d>("renderer:", [&a](auto &renderer) {
					renderer.render(a);
				});
			},
			loadProps(Resource::find(*boot, L"splash.json")) +
			Transform::append() +
			Material::append(Resource::find(*boot, L"splash.png")) +
			Primitive::append({ 1, 1, }, { 254, 238, }) +
			Animator::append(Resource::find(*boot, L"splash.anim.json"))
		);
		renderer->appendChild(splashImage);
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
		for (auto &a : *m_actors) {
			a.act();
		}
	}
};

extern "C" int tutorial() {
	cout << __FUNCTION__ << endl;
	{
		auto app = make_unique<TutorialSdl>();
		App::Runner runner(std::move(app));
		runner.run();
	}
	return 0;
}
