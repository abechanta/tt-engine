#pragma once
#include <actor.h>
#include <asset.h>
#include <cassert>
#include <clist.h>
#include <cstdint>
#include <string>

namespace tte {
	class Resource : public CList {
		//
		// public definitions
		//
	public:
		static constexpr uint32_t tag = makeTag("RSRC");

		//
		// member variables
		//
	private:
		Asset &m_asset;

		//
		// public methods
		//
	public:
		explicit Resource(Asset &asset)
			: CList(tag), m_asset(asset)
		{
			m_asset.load();
		}

		virtual ~Resource() override {
			m_asset.unload();
		}

		static Actor::Action append(Asset &asset, bool bParse = false) {
			return [&asset, bParse](Actor &a) {
				auto assetName = bParse ? a.props("resource").get<string>("asset", "") : "";
				a.appendComponent(new Resource(asset.find(assetName)));
			};
		}

		Asset &find(const Asset::Path &path) {
			return m_asset.find(path);
		}

		static Asset &find(Actor &a, const Asset::Path &path) {
			return a.getComponent<Asset &, Resource>(Asset::noAsset(), [&path](auto &resource) -> Asset & {
				return resource.find(path);
			});
		}
	};
}
