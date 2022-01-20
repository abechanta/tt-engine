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

namespace Tutorial11 {
	void test1() {
		cout << __FUNCTION__ << endl;
		cout << "--- load" << endl;
		auto animset = make_unique<Asset>(L"asset/tutorial11/falling.anim", AnimationSet::typeAnim);
		animset->load();
		ptree props1;
		auto animToReplay = animset->handle<AnimationSet>()->get("falling-overwrite");
		auto timeline1 = make_unique<Timeline>(props1, animToReplay, true);
		timeline1->pause(false);
		cout << "--- tick" << endl;
		for (auto i = 0; i < 10; i++) {
			timeline1->tick(i);
			cout << "\t" << props1.get<float>("transform.translation.y", 100.f);
		}
		cout << endl;
		cout << "--- unload" << endl;
		animset->unload();
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		cout << "--- load" << endl;
		auto animset = make_unique<Asset>(L"asset/tutorial11/falling.anim", AnimationSet::typeAnim);
		animset->load();
		ptree props1;
		auto animator1 = make_unique<Animator>(props1);
		animator1->replay(*animset, "falling-overwrite");
		cout << "--- tick" << endl;
		for (auto i = 0; i < 10; i++) {
			animator1->tick(i);
			cout << "\t" << props1.get<float>("transform.translation.y", 100.f);
		}
		cout << endl;
		cout << "--- unload" << endl;
		animset->unload();
		cout << "--- dtor" << endl;
	}

	void test3() {
		cout << __FUNCTION__ << endl;
		cout << "--- load" << endl;
		auto animset = make_unique<Asset>(L"asset/tutorial11/falling.anim", AnimationSet::typeAnim);
		animset->load();
		auto a = make_unique<Actor>(Actor::noAction, Animator::append());
		a->appendAction([](Actor &a) {
			cout << "\t" << a.get<float>("transform.translation.y", 100.f);
		});
		a->getComponent<Animator>([&animset](auto &animator) {
			animator.replay(*animset, "falling-delta");
		});
		cout << "--- tick" << endl;
		for (auto i = 0; i < 10; i++) {
			a->act();
		}
		cout << endl;
		cout << "--- unload" << endl;
		animset->unload();
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial11() {
	Tutorial11::test1();
	Tutorial11::test2();
	Tutorial11::test3();
	return 0;
}
