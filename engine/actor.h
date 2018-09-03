#pragma once
#include <clist.h>
#include <mtree.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
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
			transform<bool, int32_t>("count0", false, [](auto &count0) -> bool {
				return true, ++count0;
			}) || put<int32_t>("count0", 0);
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

		template<typename Vp>
		const Vp & put(const string &key, const Vp &value) {
			m_props.put<Vp>(key, value);
			return value;
		}

		template<typename Vp>
		Vp get(const string &key, const Vp &defvalue) const {
			return m_props.get<Vp>(key, defvalue);
		}

		template<typename Vt, typename Vp>
		Vt transform(const string &key, const Vt &defvalue, const function<Vt(const Vp &)> &operation) const {
			auto pNode = m_props.get_child_optional(key);
			return (pNode) ? operation(pNode->get_value<Vp>()) : defvalue;
		}

		template<typename Vt, typename Vp>
		Vt transform(const string &key, const Vt &defvalue, const function<Vt(Vp &)> &operation) {
			auto pNode = m_props.get_child_optional(key);
			if (!pNode) {
				return defvalue;
			}
			Vp value = pNode->get_value<Vp>();
			Vt rv = operation(value);
			pNode->put_value<Vp>(value);
			return rv;
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
