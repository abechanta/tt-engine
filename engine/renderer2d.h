#pragma once
#include <geometry.h>
#include <cassert>
#include <cstdint>
#include <vector>

namespace tte {
	using namespace boost;
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
		virtual void drawRect(Actor &a) const = 0;
		virtual void drawRect(Actor &a, const vector2 &pos, const vector2 &anchor, const vec<bool, 2> &flip) const = 0;
	};
}
