#pragma once
#include <actor.h>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <clist.h>
#include <cstdint>
#include <components/material_component.h>
#include <components/renderer2d_component.h>
#include <components/transform_component.h>
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
		static Actor::Action append(const string &rendererKey) {
			return [rendererKey](Actor &a) {
				a.appendComponent(reinterpret_cast<CList *>(new V(a.props(V::key))));
				a.appendAction([rendererKey](Actor &a) {
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
	private:
		Shape2d::Sprite m_data;

	public:
		explicit ShapeSprite(
			property_tree::ptree &props
		) : Shape(), m_data(props) {
		}

		virtual ~ShapeSprite() override {
		}

		virtual void draw(Renderer2d &renderer, Actor &a) override {
			a.getComponent<Transform>([this, &a, &renderer](auto &transform) {
				renderer.pushMatrix();
				transform.trs2d(renderer.mat());
				{
					const Renderer2dInterface &renderer2d = renderer;
					renderer2d.drawRect(a, vector2{ 0.f, 0.f, }, m_data.size(), m_data.anchor(), m_data.flip());
				}
				renderer.popMatrix();
			});
		}
	};

	class ShapeTilemap : public Shape {
	public:
		static inline const string key = "tilemap";
	private:
		Shape2d::Tilemap m_data;

	public:
		explicit ShapeTilemap(
			property_tree::ptree &props
		) : Shape(), m_data(props) {
		}

		virtual ~ShapeTilemap() override {
		}

		virtual void draw(Renderer2d &renderer, Actor &a) override {
			a.getComponent<Transform, Material>([this, &a, &renderer](auto &transform, auto &material) {
				auto size_ = m_data.size();
				auto viewOffset_ = m_data.viewOffset();
				auto cellSize_ = m_data.cellSize();
				auto blitSize_ = m_data.blitSize();
				auto mapSize_ = m_data.transpose() ? vector2i{ Y(m_data.mapSize()), X(m_data.mapSize()), } : m_data.mapSize();
				const int32_t cellBounds = X(material.size()) / X(cellSize_);

				auto translation = transform.translation();
				auto rotation = transform.rotation();
				auto scaling = transform.scaling();
				const vector2 uv0 = material.uv0();
				const vector2 uv1 = material.uv1();
				renderer.pushMatrix();
				Geometry::trs2d(renderer.mat(), translation, rotation, scaling);
				{
					if (m_data.transpose()) {
						for (int32_t rp = startPos(X(viewOffset_), X(blitSize_)); rp < X(size_); rp += X(blitSize_)) {
							int32_t ri = startIdx(X(viewOffset_) + rp, X(blitSize_), X(mapSize_));
							auto vertical = m_data.tiles[ri]();
							for (int32_t cp = startPos(Y(viewOffset_), Y(blitSize_)); cp < Y(size_); cp += Y(blitSize_)) {
								int32_t ci = startIdx(Y(viewOffset_) + cp, Y(blitSize_), Y(mapSize_));
								auto code = vertical[ci];
								material.uv0() = material.to_vector2({ ((code % cellBounds) + 0) * X(cellSize_), ((code / cellBounds) + 0) * Y(cellSize_), });
								material.uv1() = material.to_vector2({ ((code % cellBounds) + 1) * X(cellSize_), ((code / cellBounds) + 1) * Y(cellSize_), });

								const Renderer2dInterface &renderer2d = renderer;
								renderer2d.drawRect(a, vector2i{ rp, cp, }, blitSize_, vector2{ 0.f, 0.f, }, vec<bool, 2>{ false, false, });
							}
						}
					} else {
						for (int32_t rp = startPos(Y(viewOffset_), Y(blitSize_)); rp < Y(size_); rp += Y(blitSize_)) {
							int32_t ri = startIdx(Y(viewOffset_) + rp, Y(blitSize_), Y(mapSize_));
							auto holizontal = m_data.tiles[ri]();
							for (int32_t cp = startPos(X(viewOffset_), X(blitSize_)); cp < X(size_); cp += X(blitSize_)) {
								int32_t ci = startIdx(X(viewOffset_) + cp, X(blitSize_), X(mapSize_));
								auto code = holizontal[ci];
								material.uv0() = material.to_vector2({ ((code % cellBounds) + 0) * X(cellSize_), ((code / cellBounds) + 0) * Y(cellSize_), });
								material.uv1() = material.to_vector2({ ((code % cellBounds) + 1) * X(cellSize_), ((code / cellBounds) + 1) * Y(cellSize_), });

								const Renderer2dInterface &renderer2d = renderer;
								renderer2d.drawRect(a, vector2i{ cp, rp, }, blitSize_, vector2{ 0.f, 0.f, }, vec<bool, 2>{ false, false, });
							}
						}
					}
				}
				renderer.popMatrix();
				material.uv0() = uv0;
				material.uv1() = uv1;
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
}
