#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
using namespace boost;
using namespace std;

namespace Column2 {
	void test1() {
		cout << __FUNCTION__ << endl;
		property_tree::ptree pt;
		property_tree::json_parser::read_json(ifstream("asset/tutorial1.json"), pt);
		cout << "--- access1" << endl;
		cout << "\ttag1=" << pt.get<int32_t>("tag1") << endl;
		cout << "--- access2" << endl;
		for (const property_tree::ptree::value_type &arr : pt.get_child("array")) {
			cout << "\tarray.value=" << arr.second.get<int32_t>("value") << endl;
		}
		cout << "--- access3" << endl;
		property_tree::json_parser::write_json(cout, pt);
		cout << "--- dtor" << endl;
		cout << endl;
	}
}

extern "C" int column2() {
	Column2::test1();
	return 0;
}
