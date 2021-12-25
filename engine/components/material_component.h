#pragma once
#include <actor.h>
#include <asset.h>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <climits>
#include <clist.h>
#include <cstdint>
#include <functional>
#include <geometry.h>
#include <ptree.h>
#include <string>

namespace tte {
	using namespace boost;
	using namespace std;
	using ptree = property_tree::ptree;

	class Material : public CList {
		//
		// public definitions
		//
	public:
		static constexpr uint32_t tag = makeTag("MATE");

		//
		// member variables
		//
	private:
		const Asset &m_asset;
		vector2i m_size;
		struct Extra {
			vector2 uv0;
			vector2 uv1;

			void reset() {
				uv0 = { 0.f, 0.f, };
				uv1 = { 1.f, 1.f, };
			}
		} m_extra;
	public:
		PTree::PropertyV<vector4> diffuse;
		PTree::PropertyV<vector2> uv0;
		PTree::PropertyV<vector2> uv1;

		//
		// public methods
		//
	public:
		explicit Material(const Asset &asset, ptree &node)
			: CList(tag),
			m_asset(asset),
			m_size(PTree::PropertyV<vector2i>::get(asset.props(), "size", 8, PTree::subkeysWH)),
			m_extra(),
			diffuse(node, "diffuse", 1.f, PTree::subkeysRGBA),
			uv0(node, "uv.0", 0.f),
			uv1(node, "uv.1", 1.f)
		{
			auto unit = node.get<string>("uvUnit", "ratio");
			if (unit == "px") {
				uv0.set(to_ratio(scalar_cast<int32_t>(uv0.get())));
				uv1.set(to_ratio(scalar_cast<int32_t>(uv1.get())));
			}
		}

		explicit Material(const Asset &asset, ptree &node, const vector4 &diffuse_, const vector2 &uv0_, const vector2 &uv1_)
			: CList(tag),
			m_asset(asset),
			m_size(PTree::PropertyV<vector2i>::get(asset.props(), "size", 8, PTree::subkeysWH)),
			m_extra(),
			diffuse(node, "diffuse", diffuse_, PTree::subkeysRGBA),
			uv0(node, "uv.0", uv0_),
			uv1(node, "uv.1", uv1_)
		{
		}

		virtual ~Material() override {
		}

		static Actor::Action append(Asset &asset, bool bParse = false) {
			return [&asset, bParse](Actor &a) {
				auto assetName = bParse ? a.props("material").get<string>("texture", "") : "";
				a.appendComponent(new Material(asset.find(assetName), a.props("material")));
			};
		}

		//
		// property methods
		//
	public:
		operator const Asset &() const {
			return m_asset;
		}

		const vector2i &size() const {
			return m_size;
		}

		void resetUv() {
			m_extra.reset();
		}

		const vector2 &_uv0() const {
			return m_extra.uv0;
		}

		void _uv0(const vector2 &uv) {
			m_extra.uv0 = uv;
		}

		const vector2 &_uv1() const {
			return m_extra.uv1;
		}

		void _uv1(const vector2 &uv) {
			m_extra.uv1 = uv;
		}

		vector2i to_pixel(const vector2 &uv) const {
			vector2 size = scalar_cast<float>(m_size);
			return scalar_cast<int32_t>(Geometry::mul_elements<vector2>(uv, size));
		}

		vector2 to_ratio(const vector2i &uvi) const {
			vector2 size = scalar_cast<float>(m_size);
			vector2 uv = scalar_cast<float>(uvi);
			return Geometry::div_elements<vector2>(uv, size);
		}
	};
}
