#pragma once
#include <actor.h>
#include <asset.h>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <components/animator_component.h>
#include <components/indexer_component.h>
#include <components/input_component.h>
#include <components/material_component.h>
// #include <components/renderer2d_component.h>
#include <components/resource_component.h>
#include <components/shape_component.h>
#include <components/transform_component.h>
#include <cstdint>
#include <finder.h>
#include <functional>
#include <string>
#include <utility>

namespace tte {
	using namespace std;

	inline Actor::Action operator+(const Actor::Action &lhs, const Actor::Action &rhs) {
		return [lhs, rhs](Actor &a) {
			lhs(a);
			rhs(a);
		};
	}

	inline Actor::Action operator*(const Actor::Trigger &lhs, const Actor::Action &rhs) {
		return [lhs, rhs](Actor &a) {
			if (lhs(a)) {
				rhs(a);
			}
		};
	}

	inline Actor::Action findThen(const string &key, const Actor::Action &action) {
		return [key, action](Actor &) {
			Finder<Actor>::find(key, action);
		};
	}

	template<typename Vc1>
	Actor::Action withComponent(const function<void(Actor &, Vc1 &)> &action) {
		return [action](Actor &a) {
			a.getComponent<Vc1>([action, &a](Vc1 &component1) {
				action(a, component1);
			});
		};
	}

	template<typename Vc1, typename Vc2>
	Actor::Action withComponent(const function<void(Actor &, Vc1 &, Vc2 &)> &action) {
		return [action](Actor &a) {
			a.getComponent<Vc1>([action, &a](Vc1 &component1) {
				a.getComponent<Vc1>([action, &a, &component1](Vc2 &component2) {
					action(a, component1, component2);
				});
			});
		};
	}

	inline Actor::Action exit() {
		return [](Actor &a) {
			delete &a;
		};
	}

	template<typename Vp>
	Actor::Action put(const string &key, const Vp &value) {
		return [key, value](Actor &a) {
			a.props(key).put_value<Vp>(value);
		};
	}

	inline Actor::Action changeState(const string &stateValue, const string &stateKey = "state") {
		return put<string>(stateKey, stateValue);
	}

	inline Actor::Action loadProps(const Asset &asset) {
		return [&asset](Actor &a) {
			if (asset.props().get<string>("contentType", "").compare("text/json") == 0) {
				a.importProps(asset.props());
			}
		};
	}

	inline Actor::Action appendComponents(Asset &assetBase) {
		return [&assetBase](Actor &a) {
			for (auto &component : a.props("components")) {
				auto componentType = component.second.get<string>("", "");
				if (componentType == "animator") {
					Animator::append()(a);
				}
				if (componentType == "indexer") {
					Indexer::append()(a);
				}
				if (componentType == "input") {
					Input::append()(a);
				}
				if (componentType == "material") {
					Material::append(assetBase, true)(a);
				}
				if (componentType == "resource") {
					Resource::append(assetBase, true)(a);
				}
				if (componentType == "sprite") {
					Shape::append<ShapeSprite>()(a);
				}
				if (componentType == "tilemap") {
					Shape::append<ShapeTilemap>()(a);
				}
				if (componentType == "text") {
					Shape::append<ShapeText>()(a);
				}
				if (componentType == "transform") {
					Transform::append()(a);
				}
			}
		};
	}

	inline Actor::Action loadProps(const Asset &asset, Asset &assetRoot) {
		return loadProps(asset) + appendComponents(assetRoot);
	}

	inline Actor::Action notifyEvent(const string &eventName, const function<property_tree::ptree(Actor &a)> &eventSetup = [](Actor &) -> property_tree::ptree { return property_tree::ptree(); }) {
		return [eventName, eventSetup](Actor &a) {
			a.props().add_child("eventPool." + eventName, eventSetup(a));
		};
	}
}
