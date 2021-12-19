#pragma once
#include <array>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <cstdint>
#include <geometry.h>
#include <ptree.h>
#include <string>
#include <vector>

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
			PTree::PropertyV<vec, int32_t, 2> cellSize;
			PTree::PropertyV<vec, float, 2> anchor;
			PTree::PropertyV<vec, bool, 2> flip;
			PTree::Property<int32_t> code;

			Sprite(property_tree::ptree &node)
				: size(node, "size", 8.f),
				cellSize(node, "cellSize", 8),
				anchor(node, "anchor", .5f),
				flip(node, "flip", false),
				code(node, "code", 0)
			{
			}
		};

		struct Tilemap {
			PTree::PropertyV<vec, int32_t, 2> size;
			PTree::PropertyV<vec, int32_t, 2> viewOffset;
			PTree::PropertyV<vec, int32_t, 2> cellSize;
			PTree::PropertyV<vec, int32_t, 2> blitSize;
			PTree::PropertyV<vec, int32_t, 2> tileSize;
			PTree::Property<bool> transpose;
			PTree::PropertyAA<vector<int32_t>> tiles;

			Tilemap(property_tree::ptree &node)
				: size(node, "size", 8),
				viewOffset(node, "viewOffset", 0),
				cellSize(node, "cellSize", 8),
				blitSize(node, "blitSize", 8),
				tileSize(node, "tileSize", 8),
				transpose(node, "transpose", false),
				tiles(node, "tiles")
			{
			}

			vector2i getAddress(const vector2i &wp) {
				vector2i blitSize_ = blitSize();
				return vector2i{ X(wp) / X(blitSize_), Y(wp) / Y(blitSize_), };
			}

			vector2i getWp(const vector2i &addr) {
				vector2i blitSize_ = blitSize();
				return vector2i{ X(addr) * X(blitSize_), Y(addr) * Y(blitSize_), };
			}

			int32_t peek(const vector2i &addr) {
				const vector2i tileSize_ = tileSize();
				vector2i addr_ = transpose() ? vector2i{ Y(addr), X(addr), } : addr;
				Y(addr_) = Geometry::modulo(Y(addr_), Y(tileSize_));
				assert(Y(addr_) < tiles.size());
				auto horizontal = tiles[Y(addr_)]();
				X(addr_) = Geometry::modulo(X(addr_), X(tileSize_));
				assert(X(addr_) < horizontal.size());
				return horizontal[X(addr_)];
			}
		};

		struct Text {
			PTree::PropertyV<vec, int32_t, 2> size;
			PTree::PropertyV<vec, int32_t, 2> cellSize;
			PTree::PropertyV<vec, int32_t, 2> blitSize;
			PTree::PropertyV<array, string, 2> alignment;
			PTree::PropertyA<vector<string> > lines;

			Text(property_tree::ptree &node)
				: size(node, "size", 8),
				cellSize(node, "cellSize", 8),
				blitSize(node, "blitSize", 8),
				alignment(node, "alignment", "center"),
				lines(node, "lines")
			{
				auto alignment_ = alignment();
				if (Geometry::alignmentConv.find(alignment_[0]) == Geometry::alignmentConv.end()) {
					alignment_[0] = "center";
					alignment(alignment_);
				}
				if (Geometry::alignmentConv.find(alignment_[1]) == Geometry::alignmentConv.end()) {
					alignment_[1] = "center";
					alignment(alignment_);
				}
			}
		};

		struct TiledWindow {
			// TODO
		};
	}
}
