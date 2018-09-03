#include <actor.h>
#include <components/material_component.h>
#include <asset.h>
#include <asset_handler.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <boost/property_tree/ptree.hpp>
using namespace tte;

namespace Tutorial7 {
	void test1() {
		cout << __FUNCTION__ << endl;
		auto asset1 = make_unique<Asset>(L"asset/tutorial7/font1.png", AssetHandler::initializerUnknown);
		cout << "--- load" << endl;
		asset1->load();
		auto material1 = make_unique<Material>(*asset1, vector4{ 1, 1, 1, 0.5f, }, vector2{ 0.f, 0.f, }, vector2{ 0.5f, 0.5f, });
		cout << "--- diffuse" << endl;
		vector4 d = material1->diffuse();
		cout << "d=" << X(d) << " " << Y(d) << " " << Z(d) << " " << W(d) << endl;
		cout << "--- uv" << endl;
		vector2 uv0 = material1->uv0();
		vector2 uv1 = material1->uv1();
		cout << "uv[0]=" << X(uv0) << " " << Y(uv0) << endl;
		cout << "uv[1]=" << X(uv1) << " " << Y(uv1) << endl;
		cout << "--- unload" << endl;
		asset1->unload();
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		auto assetImg = make_unique<Asset>(L"asset/tutorial7/font1.png", AssetHandler::initializerUnknown);
		auto assetProp = make_unique<Asset>(L"asset/tutorial7/tutorial7.json", AssetHandler::initializerJson);
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
		actor1->findComponent<Material>([](Material &material) {
			vector4 d = material.diffuse();
			cout << "d=" << X(d) << " " << Y(d) << " " << Z(d) << " " << W(d) << endl;
			vector2 uv0 = material.uv0();
			vector2 uv1 = material.uv1();
			cout << "uv[0]=" << X(uv0) << " " << Y(uv0) << endl;
			cout << "uv[1]=" << X(uv1) << " " << Y(uv1) << endl;
		});
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial7() {
	Tutorial7::test1();
	Tutorial7::test2();
	return 0;
}
