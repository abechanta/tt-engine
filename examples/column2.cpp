#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
using namespace boost;
using namespace std;
using ptree = property_tree::ptree;

namespace Column2 {
	void test1() {
		cout << __FUNCTION__ << endl;
		ptree pt;
		read_json(ifstream("asset/tutorial1.json"), pt);
		cout << "--- access1" << endl;
		cout << "\ttag1=" << pt.get<int32_t>("tag1") << endl;
		cout << "--- access2" << endl;
		for (const ptree::value_type &arr : pt.get_child("array")) {
			cout << "\tarray.value=" << arr.second.get<int32_t>("value") << endl;
		}
		cout << "--- access3" << endl;
		write_json(cout, pt);
		cout << "--- dtor" << endl;
		cout << endl;
	}
}

extern "C" int column2() {
	Column2::test1();
	return 0;
}
