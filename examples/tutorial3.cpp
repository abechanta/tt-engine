#include <asset.h>
#include <asset_handler.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial3 {
	void test1() {
		cout << __FUNCTION__ << endl;
		AssetHandler assetHandler;
		assetHandler.clear();
		assetHandler.append({ L"<undef>", AssetHandler::typeUnknown, });
		assetHandler.append({ L".json", AssetHandler::typeJson, });
		cout << "--- ctor" << endl;
		Asset a(L"asset/tutorial3/dialog.json", assetHandler);
		Asset b(L"asset/tutorial3/dialog/message_body.rsrc", assetHandler);
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		AssetHandler assetHandler;
		assetHandler.clear();
		assetHandler.append({ L"<undef>", AssetHandler::typeUnknown, });
		assetHandler.append({ L".json", AssetHandler::typeJson, });
		assetHandler.append({ L"", AssetHandler::typeDir, });
		cout << "--- ctor" << endl;
		Asset assets(L"asset/tutorial3", assetHandler);
		auto &a = assets.find(L"dialog/button_yes.json");
		auto &b = assets.find(L"dialog");
		cout << "--- load1" << endl;
		a.load();
		cout << "--- load2" << endl;
		b.load();
		cout << "--- unload1" << endl;
		a.unload();
		cout << "--- unload2" << endl;
		b.unload();
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial3() {
	Tutorial3::test1();
	Tutorial3::test2();
	return 0;
}
