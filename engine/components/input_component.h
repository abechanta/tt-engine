#pragma once
#include <actor.h>
#include <button.h>
#include <cassert>
#include <clist.h>
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

namespace tte {
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
