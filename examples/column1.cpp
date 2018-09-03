#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
using namespace std;

namespace Column1 {
	void test1() {
		cout << __FUNCTION__ << endl;
		std::unique_ptr<uint32_t> ptr1 = make_unique<uint32_t>(123);
		std::unique_ptr<uint32_t> ptr2;
		cout << "--- access1" << endl;
		if (ptr1) {
			cout << "\t*ptr1=" << *ptr1 << endl;
		}
		if (ptr2) {
			cout << "\t*ptr2=" << *ptr2 << endl;
		}
		cout << "--- set" << endl;
		ptr1.reset();
		ptr2 = make_unique<uint32_t>(456);
		cout << "--- access2" << endl;
		if (ptr1) {
			cout << "\t*ptr1=" << *ptr1 << endl;
		}
		if (ptr2) {
			cout << "\t*ptr2=" << *ptr2 << endl;
		}
		cout << "--- dtor" << endl;
	}
}

extern "C" int column1() {
	Column1::test1();
	return 0;
}
