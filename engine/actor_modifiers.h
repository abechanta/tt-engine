#pragma once
#include <actor.h>
#include <asset.h>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <cstdint>
#include <finder.h>
#include <functional>
#include <string>
#include <utility>

namespace tte {
	using namespace std;

	Actor::Action operator+(const Actor::Action &lhs, const Actor::Action &rhs)
#if defined(tte_declare_static_variables)
	{
		return [lhs, rhs](Actor &a) {
			lhs(a);
			rhs(a);
		};
	}
#else
	;
#endif	// defined(tte_declare_static_variables)

	Actor::Action operator*(const Actor::Trigger &lhs, const Actor::Action &rhs)
#if defined(tte_declare_static_variables)
	{
		return [lhs, rhs](Actor &a) {
			if (lhs(a)) {
				rhs(a);
			}
		};
	}
#else
		;
#endif	// defined(tte_declare_static_variables)

	inline Actor::Action findAndAct(const string &key, const Actor::Action &action) {
		return [key, action](Actor &a) {
			Finder<Actor>::find(key, action);
		};
	}

	template<typename Vc1>
	Actor::Action componentModifier(const function<void(Actor &, Vc1 &)> &action) {
		return [action](Actor &a) {
			a.getComponent<Vc1>([action, &a](Vc1 &component1) {
				action(a, component1);
			});
		};
	}

	template<typename Vc1>
	Actor::Action componentModifier(const string &key, const function<void(Actor &, Vc1 &)> &action) {
		return [key, action](Actor &a) {
			Finder<Actor>::find<Vc1>(key, [action, &a](auto &component1) {
				action(a, component1);
			});
		};
	}

	template<typename Vc1, typename Vc2>
	Actor::Action componentModifier(const function<void(Actor &, Vc1 &, Vc2 &)> &action) {
		return [action](Actor &a) {
			a.getComponent<Vc1>([action, &a](Vc1 &component1) {
				a.getComponent<Vc1>([action, &a, &component1](Vc2 &component2) {
					action(a, component1, component2);
				});
			});
		};
	}

	Actor::Action actChildren()
#if defined(tte_declare_static_variables)
	{
		return [](Actor &a) {
			for_each(a.begin(true), a.end(), [](Actor &a) {
				a.act();
			});
		};
	}
#else
		;
#endif	// defined(tte_declare_static_variables)

	template<typename Vp>
	Actor::Action put(const string &key, const Vp &value) {
		return [key, value](Actor &a) {
			a.props().put<Vp>(key, value);
		};
	}

	Actor::Action loadProps(const Asset &asset)
#if defined(tte_declare_static_variables)
	{
		return [&asset](Actor &a) {
			if (asset.props().get<string>("contentType").compare("text/json") == 0) {
				a.importProps(asset.props());
			}
		};
	}
#else
		;
#endif	// defined(tte_declare_static_variables)

	Actor::Action changeState(const string &stateName)
#if defined(tte_declare_static_variables)
	{
		return [stateName](Actor &a) {
			a.props().put<string>("state", stateName);
		};
	}
#else
		;
#endif	// defined(tte_declare_static_variables)

	Actor::Action notifyEvent(const string &eventName, const function<property_tree::ptree(Actor &a)> &eventSetup = [](Actor &) -> property_tree::ptree { return property_tree::ptree(); })
#if defined(tte_declare_static_variables)
	{
		return [eventName, eventSetup](Actor &a) {
			a.props().add_child("eventPool." + eventName, eventSetup(a));
		};
	}
#else
		;
#endif	// defined(tte_declare_static_variables)
}
