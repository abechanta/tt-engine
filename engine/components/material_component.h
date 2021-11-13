#pragma once
#include <actor.h>
#include <asset.h>
#include <clist.h>
#include <geometry.h>
#include <cassert>
#include <cstdint>
#include <functional>

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
		vector4 m_diffuse;
		vector2 m_uv0;
		vector2 m_uv1;
		vector2i m_size;

		//
		// public methods
		//
	public:
		Material(
			const Asset &asset, const vector4 &diffuse, const vector2 &uv0, const vector2 &uv1
		) : CList(tag), m_asset(asset), m_diffuse(diffuse), m_uv0(uv0), m_uv1(uv1), m_size() {
			X(m_size) = asset.props().get<int32_t>("size.w", 8);
			Y(m_size) = asset.props().get<int32_t>("size.h", 8);
		}

		Material(
			const Asset& asset, const Actor &a
		) : CList(tag), m_asset(asset), m_diffuse(), m_uv0(), m_uv1(), m_size() {
			const string &key = "material";
			X(m_size) = asset.props().get<int32_t>("size.w", 8);
			Y(m_size) = asset.props().get<int32_t>("size.h", 8);
			m_diffuse = Geometry::get<vec, float, 4>(a.props(key + ".diffuse"), 1.f);
			auto pNode = a.props(key + ".uvUnit");
			if (pNode && (pNode->get<string>("") == "px")) {
				m_uv0 = to_vector2(Geometry::get<vec, int32_t, 2>(a.props(key + ".uv.0"), 0));
				m_uv1 = to_vector2(Geometry::get<vec, int32_t, 2>(a.props(key + ".uv.1"), 8));
			} else {
				m_uv0 = Geometry::get<vec, float, 2>(a.props(key + ".uv.0"), 0.f);
				m_uv1 = Geometry::get<vec, float, 2>(a.props(key + ".uv.1"), 1.f);
			}
		}

		virtual ~Material() override {
		}

		static Actor::Action append(const Asset &asset) {
			return [&asset](Actor &a) {
				auto pMaterial = new Material(asset, a);
				a.appendComponent(pMaterial);
			};
		}

		//
		// property methods
		//
	public:
		operator const Asset & () const {
			return m_asset;
		}

		vector4 & diffuse() {
			return m_diffuse;
		}

		const vector4 & diffuse() const {
			return m_diffuse;
		}

		vector2 & uv0() {
			return m_uv0;
		}

		const vector2 & uv0() const {
			return m_uv0;
		}

		vector2 & uv1() {
			return m_uv1;
		}

		const vector2 & uv1() const {
			return m_uv1;
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
