#include <mtree.h>
#include <cassert>
#include <cstdint>
#include <iostream>
using namespace tte;

namespace Tutorial0 {
	struct Value : MTree<Value> {
		uint32_t m_value;
		Value(uint32_t value) : m_value(value) {
			cout << "\tctor:" << m_value << endl;
		}
		virtual ~Value() {
			cout << "\tdtor" << endl;
		}
	};

	void test1() {
		cout << __FUNCTION__ << endl;
		auto a = new Value(123);
		auto b = new Value(456);
		auto c = new Value(789);
		a->appendChild(b);
		a->appendChild(c);
		cout << "--- dtor" << endl;
		delete a;
	}
}

extern "C" int tutorial0() {
	Tutorial0::test1();
	return 0;
}
