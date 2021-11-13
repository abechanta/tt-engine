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
#include <boost/optional.hpp>
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
			props("", [](property_tree::ptree& node) {
				node.put<int32_t>("count0", node.get<int32_t>("count0", -1) + 1);
			});
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

		optional<property_tree::ptree &> props(const string &key) {
			return get_child_optional<property_tree::ptree>(m_props, key);
		}

		optional<const property_tree::ptree &> props(const string &key) const {
			return get_child_optional<const property_tree::ptree>(m_props, key);
		}

		void props(const string &key, const function<void(property_tree::ptree &)> &operation) {
			auto pNode = get_child_optional<property_tree::ptree>(m_props, key);
			if (pNode) {
				operation(*pNode);
			}
		}

		void props(const string &key, const function<void(const property_tree::ptree &)> &operation) const {
			auto pNode = get_child_optional<const property_tree::ptree>(m_props, key);
			if (pNode) {
				operation(*pNode);
			}
		}

		template<typename Vp>
		void put(const string &key, const Vp &value) {
			if (auto pNode = get_child_optional<property_tree::ptree>(m_props, key)) {
				pNode->put_value<Vp>(value);
			}
		}

		template<typename Vp>
		Vp get(const string &key, const Vp &defvalue) const {
			auto pNode = get_child_optional<const property_tree::ptree>(m_props, key);
			return pNode ? pNode->get_value<Vp>(defvalue) : defvalue;
		}

		//template<typename Vt, typename Vp>
		//Vt transform(const string &key, const Vt &defvalue, const function<Vt(const Vp &)> &operation) const {
		//	auto pNode = get_child_optional<const property_tree::ptree>(m_props, key);
		//	return pNode ? operation(pNode->get_value<Vp>()) : defvalue;
		//}

		//template<typename Vt, typename Vp>
		//Vt transform(const string &key, const Vt &defvalue, const function<Vt(Vp &)> &operation) {
		//	auto pNode = get_child_optional<property_tree::ptree>(m_props, key);
		//	if (!pNode) {
		//		return defvalue;
		//	}
		//	Vp value = pNode->get_value<Vp>();
		//	Vt rv = operation(value);
		//	pNode->put_value<Vp>(value);
		//	return rv;
		//}

	private:
		template<typename V>
		static optional<V &> get_child_optional(V &node, property_tree::ptree::path_type key) {
			if (key.empty()) {
				return optional<V &>(node);
			}

			auto head = key.reduce();
			auto name = !head.empty() && isdigit(head.front()) ? "" : head;
			auto index = !head.empty() && name.empty() ? std::stoul(head) : 0;

			auto matches = node.equal_range(name);
			if (matches.first == matches.second) {
				return optional<V &>();
			}

			while ((index > 0) && (matches.first != matches.second)) {
				--index;
				++matches.first;
			}
			if ((index > 0) || (matches.first == matches.second)) {
				return optional<V &>();
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
		void findComponent(const function<void(Vp &)> &operation) {
			if (auto pComponent = m_components.find<Vp>()) {
				operation(*pComponent);
			}
		}

		template<typename Vp1, typename Vp2>
		void findComponent(const function<void(Vp1 &, Vp2 &)> &operation) {
			if (auto pComponent1 = m_components.find<Vp1>()) {
				if (auto pComponent2 = m_components.find<Vp2>()) {
					operation(*pComponent1, *pComponent2);
				}
			}
		}

		template<typename Vt, typename Vp>
		Vt findComponent(Vt &&defvalue, const function<Vt(Vp &)> &operation) {
			if (auto pComponent = m_components.find<Vp>()) {
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
