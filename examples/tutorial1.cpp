#include <mtree.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
using namespace tte;

namespace Tutorial1 {
	struct Item : MTree<Item> {
		string m_value;
		int32_t m_weight;

		explicit Item(string value, int32_t weight)
			: m_value(value), m_weight(weight)
		{
			cout << "\t" << "ctor: item=" << m_value << "/" << m_weight << "kg" << endl;
		}

		virtual ~Item() override {
			cout << "\t" << "dtor" << endl;
		}
	};

	void test1() {
		cout << __FUNCTION__ << endl;
		cout << "--- ctor" << endl;
		auto a = new Item("Large bag", 1);
		auto b = new Item("Dagger", 1);
		auto c = new Item("Small axe", 2);
		a->appendChild(b);
		a->appendChild(c);
		cout << "--- dtor" << endl;
		delete a;
	}
}

extern "C" int tutorial1() {
	Tutorial1::test1();
	return 0;
}
