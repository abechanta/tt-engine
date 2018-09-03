#include <actor.h>
#include <finder.h>
#include <components/indexer_component.h>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial5 {
	void test1() {
		cout << __FUNCTION__ << endl;
		auto actor1 = make_unique<Actor>(Actor::noAction, Actor::noAction);
		{
			auto indexer1 = make_unique<Indexer>("actor1", *actor1);
			cout << "--- find1" << endl;
			Finder<Actor>::find("actor1", [](Actor &) {
				cout << "actor1 found." << endl;
			});
		}
		{
			cout << "--- find2" << endl;
			Finder<Actor>::find("actor1", [](Actor &) {
				cout << "actor1 found." << endl;
			});
		}
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		{
			cout << "--- ctor" << endl;
			auto actor1 = make_unique<Actor>(Actor::noAction, Indexer::append("actor1"));
			auto actor2 = make_unique<Actor>(Actor::noAction, Indexer::append("actor2"));
			auto actor3 = make_unique<Actor>(Actor::noAction, Indexer::append("actor3"));
			cout << "--- find1" << endl;
			Finder<Actor>::find("actor2", [](Actor &) {
				cout << "actor2 found." << endl;
			});
			Finder<Actor>::find("actor4", [](Actor &) {
				cout << "actor4 found." << endl;
			});
			cout << "--- dtor" << endl;
		}
		{
			cout << "--- find2" << endl;
			Finder<Actor>::find("actor2", [](Actor &) {
				cout << "actor2 found." << endl;
			});
		}
		cout << endl;
	}
}

extern "C" int tutorial5() {
	Tutorial5::test1();
	Tutorial5::test2();
	return 0;
}
