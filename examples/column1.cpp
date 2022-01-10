#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
using namespace std;

namespace Column1 {
	void test1() {
		cout << __FUNCTION__ << endl;
		std::unique_ptr<int32_t> p1 = make_unique<int32_t>(123);
		std::unique_ptr<int32_t> p2;
		cout << "--- access1" << endl;
		if (p1) {
			cout << "\t*p1=" << *p1 << endl;
		}
		if (p2) {
			cout << "\t*p2=" << *p2 << endl;
		}

		p1.reset();
		p2 = make_unique<int32_t>(456);
		cout << "--- access2" << endl;
		if (p1) {
			cout << "\t*p1=" << *p1 << endl;
		}
		if (p2) {
			cout << "\t*p2=" << *p2 << endl;
		}
		cout << "--- dtor" << endl;
	}
}

extern "C" int column1() {
	Column1::test1();
	return 0;
}
