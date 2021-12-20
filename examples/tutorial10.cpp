#include <actor.h>
#include <helpers/actor_modifiers.h>
#include <helpers/actor_triggers.h>
#include <asset.h>
#include <asset_handler.h>
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
		AssetHandler::append({ L"<undef>", AssetHandler::typeUnknown, });
		AssetHandler::append({ L".json", AssetHandler::typeJson, });
		AssetHandler::append({ L".anim", AssetHandler::typeAnim, });
		AssetHandler::append({ L"", AssetHandler::typeDir, });
		auto assetRoot = make_unique<Asset>(L"asset/tutorial10", AssetHandler::factory(L""));
		cout << "--- ctor" << endl;
		auto actor1 = make_unique<Actor>(
			[&assetRoot](Actor &a) {
				a.getComponent<Input>([&assetRoot, &a](auto &input) {
					if (input.buttons("down").pressed()) {
						a.getComponent<Animator>([&assetRoot](auto &animator) {
							animator.replay(assetRoot->find(L"tutorial10.anim"), "0");
						});
					}
				});
			},
			[&assetRoot](Actor &a) {
				Resource::append(*assetRoot)(a);
				a.importProps(assetRoot->find(L"tutorial10.json").props());
				Animator::append()(a);
			}
		);
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		AssetHandler::clear();
		AssetHandler::append({ L"<undef>", AssetHandler::typeUnknown, });
		AssetHandler::append({ L".json", AssetHandler::typeJson, });
		AssetHandler::append({ L".anim", AssetHandler::typeAnim, });
		AssetHandler::append({ L"", AssetHandler::typeDir, });
		auto assetRoot = make_unique<Asset>(L"asset/tutorial10", AssetHandler::factory(L""));
		cout << "--- ctor" << endl;
		auto actor1 = make_unique<Actor>(
			onButtonPressed("down") * withComponent<Animator>([&assetRoot](Actor &, auto &animator) {
				animator.replay(assetRoot->find(L"tutorial10.anim"), "0");
			}),
			Resource::append(*assetRoot) +
			loadProps(assetRoot->find(L"tutorial10.json")) +
			Animator::append()
		);
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial10() {
	Tutorial10::test1();
	Tutorial10::test2();
	return 0;
}
