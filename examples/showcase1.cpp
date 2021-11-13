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
			inState("7") * onButtonPressed("down") * (changeState("8") + componentModifier<Animator>("chara3", play<Animator>("left"))) +
			inState("6") * onButtonPressed("down") * (changeState("7") + componentModifier<Animator>("chara2", play<Animator>("left"))) +
			inState("5") * onButtonPressed("down") * (changeState("6") + componentModifier<Animator>("chara1", play<Animator>("left"))) +
			inState("4") * onButtonPressed("down") * (changeState("5") + componentModifier<Animator>("chara0", play<Animator>("left"))) +
			inState("3") * onButtonPressed("down") * (changeState("4") + componentModifier<Animator>("chara3", play<Animator>("right"))) +
			inState("2") * onButtonPressed("down") * (changeState("3") + componentModifier<Animator>("chara2", play<Animator>("right"))) +
			inState("1") * onButtonPressed("down") * (changeState("2") + componentModifier<Animator>("chara1", play<Animator>("right"))) +
			inState("0") * onButtonPressed("down") * (changeState("1") + componentModifier<Animator>("chara0", play<Animator>("right"))) +
			inState("8") * changeState("0"),
			put<string>("state", "0")
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

	function<void(Actor&)> renderAsTilemap() {
		return componentModifier<Renderer2d>("renderer:", [](Actor& a, auto& renderer) {
			a.findComponent<Transform, Material>([&a, &renderer](auto& transform, auto& material) {
				const vector3 translation = transform.translation();
				const vector2 uv0 = material.uv0();
				const vector2 uv1 = material.uv1();
				{
					const vector2i size = Geometry::get<vec, int32_t, 2>(a.props("size"), 8);
					const vector2i mapsize = Geometry::get<vec, int32_t, 2>(a.props("tilemap.size"), 16);
					const vector2i cellsize = Geometry::get<vec, int32_t, 2>(a.props("tilemap.cell"), 8);
					const int32_t cellBounds = X(material.size()) / X(cellsize);

					for (int32_t y = 0; y < Y(mapsize); y++) {
						transform.translation() = translation + _0X0<int32_t>(y * Y(size));
						const vector<int32_t> vert = Geometry::get<vector, int32_t>(a.props("tilemap.tiles." + to_string(y)));
						for (int32_t x = 0; x < X(mapsize); x++) {
							auto code = vert[x];
							material.uv0() = material.to_vector2({ ((code % cellBounds) + 0) * X(cellsize), ((code / cellBounds) + 0) * Y(cellsize), });
							material.uv1() = material.to_vector2({ ((code % cellBounds) + 1) * X(cellsize), ((code / cellBounds) + 1) * Y(cellsize), });
							renderer.render(a);
							X(transform.translation()) += X(size);
						}
					}
				}
				transform.translation() = translation;
				material.uv0() = uv0;
				material.uv1() = uv1;
			});
		});
	}

	template<typename Tc>
	function<void(Actor &, Tc &)> play(const string &arg1) {
		return [arg1](Actor &, Tc &component) {
			component.play(arg1);
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
