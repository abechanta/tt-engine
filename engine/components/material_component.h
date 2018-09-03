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

		//
		// public methods
		//
	public:
		Material(
			const Asset &asset, const vector4 &diffuse, const vector2 &uv0, const vector2 &uv1
		) : CList(tag), m_asset(asset), m_diffuse(diffuse), m_uv0(uv0), m_uv1(uv1) {
		}

		virtual ~Material() override {
		}

		static Actor::Action append(const Asset &asset, const string &key = "material") {
			return [&asset, key](Actor &a) {
				if (a.get<float>("size.x", FP_NAN) == FP_NAN) {
					// filler
					float w = asset.props().get<float>("size.w", 8.f);
					a.put<float>("size.x", w);
					float h = asset.props().get<float>("size.h", 8.f);
					a.put<float>("size.y", h);
				}
				vector4 d = Geometry::get<vector4>(a.props(), key + ".diffuse", 1.f);
				vector2 uv0 = Geometry::get<vector2>(a.props(), key + ".uv.0", 0.f);
				vector2 uv1 = Geometry::get<vector2>(a.props(), key + ".uv.1", 1.f);
				a.transform<bool, string>(key + ".uv.unit", false, [&a, &uv0, &uv1](const string &unit) -> bool {
					if (unit == "px") {
						vector2 size = Geometry::get<vector2>(a.props(), "size", 1.f);
						X(uv0) /= X(size);
						Y(uv0) /= Y(size);
						X(uv1) /= X(size);
						Y(uv1) /= Y(size);
					}
					return true;
				});
				a.appendComponent(new Material(asset, d, uv0, uv1));
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
	};
}
