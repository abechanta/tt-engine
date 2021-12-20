#include <actor.h>
#include <asset.h>
#include <asset_handler.h>
#include <components/animator_component.h>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial8 {
	void test1() {
		cout << __FUNCTION__ << endl;
		auto asset1 = make_unique<Asset>(L"asset/tutorial8.anim", AnimationSet::typeAnim);
		cout << "--- load" << endl;
		asset1->load();
		cout << "--- ctor" << endl;
		ptree props1;
		auto animationFalling = asset1->handle<AnimationSet>()->get("falling");
		auto timeline1 = make_unique<Timeline>(props1, animationFalling, true);
		cout << "--- replay" << endl;
		timeline1->replay();
		cout << "--- tick" << endl;
		for (auto i = 0; i < 12; i++) {
			timeline1->tick(i);
			cout << props1.get<float>("transform.translation.y", 0.f) << endl;
		}
		cout << "--- unload" << endl;
		asset1->unload();
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		auto asset1 = make_unique<Asset>(L"asset/tutorial8.anim", AnimationSet::typeAnim);
		cout << "--- load" << endl;
		asset1->load();
		cout << "--- ctor" << endl;
		auto actor1 = make_unique<Actor>(
			[](Actor &a) {
				cout << a.get<float>("transform.translation.y", 100.f) << endl;
			},
			[](Actor &a) {
				Animator::append()(a);
			}
		);
		cout << "--- replay" << endl;
		actor1->getComponent<Animator>([&asset1](auto &animator) {
			animator.replay(*asset1, "falling");
		});
		cout << "--- tick" << endl;
		for (auto i = 0; i < 12; i++) {
			actor1->act();
		}
		cout << "--- unload" << endl;
		asset1->unload();
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial8() {
	Tutorial8::test1();
	Tutorial8::test2();
	return 0;
}
