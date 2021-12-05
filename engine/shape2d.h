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
