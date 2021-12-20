#pragma once
#include <actor.h>
#include <asset.h>
#include <cassert>
#include <clist.h>
#include <cmath>
#include <cstdint>
#include <functional>
#include <geometry.h>
#include <memory>
#include <vector>

namespace tte {
	using namespace std;

	class Renderer2dInterface {
		//
		// public definitions
		//
	private:
		std::unique_ptr<uint32_t, empty_delete<uint32_t> > m_handle;

		//
		// member variables
		//

		//
		// public methods
		//
	public:
		template<typename V>
		std::unique_ptr<V> &handle() {
			return *reinterpret_cast<std::unique_ptr<V> *>(&m_handle);
		}

		template<typename V>
		const std::unique_ptr<V> &handle() const {
			return *reinterpret_cast<const std::unique_ptr<V> *>(&m_handle);
		}

	public:
		virtual void drawPoint(Actor &a, const vector2 &point) const = 0;
		virtual void drawPoints(Actor &a, const vector<vector2> &points) const = 0;
		virtual void drawLine(Actor &a, const vector2 &lineS, const vector2 &lineE) const = 0;
		virtual void drawLines(Actor &a, const vector<vector2> &lineS, const vector<vector2> &lineE) const = 0;
		virtual void drawLines(Actor &a, const vector<vector2> &points, const vector<int32_t> &lines) const = 0;
		virtual void drawRect(Actor &a, const vector2 &pos, const vector2 &size, const vector2 &anchor, const vec<bool, 2> &flip) const = 0;
	};

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
		std::unique_ptr<Renderer2dInterface> m_handle;
		vector<matrix3x4> m_matrixStack;

		//
		// public methods
		//
	public:
		explicit Renderer2d(Actor &a, const function<void(Actor &, Renderer2d &)> &initializer)
			: CList(tag), m_matrixStack()
		{
			matrix3x4 id = Geometry::identity(matrix3x4());
			m_matrixStack.push_back(id);
			initializer(a, self());
		}

		virtual ~Renderer2d() override {
		}

		static Actor::Action append(const function<void(Actor &, Renderer2d &)> &initializer) {
			return [&initializer](Actor &a) {
				a.appendComponent(new Renderer2d(a, initializer));
			};
		}

		Renderer2d &pushMatrix() {
			m_matrixStack.push_back(m_matrixStack.back());
			return self();
		}

		Renderer2d &popMatrix() {
			m_matrixStack.pop_back();
			return self();
		}

		matrix3x4 &matrix() {
			return m_matrixStack.back();
		}

		//
		// property methods
		//
	public:
		void setRenderer(Renderer2dInterface *renderer) {
			m_handle.reset(renderer);
		}

		operator const Renderer2dInterface &() const {
			return *m_handle.get();
		}

		//
		// utility operators
		//
	private:
		Renderer2d &self() {
			return *this;
		}
	};
}
