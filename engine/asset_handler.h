#pragma once
#include <asset.h>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <boost/property_tree/json_parser.hpp>
#include <unordered_map>
#include <utility>

namespace tte {
	using namespace boost;
	using namespace std;
	namespace fs = experimental::filesystem::v1;

	class AssetHandler {
		//
		// public definitions
		//
	public:
		static constexpr wchar_t extensionUnknown[] = L"<undef>";

		//
		// member variables
		//
	private:
		static unordered_map<Asset::Path, function<void(Asset &)> > s_initializerRegistry;

		//
		// public methods
		//
	public:
		static void clear() {
			s_initializerRegistry.clear();
		}

		static void appendInitializer(const pair<Asset::Path, function<void(Asset &)> > &entry) {
			s_initializerRegistry[entry.first] = entry.second;	
		}

		static const function<void(Asset &)> & factory(const Asset::Path &filename) {
			Asset::Path &extension = filename.extension();
			auto &it = s_initializerRegistry[extension];
			return it ? it : s_initializerRegistry[extensionUnknown];
		}

		//
		// default loader
		//
	public:
		static void initializerDir(Asset &a) {
			cout << __FUNCTION__ << ": " << a.path() << endl;
			a.setLoader([](Asset &a, bool bLoad) -> bool {
				for_each(a.begin_children(), a.end_children(), [bLoad](Asset &a) {
					bLoad ? a.load() : a.unload();
				});
				return true;
			});

			for (auto &f : fs::directory_iterator(a.path())) {
				Asset *c = new Asset(Asset::Path(f), factory(Asset::Path(f)));
				a.appendChild(c);
			}
		}

		static void initializerUnknown(Asset &a) {
			cout << __FUNCTION__ << ": " << a.path() << endl;
		}

		static void initializerJson(Asset &a) {
			cout << __FUNCTION__ << ": " << a.path() << endl;
			assert(fs::is_regular_file(a.path()));
			a.setLoader([](Asset &a, bool bLoad) -> bool {
				if (bLoad) {
					read_json(ifstream(a.path()), a.props());
					write_json(cout, a.props());
				} else {
					a.props().clear();
				}
				return true;
			});
		}

		//
		// others
		//
	private:
		AssetHandler() {}
		~AssetHandler() {}
	};

#if defined(tte_declare_static_variables)
	unordered_map<Asset::Path, function<void(Asset &)> > AssetHandler::s_initializerRegistry = {};
#endif	// defined(tte_declare_static_variables)
}
