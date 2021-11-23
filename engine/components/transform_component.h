#pragma once
#include <actor.h>
#include <cassert>
#include <clist.h>
#include <cstdint>
#include <functional>
#include <geometry.h>
#include <initializer_list>
#include <ptree.h>
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
	public:
		PTree::PropertyV<vec, float, 3> translation;
		PTree::PropertyV<vec, float, 3> rotation;
		PTree::PropertyV<vec, float, 3> scaling;

		//
		// public methods
		//
	public:
		Transform(property_tree::ptree &node)
			: CList(tag),
			translation(node.get_child("transform"), "translation", 0.f),
			rotation(node.get_child("transform"), "rotation", 0.f),
			scaling(node.get_child("transform"), "scaling", 1.f)
		{
		}

		virtual ~Transform() override {
		}

		static Actor::Action append() {
			// static const unordered_map<string, float> unitConv = {
			// 	{ "radian", 1.f, },
			// 	{ "degree", Geometry::deg2rad, },
			// 	{ "fixed32", Geometry::fixed2rad, },
			// };
			return [](Actor &a) {
				// const string rUnit = a.get<string>(key + ".rotation.unit", "degree");
				// r *= unitConv.at(rUnit);
				a.appendComponent(new Transform(a.props()));
			};
		}

		matrix3x4 & trs2d(matrix3x4 &m) {
			return Geometry::trs2d(m, translation(), rotation(), scaling());
		}

		//
		// property methods
		//
	};
}
