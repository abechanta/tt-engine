#include <asset.h>
#include <asset_handler.h>
#include <boost/property_tree/json_parser.hpp>
#include <cassert>
#include <cstdint>
#include <iostream>
using namespace tte;

namespace Tutorial2 {
	void test1() {
		cout << __FUNCTION__ << endl;
		cout << "--- ctor" << endl;
		Asset a(L"asset/tutorial2/status.json", AssetHandler::typeJson);
		cout << "--- load" << endl;
		a.load();
		cout << "--- props" << endl;
		write_json(cout, a.props());
		cout << "--- unload" << endl;
		a.unload();
		cout << "--- props" << endl;
		write_json(cout, a.props());
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial2() {
	Tutorial2::test1();
	return 0;
}
