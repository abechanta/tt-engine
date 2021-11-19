#pragma once
#include <geometry.h>
#include <ptree.h>
#include <components/renderer2d_component.h>
#include <cassert>
#include <climits>
#include <cstdint>
#include <deque>
#include <iostream>
#include <iterator>
#include <boost/multi_array.hpp>
#include <optional>
#include <string>
#include <unordered_map>

namespace tte {
	using namespace boost;
	using namespace std;

	struct Sprite {
		typedef Sprite this_type;
		vector2 size;
		vector2 anchor;
		vec<bool, 2> flip;

		static this_type load(const property_tree::ptree &pt) {
			this_type v;
			//PTree::setter<this_type, vector2>("size", { 8.f, 8.f, }, [](this_type &v) -> auto & { return v.size; })(v, pt);
			//PTree::setter<this_type, vector2>("anchor", { 0.5f, 0.5f, }, [](this_type &v) -> auto & { return v.anchor; })(v, pt);
			//PTree::setter<this_type, vec<bool, 2>>("flip", { false, false, }, [](this_type &v) -> auto & { return v.flip; })(v, pt);
			v.size = Geometry::get<vec, float, 2>(pt.get_child("size"), 8.f);
			v.anchor = Geometry::get<vec, float, 2>(pt.get_child("anchor"), 0.5f);
			v.flip = Geometry::get<vec, bool, 2>(pt.get_child("flip"), false);
			return v;
		}

		void draw(Renderer2d &renderer, Actor &a) {
			renderer.drawRect(a);
		}
	};
}
