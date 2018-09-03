#pragma once
#include <actor.h>
#include <clist.h>
#include <geometry.h>
#include <cassert>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <string>
#include <unordered_map>

namespace tte {
	using namespace std;

	class Transform : public CList {
		//
		// public definitions
		//
	public:
		static constexpr uint32_t tag = makeTag("TRNS");

		//
		// member variables
		//
	private:
		vector3 m_translation;
		vector3 m_rotation;
		vector3 m_scaling;

		//
		// public methods
		//
	public:
		Transform(
			const vector3 &translation, const vector3 &rotation, const vector3 &scaling
		) : CList(tag), m_translation(translation), m_rotation(rotation), m_scaling(scaling) {
		}

		virtual ~Transform() override {
		}

		static Actor::Action append(const string &key = "transform") {
			static const unordered_map<string, float> unitConv = {
				{ "radian", 1.f, },
				{ "degree", Geometry::deg2rad, },
				{ "fixed32", Geometry::fixed2rad, },
			};
			return [key](Actor &a) {
				vector3 t = Geometry::get<vector3>(a.props(), key + ".translation");
				vector3 r = Geometry::get<vector3>(a.props(), key + ".rotation");
				string rUnit = a.get<string>(key + ".rotation.unit", "degree");
				r *= unitConv.at(rUnit);
				vector3 s = Geometry::get<vector3>(a.props(), key + ".scaling", 1.f);
				a.appendComponent(new Transform(t, r, s));
			};
		}

		matrix3x4 & trs2d(matrix3x4 &m) const {
			return Geometry::trs2d(m, m_translation, m_rotation, m_scaling);
		}

		//
		// property methods
		//
	public:
		vector3 & translation() {
			return m_translation;
		}

		const vector3 & translation() const {
			return m_translation;
		}

		vector3 & rotation() {
			return m_rotation;
		}

		const vector3 & rotation() const {
			return m_rotation;
		}

		vector3 & scaling() {
			return m_scaling;
		}

		const vector3 & scaling() const {
			return m_scaling;
		}
	};
}
