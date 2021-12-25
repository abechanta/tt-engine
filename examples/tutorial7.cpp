#include <actor.h>
#include <asset.h>
#include <asset_handler.h>
#include <components/material_component.h>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial7 {
	void test1() {
		cout << __FUNCTION__ << endl;
		auto asset1 = make_unique<Asset>(L"asset/tutorial7/font1.png", AssetHandler::typeJson);
		cout << "--- load" << endl;
		asset1->load();
		ptree props;
		auto material1 = make_unique<Material>(*asset1, props, vector4{ 1, 1, 1, 0.5f, }, vector2{ 0.f, 0.f, }, vector2{ 0.5f, 0.5f, });
		cout << "--- material" << endl;
		cout << material1->diffuse << endl;
		cout << material1->uv0 << endl;
		cout << material1->uv1 << endl;
		cout << "--- unload" << endl;
		asset1->unload();
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		auto assetImg = make_unique<Asset>(L"asset/tutorial7/font1.png", AssetHandler::typeJson);
		auto assetProp = make_unique<Asset>(L"asset/tutorial7/tutorial7.json", AssetHandler::typeJson);
		cout << "--- load" << endl;
		assetImg->load();
		assetProp->load();
		auto actor1 = make_unique<Actor>(
			Actor::noAction,
			[&assetImg, &assetProp](Actor &a) {
				a.importProps(assetProp->props());
				Material::append(*assetImg)(a);
			}
		);
		cout << "--- unload" << endl;
		assetProp->unload();
		assetImg->unload();
		cout << "--- material" << endl;
		actor1->getComponent<Material>([](Material &material) {
			cout << material.diffuse << endl;
			cout << material.uv0 << endl;
			cout << material.uv1 << endl;
		});
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial7() {
	Tutorial7::test1();
	Tutorial7::test2();
	return 0;
}
