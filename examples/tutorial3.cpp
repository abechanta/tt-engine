#include <actor.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial3 {
	void test1() {
		cout << __FUNCTION__ << endl;
		Actor a([](Actor &) {}, [](Actor &) {});
		a.appendAction([](Actor &) { cout << "a.action1" << endl; });
		a.appendAction([](Actor &) { cout << "a.action2" << endl; });
		cout << "--- act" << endl;
		a.act();
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		Actor *a = new Actor([](Actor &) { cout << "a.action1" << endl; }, [](Actor &) {});
		Actor *b = new Actor([](Actor &) { cout << "b.action1" << endl; }, [](Actor &) {});
		a->appendChild(b);
		cout << "--- act" << endl;
		for (auto &p : *a) {
			p.act();
		}
		cout << "--- dtor" << endl;
		delete a;
	}

	void test3() {
		Actor::Action action = [](const Actor &a) {
			cout << static_cast<const void *>(&a) << ":" << a.get<string>("name", "<undef>")
				<< endl;
		};

		auto put = [](const string &key, const string &value) -> Actor::Action {
			return [key, value](Actor &a) {
				a.props().put<string>(key, value);
			};
		};

		cout << __FUNCTION__ << endl;
		Actor *a = new Actor(
			[&](Actor &a) {
				cout << "---" << endl;
				action(a);
				for_each(a.begin(true), a.end(), [](Actor &a) {
					a.act();
				});
			},
			put("name", "a")
		);
		Actor *b = new Actor(action, put("name", "b"));
		Actor *c = new Actor(action, put("name", "c"));
		Actor *d = new Actor(action, put("name", "d"));
		Actor *e = new Actor(action, put("name", "e"));
		Actor *f = new Actor(
			action,
			[&](Actor &a) {
				a.props().put<string>("name", "f");
				a.appendAction(action);
			}
		);
		Actor *g = new Actor(action, put("name", "g"));

		a->appendChild(
			b->appendChild(c)->appendChild(d)
		)->appendChild(
			e->appendChild(f->appendChild(g))
		);

		cout << "--- act" << endl;
		a->act();
		cout << "--- dtor" << endl;
		delete a;
	}
}

extern "C" int tutorial3() {
	Tutorial3::test1();
	Tutorial3::test2();
	Tutorial3::test3();
	return 0;
}
