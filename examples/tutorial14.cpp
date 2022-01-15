#include <actor.h>
#include <asset.h>
#include <asset_handler.h>
#include <components/animator_component.h>
#include <components/indexer_component.h>
#include <components/material_component.h>
#include <components/resource_component.h>
#include <components/prefab_component.h>
#include <components/shape_component.h>
#include <components/transform_component.h>
#include <helpers/actor_actions.h>
#include <helpers/actor_modifiers.h>
#include <helpers/actor_triggers.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial14 {
	void test1() {
		cout << __FUNCTION__ << endl;
		AssetHandler assetHandler;
		assetHandler.clear();
		assetHandler.append({ L"<undef>", AssetHandler::typeUnknown, });
		assetHandler.append({ L".png", AssetHandler::typeJson, });
		assetHandler.append({ L".json", AssetHandler::typeJson, });
		assetHandler.append({ L".anim", AnimationSet::typeAnim, });
		assetHandler.append({ L"", AssetHandler::typeDir, });
		auto assets = make_unique<Asset>(L"asset/tutorial13", assetHandler);
		cout << "--- load" << endl;
		auto &root = assets->find(L"root.json");
		root.load();
		cout << "--- ctor" << endl;
		auto actors = make_unique<Actor>(
			Actor::noAction,
			[&assets, &root](Actor &a) {
				a.importProps(root.props());
				Indexer::append()(a);
				Prefab::append(*assets)(a);
			}
		);

		cout << "--- appendAction" << endl;
		Finder<Actor>::find("p1", [&assets](Actor &p1) {
			p1.appendAction([&assets](Actor &p1) {
				if (p1.props().get<string>("state", "") == "standing") {
					Finder<Actor>::find("sys:input", [&assets, &p1](Actor &in) {
						in.getComponent<Input>([&assets, &p1](auto &input) {
							if (input.buttons("jump").pressed()) {
								p1.getComponent<Animator>([&assets](auto &animator) {
									animator.replay(assets->find(L"scene2/falling.anim"), "falling-delta");
								});
								p1.props().put<string>("state", "jumping");
							}
						});
					});
				}
			});
		});

		cout << "--- unload" << endl;
		root.unload();
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		AssetHandler assetHandler;
		assetHandler.clear();
		assetHandler.append({ L"<undef>", AssetHandler::typeUnknown, });
		assetHandler.append({ L".png", AssetHandler::typeJson, });
		assetHandler.append({ L".json", AssetHandler::typeJson, });
		assetHandler.append({ L".anim", AnimationSet::typeAnim, });
		assetHandler.append({ L"", AssetHandler::typeDir, });
		auto assets = make_unique<Asset>(L"asset/tutorial13", assetHandler);
		cout << "--- load" << endl;
		auto &root = assets->find(L"root.json");
		root.load();
		cout << "--- ctor" << endl;
		auto actors = make_unique<Actor>(
			Actor::noAction,
			loadProps(root) + Indexer::append() + Prefab::append(*assets)
		);

		cout << "--- appendAction" << endl;
		appendActionTo("p1",
			inState("standing") * onButtonPressed("jump") * (
				withComponent<Animator>([&assets](Actor &p1, auto &animator) {
					animator.replay(assets->find(L"scene2/falling.anim"), "falling-delta");
				}) +
				changeState("jumping")
			)
		);

		cout << "--- unload" << endl;
		root.unload();
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial14() {
	// Tutorial14::test1();	// same as Tutorial13::test1()
	Tutorial14::test2();
	return 0;
}
