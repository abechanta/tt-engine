#include <actor.h>
#include <components/input_component.h>
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial12 {
	auto device = [](const initializer_list<bool> &data) -> function<bool()> {
		auto d = data.begin();
		return [d, end = data.end()]() mutable -> bool {
			return (d == end) ? false : *d++;
		};
	};

	void test1() {
		cout << __FUNCTION__ << endl;
		auto kbdX = device({ 0,0,1,1,0,0,0,1, 1,1,1,0,0,0,0,1, 1,1,1,1,1,1,1,1, });
		auto input1 = make_unique<Input>();
		cout << "--- frames" << endl;
		cout << "\t" << "past<<<";
		for (auto i = 0; i < 24; i++) {
			input1->update();
			auto d = kbdX();
			cout << d;
			input1->buttons("kbdX").update(d);
		}
		cout << "<<<now" << endl;
		cout << "--- check" << endl;
		for (auto &n : { 1, 4, 8, 15, }) {
			cout << "\t" << "on(" << n << ")=" << input1->buttons("kbdX").on(n) << "\t";
		}
		cout << endl;
		for (auto &n : { 10, 15, 20, 25, }) {
			cout << "\t" << "pressed(" << n << ")=" << input1->buttons("kbdX").pressed(n);
		}
		cout << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		auto kbdZ = device({ 0,0,0,0,0,0,0,0, 0,1,1,1,1,1,0,0, 1,1,1,1,0,0,0,0, });
		auto padA = device({ 0,0,0,0,0,0,0,1, 0,0,0,0,0,0,1,1, 1,1,1,1,1,1,1,1, });
		auto input1 = make_unique<Input>();
		cout << "--- frames" << endl;
		for (auto i = 0; i < 24; i++) {
			input1->update();
			input1->buttons("jump").update(kbdZ());
			input1->buttons("jump").update(padA() | input1->buttons("jump").on());
		}
		cout << "--- check" << endl;
		for (auto &n : { 1, 4, 8, 15, }) {
			cout << "\t" << "on(" << n << ")=" << input1->buttons("jump").on(n) << "\t";
		}
		cout << endl;
	}
}

extern "C" int tutorial12() {
	Tutorial12::test1();
	Tutorial12::test2();
	return 0;
}
