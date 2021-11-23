#pragma once
#include <actor.h>
#include <asset.h>
#include <cassert>
#include <climits>
#include <clist.h>
#include <cstdint>
#include <functional>
#include <geometry.h>
#include <ptree.h>

namespace tte {
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
	public:
		PTree::PropertyV<vec, float, 4> diffuse;
		PTree::PropertyV<vec, float, 2> uv0;
		PTree::PropertyV<vec, float, 2> uv1;

		//
		// public methods
		//
	public:
		Material(const Asset &asset, property_tree::ptree &node)
			: CList(tag),
			m_asset(asset),
			m_size(PTree::PropertyV<vec, int32_t, 2>::get(asset.props(), "size", 8, PTree::subkeysWH)),
			diffuse(node, "diffuse", 1.f, PTree::subkeysRGBA),
			uv0(node, "uv.0", 0.f),
			uv1(node, "uv.1", 1.f)
		{
			auto unit = node.get<string>("uvUnit", "ratio");
			if (unit == "px") {
				uv0(to_vector2(uv0()));
				uv1(to_vector2(uv1()));
			}
		}

		Material(const Asset &asset, property_tree::ptree &node, const vector4 &diffuse_, const vector2 &uv0_, const vector2 &uv1_)
			: CList(tag),
			m_asset(asset),
			m_size(PTree::PropertyV<vec, int32_t, 2>::get(asset.props(), "size", 8, PTree::subkeysWH)),
			diffuse(node, "diffuse", diffuse_, PTree::subkeysRGBA),
			uv0(node, "uv.0", uv0_),
			uv1(node, "uv.1", uv1_)
		{
		}

		virtual ~Material() override {
		}

		static Actor::Action append(const Asset &asset) {
			return [&asset](Actor &a) {
				a.appendComponent(new Material(asset, a.props("material")));
			};
		}

		//
		// property methods
		//
	public:
		operator const Asset &() const {
			return m_asset;
		}

		const vector2i & size() const {
			return m_size;
		}

		vector2i to_vector2i(const vector2 &uv) const {
			return vector2i{ static_cast<int32_t>(X(uv) * X(m_size)), static_cast<int32_t>(Y(uv) * Y(m_size)), };
		}

		vector2 to_vector2(const vector2i &uvi) const {
			vector2 uv = uvi;
			X(uv) /= X(m_size);
			Y(uv) /= Y(m_size);
			return uv;
		}
	};
}
