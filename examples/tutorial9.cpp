#include <actor.h>
#include <components/input_component.h>
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial9 {
	void test1() {
		cout << __FUNCTION__ << endl;
		auto input1 = make_unique<Input>();
		cout << "--- frames" << endl;
		auto key1 = initializer_list<bool>{
			0,0,0,0,1,1,0,0, 0,0,0,0,0,0,0,0, 0,1,1,1,1,1,1,1, 1,1,1,1,0,0,0,0,
			0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1, 0,0,0,0,0,0,1,1, 1,1,1,1,1,1,1,1,
		};
		cout << "\tpast<<<";
		for (auto &it : key1) {
			cout << it;
			input1->update();
			input1->buttons("key1").update(it);
		}
		cout << "<<<now" << endl;
		cout << "--- check" << endl;
		for (auto &n : { 1, 4, 8, 15, }) {
			cout << "\ton(" << n << ")=" << input1->buttons("key1").on(n);
		}
		cout << endl;
		for (auto &n : { 10, 20, 40, 60, }) {
			cout << "\tpressed(" << n << ")=" << input1->buttons("key1").pressed(n);
		}
		cout << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		auto input1 = make_unique<Input>();
		cout << "--- frames" << endl;
		auto device = [](const initializer_list<bool> &data) -> function<bool()> {
			auto d = data.begin();
			return[d, end = data.end()]() mutable -> bool {
				return (d == end) ? false : *d++;
			};
		};
		auto kbd1 = device({ 0,0,0,0,0,0,0,0, 0,1,1,1,1,1,1,1, 1,1,1,1,0,0,0,0, });
		auto pad1 = device({ 0,0,0,0,0,0,0,1, 0,0,0,0,0,0,1,1, 1,1,1,1,1,1,1,1, });
		for (auto i = 0; i < 24; i++) {
			input1->update();
			input1->buttons("left").update(kbd1());
			input1->buttons("left").update(pad1() | input1->buttons("left").on());
		}
		cout << "--- check" << endl;
		for (auto &n : { 1, 4, 8, 15, }) {
			cout << "\ton(" << n << ")=" << input1->buttons("left").on(n);
		}
		cout << endl;
	}
}

extern "C" int tutorial9() {
	Tutorial9::test1();
	Tutorial9::test2();
	return 0;
}
