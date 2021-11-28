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
		explicit Transform(property_tree::ptree &node)
			: CList(tag),
			translation(node, "translation", 0.f),
			rotation(node, "rotation", 0.f),
			scaling(node, "scaling", 1.f)
		{
			auto unit = node.get<string>("rotationUnit", "radian");
			if (auto unitConv = Geometry::rotationConv.at(unit)) {
				rotation(rotation() * unitConv);
			}
		}

		explicit Transform(property_tree::ptree &node, const vector3 &translation_, const vector3 &rotation_, const vector3 &scaling_)
			: CList(tag),
			translation(node, "translation", translation_),
			rotation(node, "rotation", rotation_),
			scaling(node, "scaling", scaling_)
		{
		}

		virtual ~Transform() override {
		}

		static Actor::Action append() {
			return [](Actor &a) {
				a.appendComponent(new Transform(a.props("transform")));
			};
		}

		matrix3x4 &trs2d(matrix3x4 &m) {
			return Geometry::trs2d(m, translation(), rotation(), scaling());
		}

		//
		// property methods
		//
	};
}
