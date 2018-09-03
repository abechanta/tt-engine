#pragma once
#include <actor.h>
#include <clist.h>
#include <geometry.h>
#include <cassert>
#include <cstdint>
#include <functional>
#include <initializer_list>

namespace tte {
	class Primitive : public CList {
		//
		// public definitions
		//
	public:
		static constexpr uint32_t tag = makeTag("PRIM");

		enum Shape {
			DrawPoints,
			DrawLines,
			DrawRects,
		};

		//
		// member variables
		//
	private:
		Shape m_shape;
		vector3 m_pos;
		vector3 m_size;
		vector4 m_color;

		//
		// public methods
		//
	public:
		Primitive(
			const vector3 &pos, const vector3 &size
		) : CList(tag), m_shape(DrawRects), m_pos(pos), m_size(size), m_color() {
		}

		virtual ~Primitive() override {
		}

		static Actor::Action append(const vector3 &pos, const vector3 &size) {
			return [&pos, &size](Actor &a) {
				a.appendComponent(new Primitive(pos, size));
			};
		}

		//
		// property methods
		//
	public:
		vector3 & pos() {
			return m_pos;
		}

		const vector3 & pos() const {
			return m_pos;
		}

		vector3 & size() {
			return m_size;
		}

		const vector3 & size() const {
			return m_size;
		}

		vector4 & color() {
			return m_color;
		}

		const vector4 & color() const {
			return m_color;
		}
	};
}
