#pragma once
#include <actor.h>
#include <clist.h>
#include <geometry.h>
#include <components/renderer2d_component.h>
#include <shape2d.h>
#include <cassert>
#include <cstdint>
#include <functional>

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
				a.appendComponent(reinterpret_cast<CList *>(new V(a.props())));
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
	private:
		Shape2d::Sprite m_data;

	public:
		explicit ShapeSprite(
			const property_tree::ptree &props
		) : Shape(), m_data(Shape2d::Sprite::load(props.get_child("sprite"))) {
		}

		virtual ~ShapeSprite() override {
		}

		virtual void draw(Renderer2d &renderer, Actor &a) override {
			m_data.draw(renderer, a);
		}
	};

	class ShapeTilemap : public Shape {
	private:
		Shape2d::Tilemap m_data;

	public:
		explicit ShapeTilemap(
			const property_tree::ptree &props
		) : Shape(), m_data(Shape2d::Tilemap::load(props.get_child("tilemap"))) {
		}

		virtual ~ShapeTilemap() override {
		}

		virtual void draw(Renderer2d &renderer, Actor &a) override {
			m_data.draw(renderer, a);
		}
	};
}
