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

namespace Tutorial4 {
	void test1() {
		cout << __FUNCTION__ << endl;
		auto asset1 = make_unique<Asset>(L"asset/tutorial4/tutorial4.json", AssetHandler::initializerJson);
		cout << "--- load" << endl;
		auto resource1 = make_unique<Resource>(*asset1);
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		cout << "--- ctor" << endl;
		AssetHandler::clear();
		AssetHandler::appendInitializer({ L"<undef>", AssetHandler::initializerUnknown, });
		AssetHandler::appendInitializer({ L".json", AssetHandler::initializerJson, });
		AssetHandler::appendInitializer({ L"", AssetHandler::initializerDir, });
		auto assetRoot = make_unique<Asset>(L"asset/tutorial4/font", AssetHandler::factory(L""));
		cout << "--- load" << endl;
		unique_ptr<Actor> actor1 = make_unique<Actor>(
			[](Actor &a) {
				a.findComponent<Resource>([&a](auto &resource) {
					auto &js = resource.find(L"font1.json");
					cout << "contentType=" << js.props().get<string>("contentType", "<undef>") << endl;
				});
			},
			Resource::append(*assetRoot)
		);
		cout << "--- tick" << endl;
		actor1->act();
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial4() {
	Tutorial4::test1();
	Tutorial4::test2();
	return 0;
}
