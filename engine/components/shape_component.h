#pragma once
#include <actor.h>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <clist.h>
#include <components/material_component.h>
#include <components/renderer2d_component.h>
#include <components/transform_component.h>
#include <cstdint>
#include <finder.h>
#include <geometry.h>
#include <renderer2d.h>
#include <shape2d.h>

namespace tte {
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
		Shape2d::Sprite m_data;

	public:
		explicit ShapeSprite(property_tree::ptree &props)
			: Shape(), m_data(props)
		{
		}

		virtual ~ShapeSprite() override {
		}

		virtual void draw(Renderer2d &renderer, Actor &a) override {
			a.getComponent<Transform, Material>([this, &a, &renderer](auto &transform, auto &material) {
				auto size_ = m_data.size();
				auto cellSize_ = m_data.cellSize();
				auto anchor_ = m_data.anchor();
				auto flip_ = m_data.flip();
				auto code_ = m_data.code();
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
		Shape2d::Tilemap m_data;

	public:
		explicit ShapeTilemap(property_tree::ptree &props)
			: Shape(), m_data(props)
		{
		}

		virtual ~ShapeTilemap() override {
		}

		virtual void draw(Renderer2d &renderer, Actor &a) override {
			a.getComponent<Transform, Material>([this, &a, &renderer](auto &transform, auto &material) {
				auto size_ = m_data.size();
				auto viewOffset_ = m_data.viewOffset();
				auto cellSize_ = m_data.cellSize();
				auto blitSize_ = m_data.blitSize();
				auto tileSize_ = m_data.tileSize();
				if (m_data.transpose()) {
					tileSize_ = YX(tileSize_);
				}
				const int32_t cellBounds = X(material.size()) / X(cellSize_);

				auto translation = transform.translation();
				auto rotation = transform.rotation();
				auto scaling = transform.scaling();
				renderer.pushMatrix();
				Geometry::trs2d(renderer.matrix(), translation, rotation, scaling);
				material.resetUv();
				{
					const auto &anchor = zero_vec<float, 2>();
					const auto &flip = zero_vec<bool, 2>();

					if (m_data.transpose()) {
						for (int32_t rp = startPos(X(viewOffset_), X(blitSize_)); rp < X(size_); rp += X(blitSize_)) {
							int32_t ri = startIdx(X(viewOffset_) + rp, X(blitSize_), X(tileSize_));
							auto vertical = m_data.tiles[ri]();
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
							auto horizontal = m_data.tiles[ri]();
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
	};

	class ShapeText : public Shape {
	public:
		static inline const string key = "text";
	public:
		Shape2d::Text m_data;

	public:
		explicit ShapeText(property_tree::ptree &props)
			: Shape(), m_data(props)
		{
		}

		virtual ~ShapeText() override {
		}

		virtual void draw(Renderer2d &renderer, Actor &a) override {
			a.getComponent<Transform, Material>([this, &a, &renderer](auto &transform, auto &material) {
				auto size_ = m_data.size();
				auto cellSize_ = m_data.cellSize();
				auto blitSize_ = m_data.blitSize();
				auto alignment_ = Geometry::conv_elements<vector2>(Geometry::alignmentConv, m_data.alignment());
				auto lines_ = m_data.lines();
				const int32_t cellBounds = X(material.size()) / X(cellSize_);

				auto translation = transform.translation();
				auto rotation = transform.rotation();
				auto scaling = transform.scaling();
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
