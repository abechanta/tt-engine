#pragma once
#include <actor.h>
#include <cassert>
#include <clist.h>
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

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
			onoff ? (m_bits |= 1) : (m_bits &= ~1);
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

	class Input : public CList {
		//
		// public definitions
		//
	public:
		static constexpr uint32_t tag = makeTag("INPT");

		//
		// member variables
		//
	private:
		unordered_map<string, Button<> > m_buttons;

		//
		// public methods
		//
	public:
		explicit Input()
			: CList(tag), m_buttons()
		{
		}

		virtual ~Input() override {
		}

		static Actor::Action append() {
			return [](Actor &a) {
				a.appendComponent(new Input());
			};
		}

		void update() {
			for (auto &button : m_buttons) {
				button.second.updateFrame();
			}
		}

		//
		// property methods
		//
	public:
		Button<> &buttons(const string &alias) {
			return m_buttons[alias];
		}
	};
}
