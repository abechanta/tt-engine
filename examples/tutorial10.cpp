#include <actor.h>
#include <actor_modifiers.h>
#include <actor_triggers.h>
#include <asset.h>
#include <asset_handler.h>
#include <button.h>
#include <geometry.h>
#include <components/animator_component.h>
#include <components/indexer_component.h>
#include <components/material_component.h>
#include <components/resource_component.h>
#include <components/transform_component.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial10 {
	void test1() {
		cout << __FUNCTION__ << endl;
		AssetHandler::clear();
		AssetHandler::appendInitializer({ L"<undef>", AssetHandler::initializerUnknown, });
		AssetHandler::appendInitializer({ L".json", AssetHandler::initializerJson, });
		AssetHandler::appendInitializer({ L"", AssetHandler::initializerDir, });
		auto assetRoot = make_unique<Asset>(L"asset/tutorial10", AssetHandler::factory(L""));
		cout << "--- ctor" << endl;
		auto actor1 = make_unique<Actor>(
			[](Actor &a) {
				a.findComponent<Input>([&a](auto &input) {
					if (input.buttons("down").pressed()) {
						a.findComponent<Animator>([](auto &animator) {
							animator.play("0");
						});
					}
				});
			},
			[&assetRoot](Actor &a) {
				Resource::append(*assetRoot)(a);
				a.importProps(assetRoot->find(L"tutorial10.json").props());
				Animator::append(assetRoot->find(L"tutorial10.anim.json"))(a);
			}
		);
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		AssetHandler::clear();
		AssetHandler::appendInitializer({ L"<undef>", AssetHandler::initializerUnknown, });
		AssetHandler::appendInitializer({ L".json", AssetHandler::initializerJson, });
		AssetHandler::appendInitializer({ L"", AssetHandler::initializerDir, });
		auto assetRoot = make_unique<Asset>(L"asset/tutorial10", AssetHandler::factory(L""));
		cout << "--- ctor" << endl;
		auto actor1 = make_unique<Actor>(
			onButtonPressed("down") * componentModifier<Animator>([](Actor &, auto &animator) {
				animator.play("0");
			}),
			Resource::append(*assetRoot) +
			loadProps(assetRoot->find(L"tutorial10.json")) +
			Animator::append(assetRoot->find(L"tutorial10.anim.json"))
		);
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial10() {
	Tutorial10::test1();
	Tutorial10::test2();
	return 0;
}
