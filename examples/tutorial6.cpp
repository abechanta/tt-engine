#include <actor.h>
#include <asset.h>
#include <asset_handler.h>
#include <components/resource_component.h>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial6 {
	void test1() {
		cout << __FUNCTION__ << endl;
		auto a = make_unique<Asset>(L"asset/tutorial6/player_globals.json", AssetHandler::typeJson);
		cout << "--- ctor" << endl;
		auto resource1 = make_unique<Resource>(*a);
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		AssetHandler assetHandler;
		assetHandler.clear();
		assetHandler.append({ L".json", AssetHandler::typeJson, });
		assetHandler.append({ L"", AssetHandler::typeDir, });
		auto assets = make_unique<Asset>(L"asset/tutorial6", assetHandler);
		cout << "--- ctor" << endl;
		unique_ptr<Actor> a = make_unique<Actor>(
			[](Actor &a) {
				auto &pl = Resource::find(a, L"player_globals.json");
				cout << "\t" << "score=" << pl.props().get<int32_t>("score", 0) << endl;
				cout << "\t" << "coins=" << pl.props().get<int32_t>("coins", 0) << endl;
				cout << "\t" << "playersLeft=" << pl.props().get<int32_t>("playersLeft", 0) << endl;
			},
			Resource::append(*assets)
		);
		cout << "--- tick" << endl;
		a->act();
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial6() {
	Tutorial6::test1();
	Tutorial6::test2();
	return 0;
}
