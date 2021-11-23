#include <actor.h>
#include <asset_handler.h>
#include <geometry.h>
#include <components/transform_component.h>
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial6 {
	void test1() {
		cout << __FUNCTION__ << endl;
		property_tree::ptree pt;
		auto transform1 = make_unique<Transform>(pt, vector3{ 1, 2, 3, }, vector3{ 4, 5, 6, }, vector3{ 7, 8, 9, });
		cout << "--- translation" << endl;
		vector3 t = transform1->translation();
		cout << X(t) << " " << Y(t) << " " << Z(t) << endl;
		cout << "--- rotation" << endl;
		vector3 r = transform1->rotation();
		cout << X(r) << " " << Y(r) << " " << Z(r) << endl;
		cout << "--- scaling" << endl;
		vector3 s = transform1->scaling();
		cout << X(s) << " " << Y(s) << " " << Z(s) << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		auto asset1 = make_unique<Asset>(L"asset/tutorial6.json", AssetHandler::typeJson);
		cout << "--- load" << endl;
		asset1->load();
		cout << "--- ctor" << endl;
		auto actor1 = make_unique<Actor>(
			Actor::noAction,
			[&asset1](Actor &a) {
				a.importProps(asset1->props());
				Transform::append()(a);
			}
		);
		cout << "--- unload" << endl;
		asset1->unload();
		cout << "--- transformation" << endl;
		actor1->getComponent<Transform>([](Transform &transform) {
			vector3 &t = transform.translation();
			vector3 p = { 1.f, 2.f, 3.f, };
			vector3 q = Geometry::pos(transform.trs2d(Geometry::identity(matrix3x4())), p);
			cout << "t=" << X(t) << " " << Y(t) << " " << Z(t) << endl;
			cout << "q=" << X(q) << " " << Y(q) << " " << Z(q) << endl;
		});
	}
}

extern "C" int tutorial6() {
	Tutorial6::test1();
	Tutorial6::test2();
	return 0;
}
