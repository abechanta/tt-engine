#pragma once
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>

namespace tte {
	using namespace boost;
	using namespace std;

	namespace PTree {
		static inline const initializer_list<string> subkeysXYZW = { "x", "y", "z", "w", };
		static inline const initializer_list<string> subkeysWH = { "w", "h", };
		static inline const initializer_list<string> subkeysXYWH = { "x", "y", "w", "h", };
		static inline const initializer_list<string> subkeysRGBA = { "r", "g", "b", "a", };

		inline property_tree::ptree &get_child(property_tree::ptree &node, property_tree::ptree::path_type key) {
			if (key.empty()) {
				return node;
			}

			auto head = key.reduce();
			auto name = !head.empty() && isdigit(head.front()) ? "" : head;
			auto index = !head.empty() && name.empty() ? std::stol(head) : 0;

			auto matches = node.equal_range(name);
			if (matches.first == matches.second) {
				node.push_back(make_pair(name, property_tree::ptree()));
				matches = node.equal_range(name);
				assert(matches.first != matches.second);
			}

			assert(index >= 0);
			while (matches.first != matches.second) {
				if (index == 0) {
					return get_child(matches.first->second, key);
				}
				--index;
				++matches.first;
			}
			do {
				node.push_back(make_pair(name, property_tree::ptree()));
			} while (index-- > 0);
			return get_child(node.back().second, key);
		}

		inline std::optional<property_tree::ptree> get_child_optional(property_tree::ptree &node, property_tree::ptree::path_type key) {
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

		inline std::optional<const property_tree::ptree> get_child_optional(const property_tree::ptree &node, property_tree::ptree::path_type key) {
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

		template<typename V>
		class Property {
		private:
			property_tree::ptree &m_node;
			const string m_key;
			const V m_defval;

		public:
			Property(property_tree::ptree &node, const string &key, const V &defval = 0)
				: m_node(node), m_key(key), m_defval(defval)
			{
			}

			V operator()() const {
				return m_node.get<V>(m_key, m_defval);
			}

			void operator()(const V &val) {
				m_node.put<V>(m_key, val);
			}

			ostream &to_string(ostream &os) const {
				auto val = operator()();
				os << m_key << ": " << val << "]";
				return os;
			}
		};

		template<class C>
		class PropertyA {
		private:
			property_tree::ptree &m_node;
			const string m_key;

		public:
			PropertyA(property_tree::ptree &node, const string &key)
				: m_node(get_child(node, key)), m_key(key)
			{
			}

			C operator()() const {
				C val = {};
				for (auto &ch : m_node) {
					val.push_back(ch.second.get<C::value_type>(""));
				}
				return val;
			}

			void operator()(const C &val) {
				m_node.clear();
				for (auto elm : val) {
					m_node.push_back(make_pair("", elm));
				}
			}

			ostream &to_string(ostream &os) const {
				auto it = m_subkeys.begin();
				auto val = operator()();
				os << m_key << ": [ ";
				for (auto &e : val.a) {
					os << e << ", ";
				}
				os << "]";
				return os;
			}
		};

		template<class C>
		class PropertyAA : public vector<PTree::PropertyA<C>> {
		private:
			property_tree::ptree &m_node;
			const string m_key;

		public:
			PropertyAA(property_tree::ptree &node, const string &key)
				: m_node(get_child(node, key)), m_key(key)
			{
				for (auto &ch : m_node) {
					PTree::PropertyA<C> val(ch.second, "");
					push_back(val);
				}
			}

			ostream &to_string(ostream &os) const {
				auto it = m_subkeys.begin();
				os << m_key << ": [ ";
				for (auto &e : *this) {
					os << *it++ << ": " << e << ", ";
				}
				os << "]";
				return os;
			}
		};

		template<template<typename, int> class C, typename V, int N>
		class PropertyV {
		private:
			property_tree::ptree &m_node;
			const string m_key;
			const V m_defval;
			const initializer_list<string> m_subkeys;

		public:
			PropertyV(property_tree::ptree &node, const string &key, const V defval = 0, const initializer_list<string> subkeys = subkeysXYZW)
				: m_node(get_child(node, key)), m_key(key), m_defval(defval), m_subkeys(subkeys)
			{
			}

			PropertyV(property_tree::ptree &node, const string &key, const C<V, N> &val, const initializer_list<string> subkeys = subkeysXYZW)
				: m_node(get_child(node, key)), m_key(key), m_defval(0), m_subkeys(subkeys)
			{
				operator()(val);
			}

			C<V, N> operator()() const {
				return get(m_node, m_defval, m_subkeys);
			}

			void operator()(const C<V, N> &val) {
				auto it = m_subkeys.begin();
				for (auto &elm : val.a) {
					m_node.put<V>(*it++, elm);
				}
			}

			static C<V, N> get(const property_tree::ptree &node, const V defval = 0, const initializer_list<string> subkeys = subkeysXYZW) {
				C<V, N> val = {};
				auto it = subkeys.begin();
				for (auto &e : val.a) {
					e = node.get<V>(*it++, defval);
				}
				return val;
			}

			static C<V, N> get(const property_tree::ptree &node, const string &key, const V defval = 0, const initializer_list<string> subkeys = subkeysXYZW) {
				C<V, N> val = {};
				auto pNode = get_child_optional(node, key);
				assert(pNode);
				return get(*pNode, defval, subkeys);
			}

			ostream &to_string(ostream &os) const {
				auto it = m_subkeys.begin();
				auto val = operator()();
				os << m_key << ": [ ";
				for (auto &e : val.a) {
					os << *it++ << ": " << e << ", ";
				}
				os << "]";
				return os;
			}
		};

		template<typename V, typename Vp>
		static function<void(V &, const property_tree::ptree &)> counter(const string &key, const function<Vp &(V &)> &setter) {
			return [key, setter](V &v, const property_tree::ptree &pt) {
				auto node = pt.get_child_optional(key);
				setter(v) = node ? node->size() : 0;
			};
		}

		template<typename V, typename Vp>
		static function<void(V &, const property_tree::ptree &)> setter(const string &key, const Vp &defval, const function<Vp &(V &)> &setter) {
			return [key, defval, setter](V &v, const property_tree::ptree &pt) {
				setter(v) = pt.get<Vp>(key, defval);
			};
		}

		template<typename V, typename Vp, typename Vp2>
		static function<void(V &, const property_tree::ptree &)> setter(const string &key, const initializer_list<string> &subkey, const Vp &defval, const function<Vp &(V &)> &setter) {
			return [key, subkey, defval, setter](V &v, const property_tree::ptree &pt) {
				Vp r = defval;
				auto it = subkey.begin();
				for (auto &e : r.a) {
					if (it) {
						if (auto d = pt.get_optional<Vp2>(key + *it++)) {
							e = *d;
						}
					}
				}
				setter(v) = r;
			};
		}

		template<typename V, typename Vp, typename Vp2>
		static function<void(V &, const property_tree::ptree &)> inserter(const string &key, const function<back_insert_iterator<Vp>(V &)> &setter, const function<Vp2(const property_tree::ptree &)> &subloader) {
			return [key, setter, subloader](V &v, const property_tree::ptree &pt) {
				if (auto node = pt.get_child_optional(key)) {
					for (auto &sub : *node) {
						setter(v) = subloader(sub.second);
					}
				}
			};
		}
	}

	template<typename V>
	ostream & operator<<(ostream &os, const PTree::Property<V> &rhs) {
		return rhs.to_string(os);
	}
	template<class C>
	ostream &operator<<(ostream &os, const PTree::PropertyA<C> &rhs) {
		return rhs.to_string(os);
	}
	template<class C>
	ostream &operator<<(ostream &os, const PTree::PropertyAA<C> &rhs) {
		return rhs.to_string(os);
	}
	template<template<typename, int> class C, typename V, int N>
	ostream &operator<<(ostream &os, const PTree::PropertyV<C, V, N> &rhs) {
		return rhs.to_string(os);
	}
}
