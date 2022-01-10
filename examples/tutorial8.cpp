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

namespace Tutorial8 {
	void test1() {
		cout << __FUNCTION__ << endl;
		ptree props;
		auto transform1 = make_unique<Transform>(
			props, vector3{ 1, 2, 3, }, vector3{ 4, 5, 6, }, vector3{ 7, 8, 9, }
		);
		cout << "--- transformation" << endl;
		cout << "\t" << transform1->translation << endl;
		cout << "\t" << transform1->rotation << endl;
		cout << "\t" << transform1->scaling << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		cout << "--- ctor" << endl;
		auto a = make_unique<Actor>(
			Actor::noAction,
			[](Actor &a) {
				auto a1 = make_unique<Asset>(L"asset/tutorial8/arrow.json", AssetHandler::typeJson);
				a1->load();
				a.importProps(a1->props());
				a1->unload();
				Transform::append()(a);
			}
		);
		cout << "--- transformation" << endl;
		a->getComponent<Transform>([](auto &transform) {
			vector3 t = transform.translation.get();
			vector3 p = { 1.f, 2.f, 3.f, };
			matrix3x4 m = Geometry::identity(matrix3x4());
			vector3 q = Geometry::pos(transform.trs2d(m), p);
			cout << "\t" << "t=" << X(t) << " " << Y(t) << " " << Z(t) << endl;
			cout << "\t" << "q=" << X(q) << " " << Y(q) << " " << Z(q) << endl;
		});
	}
}

extern "C" int tutorial8() {
	Tutorial8::test1();
	Tutorial8::test2();
	return 0;
}
