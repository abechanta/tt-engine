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

namespace Tutorial9 {
	void test1() {
		cout << __FUNCTION__ << endl;
		auto img = make_unique<Asset>(L"asset/tutorial9/p1.png", AssetHandler::typeJson);
		cout << "--- load" << endl;
		img->load();
		ptree props;
		auto material1 = make_unique<Material>(
			*img, props, vector4{ 1.f, 1.f, 1.f, 0.5f, }, vector2{ 0.f, 0.f, }, vector2{ 0.5f, 0.5f, }
		);
		cout << "--- unload" << endl;
		img->unload();
		cout << "--- material" << endl;
		cout << "\t" << material1->diffuse << endl;
		cout << "\t" << material1->uv0 << endl;
		cout << "\t" << material1->uv1 << endl;
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		auto a = make_unique<Actor>(
			Actor::noAction,
			[](Actor &a) {
				auto img = make_unique<Asset>(L"asset/tutorial9/p1.png", AssetHandler::typeJson);
				auto def = make_unique<Asset>(L"asset/tutorial9/p1.json", AssetHandler::typeJson);
				def->load();
				a.importProps(def->props());
				def->unload();
				img->load();
				Material::append(*img)(a);
				img->unload();
			}
		);
		cout << "--- material" << endl;
		a->getComponent<Material>([](auto &material) {
			cout << "\t" << material.diffuse << endl;
			cout << "\t" << material.uv0 << endl;
			cout << "\t" << material.uv1 << endl;
		});
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial7() {
	Tutorial9::test1();
	Tutorial9::test2();
	return 0;
}
