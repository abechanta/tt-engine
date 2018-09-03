#pragma once
#include <actor.h>
#include <asset.h>
#include <clist.h>
#include <geometry.h>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace tte {
	using namespace std;

	class Renderer2d : public CList {
		//
		// public definitions
		//
	public:
		static constexpr uint32_t tag = makeTag("RE2D");

		//
		// member variables
		//
	private:
		std::unique_ptr<uint32_t, empty_delete<uint32_t> > m_handle;
		vector<matrix3x4> m_matrixStack;
		function<void(Renderer2d &, Actor &)> m_renderer;

		//
		// public methods
		//
	public:
		Renderer2d(
			Actor &a,
			const function<void(Actor &, Renderer2d &)> &initializer
		) : CList(tag), m_matrixStack(), m_renderer(noRenderer) {
			matrix3x4 id = Geometry::identity(matrix3x4());
			m_matrixStack.push_back(id);
			initializer(a, *this);
		}

		virtual ~Renderer2d() override {
		}

		static Actor::Action append(const function<void(Actor &, Renderer2d &)> &initializer) {
			return [&initializer](Actor &a) {
				a.appendComponent(new Renderer2d(a, initializer));
			};
		}

		Renderer2d & pushMatrix() {
			m_matrixStack.push_back(m_matrixStack.back());
			return *this;
		}

		Renderer2d & popMatrix() {
			m_matrixStack.pop_back();
			return *this;
		}

		matrix3x4 & mat() {
			return m_matrixStack.back();
		}

		void render(Actor &a) {
			m_renderer(self(), a);
		}

		//
		// property methods
		//
	public:
		template<typename V>
		std::unique_ptr<V> & handle() {
			return *reinterpret_cast<std::unique_ptr<V> *>(&m_handle);
		}

		template<typename V>
		const std::unique_ptr<V> & handle() const {
			return *reinterpret_cast<const std::unique_ptr<V> *>(&m_handle);
		}

		void setRenderer(const function<void(Renderer2d &, Actor &)> &renderer) {
			m_renderer = renderer;
		}

		//
		// utility operators
		//
	public:
		static void noRenderer(Renderer2d &, Actor &) {
		}

	private:
		Renderer2d & self() {
			return *this;
		}
	};
}
