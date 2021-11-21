#pragma once
#include <geometry.h>
#include <ptree.h>
#include <renderer2d.h>
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
	namespace Shape2d {
		using namespace boost;
		using namespace std;

		struct Point {
			// TODO
		};

		struct Line {
			// TODO
		};

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

			void draw(const Renderer2dInterface &renderer, Actor &a) {
				renderer.drawRect(a, size, anchor, flip);
			}
		};

		struct Tilemap {
			typedef Tilemap this_type;
			vector2i size;
			vector2i viewOffset;
			vector2i cellSize;
			vector2i blitSize;
			vector2i mapSize;
			bool transpose;

			static this_type load(const property_tree::ptree &pt) {
				this_type v;
				v.size = Geometry::get<vec, int32_t, 2>(pt.get_child("size"), 8.f);
				v.viewOffset = Geometry::get<vec, int32_t, 2>(pt.get_child("viewOffset"), 0.f);
				v.cellSize = Geometry::get<vec, int32_t, 2>(pt.get_child("cellSize"), 8.f);
				v.blitSize = Geometry::get<vec, int32_t, 2>(pt.get_child("blitSize"), 8.f);
				v.mapSize = Geometry::get<vec, int32_t, 2>(pt.get_child("mapSize"), 1);
				v.transpose = pt.get<bool>("transpose", false);
				return v;
			}

			void draw(const Renderer2dInterface &renderer, Actor &a) {
				a.getComponent<Transform, Material>([this, &a, &renderer](auto &transform, auto &material) {
					const int32_t cellBounds = X(material.size()) / X(cellSize);
					const vector3 translation = transform.translation();
					const vector2 uv0 = material.uv0();
					const vector2 uv1 = material.uv1();
					{
						const vector2i mapSize_ = transpose ? vector2i{ Y(mapSize), X(mapSize), } : mapSize;

						if (transpose) {
							for (int32_t rp = -(X(viewOffset) % X(blitSize)); rp < X(size); rp += X(blitSize)) {
								int32_t ri = (X(viewOffset) + rp) / X(blitSize);
								if (ri >= X(mapSize_)) {
									break;
								}
								transform.translation() = translation + X00<int32_t>(rp);
								const vector<int32_t> vertical = Geometry::get<vector, int32_t>(a.props("tilemap.tiles." + to_string(ri)));
								for (int32_t cp = -(Y(viewOffset) % Y(blitSize)); cp < Y(size); cp += Y(blitSize)) {
									int32_t ci = (Y(viewOffset) + cp) / Y(blitSize);
									if (ci >= Y(mapSize_)) {
										break;
									}
									auto code = vertical[ci];
									material.uv0() = material.to_vector2({ ((code % cellBounds) + 0) * X(cellSize), ((code / cellBounds) + 0) * Y(cellSize), });
									material.uv1() = material.to_vector2({ ((code % cellBounds) + 1) * X(cellSize), ((code / cellBounds) + 1) * Y(cellSize), });
									renderer.drawRect(a, blitSize, vector2{ 0.f, 0.f, }, vec<bool, 2>{ false, false, });
									Y(transform.translation()) += Y(blitSize);
								}
							}
						} else {
							for (int32_t rp = -(Y(viewOffset) % Y(blitSize)); rp < Y(size); rp += Y(blitSize)) {
								int32_t ri = (Y(viewOffset) + rp) / Y(blitSize);
								if (ri >= Y(mapSize_)) {
									break;
								}
								transform.translation() = translation + _0X0<int32_t>(rp);
								const vector<int32_t> holizontal = Geometry::get<vector, int32_t>(a.props("tilemap.tiles." + to_string(ri)));
								for (int32_t cp = -(X(viewOffset) % X(blitSize)); cp < X(size); cp += X(blitSize)) {
									int32_t ci = (X(viewOffset) + cp) / X(blitSize);
									if (ci >= X(mapSize_)) {
										break;
									}
									auto code = holizontal[ci];
									material.uv0() = material.to_vector2({ ((code % cellBounds) + 0) * X(cellSize), ((code / cellBounds) + 0) * Y(cellSize), });
									material.uv1() = material.to_vector2({ ((code % cellBounds) + 1) * X(cellSize), ((code / cellBounds) + 1) * Y(cellSize), });
									renderer.drawRect(a, blitSize, vector2{ 0.f, 0.f, }, vec<bool, 2>{ false, false, });
									X(transform.translation()) += X(blitSize);
								}
							}
						}
					}
					transform.translation() = translation;
					material.uv0() = uv0;
					material.uv1() = uv1;
				});
			}
		};

		struct TiledWindow {
			// TODO
		};
	}
}
