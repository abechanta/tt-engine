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
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial13 {
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
		cout << "--- ctor" << endl;
		auto actors = make_unique<Actor>(
			Actor::noAction,
			Indexer::append("root")
		);
		actors->appendChild(new Actor(
			Actor::noAction,
			Resource::append(assets->find(L"scene1"))
		));
		actors->appendChild(new Actor(
			Actor::noAction,
			[&assets](Actor &a) {
				a.importProps(assets->find(L"scene1/hedgehog1.json").props());
				Indexer::append("p1")(a);
				Transform::append()(a);
				Material::append(assets->find(L"scene1/hedgehog.png"))(a);
				Shape::append<ShapeSprite>()(a);
				Animator::append()(a);
			}
		));
		actors->appendChild(new Actor(
			Actor::noAction,
			[&assets](Actor &a) {
				a.importProps(assets->find(L"scene1/hedgehog2.json").props());
				Indexer::append("p2")(a);
				Transform::append()(a);
				Material::append(assets->find(L"scene1/hedgehog.png"))(a);
				Shape::append<ShapeSprite>()(a);
				Animator::append()(a);
			}
		));
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
			[&assets, &root](Actor &a) {
				a.importProps(root.props());
				Indexer::append()(a);
				Prefab::append(*assets)(a);
			}
		);
		cout << "--- unload" << endl;
		root.unload();
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial13() {
	Tutorial13::test1();
	Tutorial13::test2();
	return 0;
}
