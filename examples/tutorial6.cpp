#include <actor.h>
#include <asset_handler.h>
#include <components/transform_component.h>
#include <geometry.h>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial6 {
	void test1() {
		cout << __FUNCTION__ << endl;
		ptree props;
		auto transform1 = make_unique<Transform>(props, vector3{ 1, 2, 3, }, vector3{ 4, 5, 6, }, vector3{ 7, 8, 9, });
		cout << "--- transformation" << endl;
		cout << transform1->translation << endl;
		cout << transform1->rotation << endl;
		cout << transform1->scaling << endl;
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
			vector3 t = transform.translation();
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
