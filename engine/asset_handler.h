#pragma once
#include <asset.h>
#include <algorithm>
#include <boost/property_tree/json_parser.hpp>
#include <cassert>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <utility>

namespace tte {
	using namespace boost;
	using namespace std;

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
		static unordered_map<Asset::Path, function<void(Asset &)> > s_handlers;

		//
		// public methods
		//
	public:
		AssetHandler() {
		}

		virtual ~AssetHandler() {
		}

		void clear() const {
			s_handlers.clear();
		}

		void append(const pair<Asset::Path, function<void(Asset &)> > &entry) const {
			s_handlers[entry.first] = entry.second;
		}

		//
		// others
		//
	public:
		operator const function<void(Asset &)> () const {
			return [this](Asset &a) {
				lookup(a.path())(a);
			};
		}

		const function<void(Asset &)> &lookup(const Asset::Path &filename) const {
			Asset::Path &extension = filename.extension();
			auto &it = s_handlers[extension];
			return it ? it : s_handlers[extensionUnknown];
		}

		//
		// default loader
		//
	public:
		static void typeDir(Asset &a) {
			cout << __FUNCTION__ << ": " << a.path() << endl;
			a.setHandler([](Asset &a, bool bLoad) -> bool {
				for_each(a.begin_children(), a.end_children(), [bLoad](Asset &a) {
					bLoad ? a.load() : a.unload();
				});
				return true;
			});

			AssetHandler assetHandler;
			for (auto &f : filesystem::directory_iterator(a.path())) {
				Asset *c = new Asset(Asset::Path(f), assetHandler);
				a.appendChild(c);
			}
		}

		static void typeUnknown(Asset &a) {
			cout << __FUNCTION__ << ": " << a.path() << endl;
		}

		static void typeJson(Asset &a) {
			cout << __FUNCTION__ << ": " << a.path() << endl;
			assert(filesystem::is_regular_file(a.path()));
			a.setHandler([](Asset &a, bool bLoad) -> bool {
				if (bLoad) {
					read_json(ifstream(a.path()), a.props());
#if defined(_DEBUG)
					write_json(cout, a.props());
#endif
				} else {
					a.props().clear();
				}
				return true;
			});
		}
	};

#if defined(tte_declare_static_variables)
	unordered_map<Asset::Path, function<void(Asset &)> > AssetHandler::s_handlers = {};
#endif	// defined(tte_declare_static_variables)
}
