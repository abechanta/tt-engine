#include <actor.h>
#include <components/indexer_component.h>
#include <finder.h>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial7 {
	void test1() {
		cout << __FUNCTION__ << endl;
		Actor a(Actor::noAction, [](Actor &a) {
			a.props().put<string>("name", "a");
		});
		cout << "--- find1" << endl;
		{
			auto indexer1 = make_unique<Indexer>("actor-a", a);
			Finder<Actor>::find("actor-a", [](Actor &a) {
				auto name = a.get<string>("name", "");
				cout << "\t" << "actor-a(name=" << name << ") found." << endl;
			});
		}
		cout << "--- find2" << endl;
		{
			Finder<Actor>::find("actor-a", [](Actor &a) {
				auto name = a.get<string>("name", "");
				cout << "\t" << "actor-a(name=" << name << ") found." << endl;
			});
		}
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		{
			cout << "--- ctor" << endl;
			auto a = make_unique<Actor>(Actor::noAction, Indexer::append("p1"));
			auto b = make_unique<Actor>(Actor::noAction, Indexer::append("p2"));
			cout << "--- find1" << endl;
			Finder<Actor>::find("p2", [](Actor &) { cout << "\t" << "p2 found." << endl; });
			Finder<Actor>::find("p3", [](Actor &) { cout << "\t" << "p3 found." << endl; });
			cout << "--- dtor" << endl;
		}
		cout << "--- find2" << endl;
		Finder<Actor>::find("p2", [](Actor &) { cout << "\t" << "p2 found." << endl; });
	}
}

extern "C" int tutorial7() {
	Tutorial7::test1();
	Tutorial7::test2();
	return 0;
}
