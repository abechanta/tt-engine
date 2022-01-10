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
		read_json(ifstream("asset/tutorial2/status.json"), pt);
		cout << "--- access1" << endl;
		cout << "\tlevel=" << pt.get<int32_t>("level") << endl;
		cout << "\tgold=" << pt.get<int32_t>("gold") << endl;
		cout << "--- access2" << endl;
		for (const ptree::value_type &arr : pt.get_child("spells")) {
			cout << "\tspells[].name=" << arr.second.get<string>("name") << endl;
		}
		cout << "--- access3" << endl;
		write_json(cout, pt);
		cout << "--- dtor" << endl;
	}
}

extern "C" int column2() {
	Column2::test1();
	return 0;
}
