#pragma once
#include <actor.h>
#include <cassert>
#include <cstdint>
#include <finder.h>
#include <functional>
#include <string>
#include <utility>

namespace tte {
	using namespace std;

	inline void appendActionTo(const string &key, const Actor::Action &action) {
		Finder<Actor>::find(key, [action](Actor &a) {
			a.appendAction(action);
		});
	}

	inline void appendChildTo(const string &key, const Actor::Action &initializer) {
		Finder<Actor>::find(key, [initializer](Actor &a) {
			a.appendChild(new Actor(Actor::noAction, initializer));
		});
	}
}
