#pragma once
#include <actor.h>
#include <array>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <cstdint>
#include <clist.h>
#include <components/material_component.h>
#include <components/renderer2d_component.h>
#include <components/transform_component.h>
#include <cstdint>
#include <finder.h>
#include <geometry.h>
#include <ptree.h>
#include <string>
#include <vector>

namespace tte {
	using namespace boost;
	using namespace std;
	using ptree = property_tree::ptree;

	class Shape : public CList {
		//
		// public definitions
		//
	public:
		static constexpr uint32_t tag = makeTag("SHAP");

		//
		// member variables
		//

		//
		// public methods
		//
	public:
		explicit Shape() : CList(tag) {
		}

		virtual ~Shape() override {
		}

		virtual void draw(Renderer2d &renderer, Actor &a) = 0;

		template<typename V>
		static Actor::Action append() {
			return [](Actor &a) {
				a.appendComponent(reinterpret_cast<CList *>(new V(a.props(V::key))));
				a.appendAction([](Actor &a) {
					auto rendererKey = a.props(V::key).get<string>("renderer", "renderer");
					Finder<Actor>::find<Renderer2d>(rendererKey, [&a](auto &renderer) {
						a.getComponent<V>([&renderer, &a](auto &shape) {
							shape.draw(renderer, a);
						});
					});
				});
			};
		}

		//
		// property methods
		//
	};

	class ShapeSprite : public Shape {
	public:
		static inline const string key = "sprite";
	public:
		PTree::PropertyV<vector2> size;
		PTree::PropertyV<vector2i> cellSize;
		PTree::PropertyV<vector2> anchor;
		PTree::PropertyV<vec<bool, 2> > flip;
		PTree::Property<int32_t> code;

	public:
		explicit ShapeSprite(ptree &node)
			: Shape(),
			size(node, "size", 8.f),
			cellSize(node, "cellSize", 8),
			anchor(node, "anchor", .5f),
			flip(node, "flip", false),
			code(node, "code", 0)
		{
		}

		virtual ~ShapeSprite() override {
		}

		virtual void draw(Renderer2d &renderer, Actor &a) override {
			a.getComponent<Transform, Material>([this, &a, &renderer](auto &transform, auto &material) {
				auto size_ = size.get();
				auto cellSize_ = cellSize.get();
				auto anchor_ = anchor.get();
				auto flip_ = flip.get();
				auto code_ = code.get();
				const int32_t cellBounds = X(material.size()) / X(cellSize_);

				renderer.pushMatrix();
				transform.trs2d(renderer.matrix());
				material.resetUv();
				{
					auto uv = Geometry::mul_elements<vector2i>(vector2i{ code_ % cellBounds, code_ / cellBounds, }, cellSize_);
					material._uv0(material.to_ratio(uv));
					material._uv1(material.to_ratio(uv + cellSize_));
					auto anchorOffset = Geometry::mul_elements<vector2>(anchor_, scalar_cast<float>(cellSize_));

					const Renderer2dInterface &renderer2d = renderer;
					renderer2d.drawRect(a, -anchorOffset, size_, anchor_, flip_);
				}
				material.resetUv();
				renderer.popMatrix();
			});
		}
	};

	class ShapeTilemap : public Shape {
	public:
		static inline const string key = "tilemap";
	public:
		PTree::PropertyV<vector2i> size;
		PTree::PropertyV<vector2i> viewOffset;
		PTree::PropertyV<vector2i> cellSize;
		PTree::PropertyV<vector2i> blitSize;
		PTree::PropertyV<vector2i> tileSize;
		PTree::Property<bool> transpose;
		PTree::PropertyAA<vector<int32_t> > tiles;

	public:
		explicit ShapeTilemap(ptree &node)
			: Shape(),
			size(node, "size", 8),
			viewOffset(node, "viewOffset", 0),
			cellSize(node, "cellSize", 8),
			blitSize(node, "blitSize", 8),
			tileSize(node, "tileSize", 8),
			transpose(node, "transpose", false),
			tiles(node, "tiles")
		{
		}

		virtual ~ShapeTilemap() override {
		}

		virtual void draw(Renderer2d &renderer, Actor &a) override {
			a.getComponent<Transform, Material>([this, &a, &renderer](auto &transform, auto &material) {
				auto size_ = size.get();
				auto viewOffset_ = viewOffset.get();
				auto cellSize_ = cellSize.get();
				auto blitSize_ = blitSize.get();
				auto tileSize_ = tileSize.get();
				if (transpose.get()) {
					tileSize_ = YX(tileSize_);
				}
				const int32_t cellBounds = X(material.size()) / X(cellSize_);

				auto translation = transform.translation.get();
				auto rotation = transform.rotation.get();
				auto scaling = transform.scaling.get();
				renderer.pushMatrix();
				Geometry::trs2d(renderer.matrix(), translation, rotation, scaling);
				material.resetUv();
				{
					const auto &anchor = zero_vec<float, 2>();
					const auto &flip = zero_vec<bool, 2>();

					if (transpose.get()) {
						for (int32_t rp = startPos(X(viewOffset_), X(blitSize_)); rp < X(size_); rp += X(blitSize_)) {
							int32_t ri = startIdx(X(viewOffset_) + rp, X(blitSize_), X(tileSize_));
							auto vertical = tiles[ri].get();
							for (int32_t cp = startPos(Y(viewOffset_), Y(blitSize_)); cp < Y(size_); cp += Y(blitSize_)) {
								int32_t ci = startIdx(Y(viewOffset_) + cp, Y(blitSize_), Y(tileSize_));
								auto code = vertical[ci];
								auto uv = Geometry::mul_elements<vector2i>(vector2i{ code % cellBounds, code / cellBounds, }, cellSize_);
								material._uv0(material.to_ratio(uv));
								material._uv1(material.to_ratio(uv + cellSize_));
								auto pos = vector2i{ rp, cp, };

								const Renderer2dInterface &renderer2d = renderer;
								renderer2d.drawRect(a, scalar_cast<float>(pos), scalar_cast<float>(blitSize_), anchor, flip);
							}
						}
					} else {
						for (int32_t rp = startPos(Y(viewOffset_), Y(blitSize_)); rp < Y(size_); rp += Y(blitSize_)) {
							int32_t ri = startIdx(Y(viewOffset_) + rp, Y(blitSize_), Y(tileSize_));
							auto horizontal = tiles[ri].get();
							for (int32_t cp = startPos(X(viewOffset_), X(blitSize_)); cp < X(size_); cp += X(blitSize_)) {
								int32_t ci = startIdx(X(viewOffset_) + cp, X(blitSize_), X(tileSize_));
								auto code = horizontal[ci];
								auto uv = Geometry::mul_elements<vector2i>(vector2i{ code % cellBounds, code / cellBounds, }, cellSize_);
								material._uv0(material.to_ratio(uv));
								material._uv1(material.to_ratio(uv + cellSize_));
								auto pos = vector2i{ rp, cp, };

								const Renderer2dInterface &renderer2d = renderer;
								renderer2d.drawRect(a, scalar_cast<float>(pos), scalar_cast<float>(blitSize_), anchor, flip);
							}
						}
					}
				}
				material.resetUv();
				renderer.popMatrix();
			});
		}

	private:
		static int32_t startPos(int32_t pos, int32_t size) {
			int32_t val = pos % size;
			val += (pos < 0) ? size : 0;
			return -val;
		}

		static int32_t startIdx(int32_t pos, int32_t size, int32_t wrap) {
			int32_t val = pos / size;
			val %= wrap;
			val += (val < 0) ? wrap : 0;
			return val;
		}

	public:
		vector2i getAddress(const vector2i &wp) {
			vector2i blitSize_ = blitSize.get();
			return Geometry::div_elements<vector2i>(wp, blitSize_);
		}

		vector2i getWp(const vector2i &addr) {
			vector2i blitSize_ = blitSize.get();
			return Geometry::mul_elements(addr, blitSize_);
		}

		int32_t peek(const vector2i &addr) {
			const vector2i tileSize_ = tileSize.get();
			vector2i addr_ = addr;
			if (transpose.get()) {
				addr_ = YX(addr_);
			}
			Y(addr_) = Geometry::modulo(Y(addr_), Y(tileSize_));
			assert(Y(addr_) < tiles.size());
			auto horizontal = tiles[Y(addr_)].get();
			X(addr_) = Geometry::modulo(X(addr_), X(tileSize_));
			assert(X(addr_) < horizontal.size());
			return horizontal[X(addr_)];
		}
	};

	class ShapeText : public Shape {
	public:
		static inline const string key = "text";
	public:
		PTree::PropertyV<vector2i> size;
		PTree::PropertyV<vector2i> cellSize;
		PTree::PropertyV<vector2i> blitSize;
		PTree::PropertyA<vector<string> > alignment;
		PTree::PropertyA<vector<string> > lines;

	public:
		explicit ShapeText(ptree &node)
			: Shape(),
			size(node, "size", 8),
			cellSize(node, "cellSize", 8),
			blitSize(node, "blitSize", 8),
			alignment(node, "alignment"),
			lines(node, "lines")
		{
			auto alignment_ = alignment.get();
			if (Geometry::alignmentConv.find(alignment_[0]) == Geometry::alignmentConv.end()) {
				alignment_[0] = "center";
				alignment.set(alignment_);
			}
			if (Geometry::alignmentConv.find(alignment_[1]) == Geometry::alignmentConv.end()) {
				alignment_[1] = "center";
				alignment.set(alignment_);
			}
		}

		virtual ~ShapeText() override {
		}

		virtual void draw(Renderer2d &renderer, Actor &a) override {
			a.getComponent<Transform, Material>([this, &a, &renderer](auto &transform, auto &material) {
				auto size_ = size.get();
				auto cellSize_ = cellSize.get();
				auto blitSize_ = blitSize.get();
				auto alignment_ = Geometry::conv_elements<vector2>(Geometry::alignmentConv, alignment.get());
				auto lines_ = lines.get();
				const int32_t cellBounds = X(material.size()) / X(cellSize_);

				auto translation = transform.translation.get();
				auto rotation = transform.rotation.get();
				auto scaling = transform.scaling.get();
				renderer.pushMatrix();
				Geometry::trs2d(renderer.matrix(), translation, rotation, scaling);
				material.resetUv();
				{
					const auto &anchor = zero_vec<float, 2>();
					const auto &flip = zero_vec<bool, 2>();

					for (int32_t r = 0; r < lines_.size(); r++) {
						int32_t rp = r * Y(blitSize_);
						auto offset = Geometry::mul_elements<vector2i>(
							vector2i{ static_cast<int32_t>(lines_[r].length()), static_cast<int32_t>(lines_.size()), },
							scalar_cast<int32_t>(Geometry::mul_elements<vector2>(alignment_, scalar_cast<float>(blitSize_)))
						);
						for (int32_t c = 0; c < lines_[r].length(); c++) {
							int32_t cp = c * X(blitSize_);
							auto code = lines_[r][c];
							auto uv = Geometry::mul_elements<vector2i>(vector2i{ code % cellBounds, code / cellBounds, }, cellSize_);
							material._uv0(material.to_ratio(uv));
							material._uv1(material.to_ratio(uv + cellSize_));
							auto pos = vector2i{ cp - X(offset), rp - Y(offset), };

							const Renderer2dInterface &renderer2d = renderer;
							renderer2d.drawRect(a, scalar_cast<float>(pos), scalar_cast<float>(blitSize_), anchor, flip);
						}
					}
				}
				material.resetUv();
				renderer.popMatrix();
			});
		}
	};
}
