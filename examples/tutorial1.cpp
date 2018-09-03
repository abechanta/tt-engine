#include <asset.h>
#include <asset_handler.h>
#include <cassert>
#include <cstdint>
#include <iostream>
using namespace tte;

namespace Tutorial1 {
	void test1() {
		cout << __FUNCTION__ << endl;
		Asset asset1(L"asset/tutorial1.json", AssetHandler::initializerJson);
		cout << "--- load" << endl;
		asset1.load();
		cout << "--- unload" << endl;
		asset1.unload();
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial1() {
	Tutorial1::test1();
	return 0;
}
