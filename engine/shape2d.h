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
#include <boost/property_tree/ptree.hpp>
#include <string>

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
			PTree::PropertyV<vec, float, 2> size;
			PTree::PropertyV<vec, float, 2> anchor;
			PTree::PropertyV<vec, bool, 2> flip;

			Sprite(property_tree::ptree &node)
				: size(node, "size", 8.f),
				anchor(node, "anchor", .5f),
				flip(node, "flip", false)
			{
			}

			void draw(const Renderer2dInterface &renderer, Actor &a) {
				renderer.drawRect(a, size(), anchor(), flip());
			}
		};

		struct Tilemap {
			PTree::PropertyV<vec, int32_t, 2> size;
			PTree::PropertyV<vec, int32_t, 2> viewOffset;
			PTree::PropertyV<vec, int32_t, 2> cellSize;
			PTree::PropertyV<vec, int32_t, 2> blitSize;
			PTree::PropertyV<vec, int32_t, 2> mapSize;
			PTree::Property<bool> transpose;
			PTree::PropertyAA<vector<int32_t>> tiles;

			Tilemap(property_tree::ptree &node)
				: size(node, "size", 8),
				viewOffset(node, "viewOffset", 0),
				cellSize(node, "cellSize", 8),
				blitSize(node, "blitSize", 8),
				mapSize(node, "mapSize", 8),
				transpose(node, "transpose", false),
				tiles(node, "tiles")
			{
			}

			int32_t startPos(int32_t pos, int32_t size) {
				int32_t val = pos % size;
				val += (pos < 0) ? size : 0;
				return -val;
			}

			int32_t startIdx(int32_t pos, int32_t size, int32_t wrap) {
				int32_t val = pos / size;
				val %= wrap;
				val += (val < 0) ? wrap : 0;
				return val;
			}

			void draw(const Renderer2dInterface &renderer, Actor &a) {
				a.getComponent<Transform, Material>([this, &a, &renderer](auto &transform, auto &material) {
					auto size_ = size();
					auto viewOffset_ = viewOffset();
					auto cellSize_ = cellSize();
					auto blitSize_ = blitSize();
					auto mapSize_ = transpose() ? vector2i{ Y(mapSize()), X(mapSize()), } : mapSize();
					const int32_t cellBounds = X(material.size()) / X(cellSize());

					const vector3 translation = transform.translation();
					const vector2 uv0 = material.uv0();
					const vector2 uv1 = material.uv1();
					{
						if (transpose()) {
							for (int32_t rp = startPos(X(viewOffset_), X(blitSize_)); rp < X(size_); rp += X(blitSize_), transform.translation() = translation + X00<int32_t>(rp)) {
								int32_t ri = startIdx(X(viewOffset_) + rp, X(blitSize_), X(mapSize_));
								auto vertical = tiles[ri]();
								for (int32_t cp = startPos(Y(viewOffset_), Y(blitSize_)); cp < Y(size_); cp += Y(blitSize_), Y(transform.translation()) += Y(blitSize_)) {
									int32_t ci = startIdx(Y(viewOffset_) + cp, Y(blitSize_), Y(mapSize_));
									auto code = vertical[ci];
									material.uv0() = material.to_vector2({ ((code % cellBounds) + 0) * X(cellSize_), ((code / cellBounds) + 0) * Y(cellSize_), });
									material.uv1() = material.to_vector2({ ((code % cellBounds) + 1) * X(cellSize_), ((code / cellBounds) + 1) * Y(cellSize_), });

									renderer.drawRect(a, blitSize_, vector2{ 0.f, 0.f, }, vec<bool, 2>{ false, false, });
								}
							}
						} else {
							for (int32_t rp = startPos(Y(viewOffset_), Y(blitSize_)); rp < Y(size_); rp += Y(blitSize_), transform.translation() = translation + _0X0<int32_t>(rp)) {
								int32_t ri = startIdx(Y(viewOffset_) + rp, Y(blitSize_), Y(mapSize_));
								auto holizontal = tiles[ri]();
								for (int32_t cp = startPos(X(viewOffset_), X(blitSize_)); cp < X(size_); cp += X(blitSize_), X(transform.translation()) += X(blitSize_)) {
									int32_t ci = startIdx(X(viewOffset_) + cp, X(blitSize_), X(mapSize_));
									auto code = holizontal[ci];
									material.uv0() = material.to_vector2({ ((code % cellBounds) + 0) * X(cellSize_), ((code / cellBounds) + 0) * Y(cellSize_), });
									material.uv1() = material.to_vector2({ ((code % cellBounds) + 1) * X(cellSize_), ((code / cellBounds) + 1) * Y(cellSize_), });

									renderer.drawRect(a, blitSize_, vector2{ 0.f, 0.f, }, vec<bool, 2>{ false, false, });
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
