#include <actor.h>
#include <components/animator_component.h>
#include <asset.h>
#include <asset_handler.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <boost/property_tree/ptree.hpp>
using namespace tte;

namespace Tutorial8 {
	void test1() {
		cout << __FUNCTION__ << endl;
		auto asset1 = make_unique<Asset>(L"asset/tutorial8.json", AssetHandler::initializerJson);
		cout << "--- load" << endl;
		asset1->load();
		cout << "--- ctor" << endl;
		auto timeline1 = make_unique<Timeline>(asset1->props().get_child("animation.falling"));
		cout << "--- unload" << endl;
		asset1->unload();
		cout << "--- play" << endl;
		timeline1->play();
		cout << "--- tick" << endl;
		for (auto i = 0; i < 12; i++) {
			property_tree::ptree props1;
			timeline1->tick(i, props1);
			cout << props1.get<float>("transform.translation.y", 0.f) << endl;
		}
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		auto asset1 = make_unique<Asset>(L"asset/tutorial8.json", AssetHandler::initializerJson);
		cout << "--- load" << endl;
		asset1->load();
		cout << "--- ctor" << endl;
		auto actor1 = make_unique<Actor>(
			Actor::noAction,
			[&asset1](Actor &a) {
				Animator::append(*asset1)(a);
				a.appendAction([](Actor &a) {
					cout << a.get<float>("transform.translation.y", 100.f) << endl;
				});
			}
		);
		cout << "--- unload" << endl;
		asset1->unload();
		cout << "--- play" << endl;
		actor1->getComponent<Animator>([](auto &animator) {
			animator.play("falling");
		});
		cout << "--- tick" << endl;
		for (auto i = 0; i < 12; i++) {
			actor1->act();
		}
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial8() {
	Tutorial8::test1();
	Tutorial8::test2();
	return 0;
}
