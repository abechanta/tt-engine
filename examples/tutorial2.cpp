#include <asset.h>
#include <asset_handler.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial2 {
	void test1() {
		cout << __FUNCTION__ << endl;
		Asset asset1(L"asset/tutorial2/tutorial2.json", AssetHandler::initializerJson);
		cout << "--- load" << endl;
		asset1.load();
		cout << "--- unload" << endl;
		asset1.unload();
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		AssetHandler::clear();
		AssetHandler::appendInitializer({ L"<undef>", AssetHandler::initializerUnknown, });
		AssetHandler::appendInitializer({ L".json", AssetHandler::initializerJson, });
		AssetHandler::appendInitializer({ L"", AssetHandler::initializerDir, });
		cout << "--- ctor" << endl;
		Asset assetRoot(L"asset/tutorial2", AssetHandler::factory(L""));
		cout << "--- dtor" << endl;
	}

	void test3() {
		cout << __FUNCTION__ << endl;
		AssetHandler::clear();
		AssetHandler::appendInitializer({ L"<undef>", AssetHandler::initializerUnknown, });
		AssetHandler::appendInitializer({ L".json", AssetHandler::initializerJson, });
		AssetHandler::appendInitializer({ L"", AssetHandler::initializerDir, });
		cout << "--- ctor" << endl;
		Asset assetRoot(L"asset/tutorial2", AssetHandler::factory(L""));
		auto &asset1 = assetRoot.find(L"font/font1.json");
		auto &asset2 = assetRoot.find(L"font");
		cout << "--- load1" << endl;
		asset1.load();
		cout << "--- load2" << endl;
		asset2.load();
		cout << "--- unload1" << endl;
		asset1.unload();
		cout << "--- unload2" << endl;
		asset2.unload();
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial2() {
	Tutorial2::test1();
	Tutorial2::test2();
	Tutorial2::test3();
	return 0;
}
