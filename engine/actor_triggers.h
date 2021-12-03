#pragma once
#include <actor.h>
#include <finder.h>
#include <components/input_component.h>
#include <cassert>
#include <cstdint>
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <utility>

namespace tte {
	using namespace std;

	inline Actor::Trigger operator+(const Actor::Trigger &lhs, const Actor::Trigger &rhs) {
		return [lhs, rhs](Actor &a) -> bool {
			return lhs(a) || rhs(a);
		};
	}

	inline Actor::Trigger operator*(const Actor::Trigger &lhs, const Actor::Trigger &rhs) {
		return [lhs, rhs](Actor &a) -> bool {
			return lhs(a) && rhs(a);
		};
	}

	inline Actor::Trigger inState(const string &stateName) {
		return [stateName](Actor &a) -> bool {
			auto pSrc = a.props().get_child_optional("state");
			return pSrc && (pSrc->get_value<string>().compare(stateName) == 0);
		};
	}

	inline Actor::Trigger onEvent(const string &eventName) {
		return [eventName](Actor &a) -> bool {
			auto pPool = a.props().get_child_optional("eventPool");
			if (!pPool) {
				return false;
			}
			auto pSrc = pPool->get_child_optional(eventName);
			if (!pSrc) {
				a.props().erase("event");
				return false;
			}
			a.props().put_child("event." + eventName, *pSrc);
			pPool->erase(eventName);
			return true;
		};
	}

	inline Actor::Trigger onButtonOn(const string &alias, uint32_t range = 1) {
		return [alias, range](Actor &) -> bool {
			return Finder<Actor>::find<bool>("input:", false, [alias, range](Actor &a) -> bool {
				return a.getComponent<bool, Input>(false, [alias, range](auto &input) -> bool {
					return input.buttons(alias).on(range);
				});
			});
		};
	}

	inline Actor::Trigger onButtonOff(const string &alias, uint32_t range = 1) {
		return [alias, range](Actor &) -> bool {
			return Finder<Actor>::find<bool>("input:", false, [alias, range](Actor &a) -> bool {
				return a.getComponent<bool, Input>(false, [alias, range](auto &input) -> bool {
					return input.buttons(alias).off(range);
				});
			});
		};
	}

	inline Actor::Trigger onButtonPressed(const string &alias, uint32_t range = 1, uint32_t needs = 1) {
		return [alias, range, needs](Actor &) -> bool {
			return Finder<Actor>::find<bool>("input:", false, [alias, range, needs](Actor &a) -> bool {
				return a.getComponent<bool, Input>(false, [alias, range, needs](auto &input) -> bool {
					return (input.buttons(alias).pressed(range) >= needs);
				});
			});
		};
	}

	inline Actor::Trigger onButtonReleased(const string &alias, uint32_t range = 1, uint32_t needs = 1) {
		return [alias, range, needs](Actor &) -> bool {
			return Finder<Actor>::find<bool>("input:", false, [alias, range, needs](Actor &a) -> bool {
				return a.getComponent<bool, Input>(false, [alias, range, needs](auto &input) -> bool {
					return (input.buttons(alias).released(range) >= needs);
				});
			});
		};
	}
}
