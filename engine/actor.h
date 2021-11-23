#pragma once
#include <clist.h>
#include <mtree.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <functional>
#include <iostream>
#include <optional>
#include <boost/property_tree/ptree.hpp>

namespace tte {
	using namespace boost;
	using namespace std;

	class Actor : public MTree<Actor> {
	private:
		class Components : public CList {
			//
			// public definitions
			//
		public:
			static constexpr uint32_t tag = makeTag("ROOT");

			//
			// public methods
			//
		public:
			Components() : CList(tag) {
			}

			virtual ~Components() override {
				while (next() != end()) {
					delete next();
				}
			}
		};

		//
		// public definitions
		//
	public:
		typedef function<void(Actor &)> Action;
		typedef function<bool(Actor &)> Trigger;

		//
		// member variables
		//
	private:
		Action m_actions;
		property_tree::ptree m_props;
		Components m_components;

		//
		// public methods
		//
	public:
		Actor(
			const Action &action,
			const Action &initializer
		) : MTree(), m_actions(action), m_props(), m_components() {
			initializer(*this);
		}

		virtual ~Actor() override {
			cout << "Actor::dtor" << endl;
			m_props.clear();
		}

		void appendAction(const Action &newAction) {
			m_actions = [oldActions = m_actions, newAction](Actor &a) {
				oldActions(a);
				newAction(a);
			};
		}

		void act() {
			auto count0 = m_props.get<int32_t>("count0", -1);
			m_props.put<int32_t>("count0", ++count0);
			m_actions(*this);
		}

		//
		// property methods
		//
	public:
		void importProps(const property_tree::ptree &props) {
			m_props.insert(m_props.end(), props.begin(), props.end());
		}

		property_tree::ptree & props() {
			return m_props;
		}

		const property_tree::ptree & props() const {
			return m_props;
		}

		property_tree::ptree & props(const string &key) {
			if (!m_props.get_child_optional(key)) {
				m_props.add_child(key, property_tree::ptree());
			}
			return m_props.get_child(key);
		}

		template<typename Vp>
		Vp get(const string &key, const Vp &defvalue) const {
			auto pNode = get_child_optional(m_props, key);
			return pNode ? pNode->get_value<Vp>(defvalue) : defvalue;
		}

	private:
		static std::optional<const property_tree::ptree> get_child_optional(const property_tree::ptree &node, property_tree::ptree::path_type key) {
			if (key.empty()) {
				return node;
			}

			auto head = key.reduce();
			auto name = !head.empty() && isdigit(head.front()) ? "" : head;
			auto index = !head.empty() && name.empty() ? std::stoul(head) : 0;

			auto matches = node.equal_range(name);
			if (matches.first == matches.second) {
				return std::nullopt;
			}

			while ((index > 0) && (matches.first != matches.second)) {
				--index;
				++matches.first;
			}
			if ((index > 0) || (matches.first == matches.second)) {
				return std::nullopt;
			}

			return get_child_optional(matches.first->second, key);
		}

		//
		// component operators
		//
	public:
		void appendComponent(CList *pComponent) {
			m_components.append(pComponent);
		}

		template<typename Vp>
		void getComponent(const function<void(Vp &)> &operation) {
			if (auto pComponent = m_components.get<Vp>()) {
				operation(*pComponent);
			}
		}

		template<typename Vp1, typename Vp2>
		void getComponent(const function<void(Vp1 &, Vp2 &)> &operation) {
			if (auto pComponent1 = m_components.get<Vp1>()) {
				if (auto pComponent2 = m_components.get<Vp2>()) {
					operation(*pComponent1, *pComponent2);
				}
			}
		}

		template<typename Vt, typename Vp>
		Vt getComponent(Vt &&defvalue, const function<Vt(Vp &)> &operation) {
			if (auto pComponent = m_components.get<Vp>()) {
				return operation(*pComponent);
			}
			return defvalue;
		}

		//
		// others
		//
	public:
		static void noAction(Actor &) {
		}
	};
}
