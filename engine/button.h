#pragma once
#include <cassert>
#include <cstdint>

namespace tte {

	template<typename V = uint64_t>
	class Button {
		friend class Input;

		//
		// member variables
		//
	private:
		V m_bits;

		//
		// public methods
		//
	public:
		Button() : m_bits(0) {
		}

		virtual ~Button() {
		}

	private:
		void updateFrame() {
			m_bits = (m_bits << 1) | (m_bits & 1);
		}

	public:
		void update(bool onoff) {
			m_bits = (m_bits & ~1) | (onoff ? 1 : 0);
		}

		//
		// test methods
		//
	public:
		bool on(uint32_t range = 1) const {
			(range >= sizeof(V) * 8) && (range = sizeof(V) * 8 - 1);
			V maskbit = (static_cast<V>(1) << range) - 1;
			return (m_bits & maskbit) == maskbit;
		}

		bool off(uint32_t range = 1) const {
			(range >= sizeof(V) * 8) && (range = sizeof(V) * 8 - 1);
			V maskbit = (static_cast<V>(1) << range) - 1;
			return (~m_bits & maskbit) == maskbit;
		}

		uint32_t pressed(uint32_t range = 1) const {
			(range >= sizeof(V) * 8) && (range = sizeof(V) * 8 - 1);
			V maskbit = (static_cast<V>(1) << range) - 1;
			V bitsPrev = m_bits >> 1;
			return count(~bitsPrev & m_bits & maskbit);
		}

		uint32_t released(uint32_t range = 1) const {
			(range >= sizeof(V) * 8) && (range = sizeof(V) * 8 - 1);
			V maskbit = (static_cast<V>(1) << range) - 1;
			V bitsPrev = m_bits >> 1;
			return count(bitsPrev & ~m_bits & maskbit);
		}

		//
		// others
		//
	private:
		static uint32_t count(V bits) {
			uint32_t nbBits = 0;
			for (; bits > 0; bits >>= 1) {
				nbBits += bits & 1;
			}
			return nbBits;
		}
	};
}
