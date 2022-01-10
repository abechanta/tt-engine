#include <actor.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial4 {
	void test1() {
		cout << __FUNCTION__ << endl;
		Actor a;
		a.appendAction([](Actor &) { cout << "\t" << "Hello world." << endl; });
		a.appendAction([](Actor &) { cout << "\t" << "Hello world, again." << endl; });
		cout << "--- act" << endl;
		a.act();
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		Actor *a = new Actor([](Actor &) { cout << "\t" << "Hello world, from a." << endl; });
		Actor *b = new Actor([](Actor &) { cout << "\t" << "Hello world, from b." << endl; });
		a->appendChild(b);
		cout << "--- act" << endl;
		for (auto &p : *a) {
			p.act();
		}
		cout << "--- dtor" << endl;
		delete a;
	}

	void test3() {
		Actor::Action printName = [](Actor &a) {
			cout << "\t" << static_cast<void *>(&a) << ":" << a.get<string>("name", "<undef>") << endl;
		};

		auto putName = [](const string &key, const string &value) -> Actor::Action {
			return [key, value](Actor &a) {
				a.props().put<string>(key, value);
			};
		};

		cout << __FUNCTION__ << endl;
		Actor *a = new Actor(printName, putName("name", "a"));
		a->appendAction([](Actor &a) {
			for_each(a.begin(true), a.end(), [](Actor &a) {
				a.act();
			});
		});
		Actor *b = new Actor(printName, putName("name", "b"));
		Actor *c = new Actor(printName, putName("name", "c"));
		Actor *d = new Actor(printName, putName("name", "d"));
		Actor *e = new Actor(printName, putName("name", "e"));
		Actor *f = new Actor(printName, putName("name", "f"));
		Actor *g = new Actor(printName, putName("name", "g"));

		a->appendChild(
			b->appendChild(c)->appendChild(d)
		)->appendChild(
			e->appendChild(
				f->appendChild(g)
			)
		);

		cout << "--- act" << endl;
		a->act();
		cout << "--- dtor" << endl;
		delete a;
	}
}

extern "C" int tutorial4() {
	Tutorial4::test1();
	Tutorial4::test2();
	Tutorial4::test3();
	return 0;
}
