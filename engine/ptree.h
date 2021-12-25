#pragma once
#include <array>
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
	template<typename T>
	using optional = std::optional<T>;
	using ptree = property_tree::ptree;

	namespace PTree {
		static const initializer_list<string> subkeysXYZW = { "x", "y", "z", "w", };
		static const initializer_list<string> subkeysWH = { "w", "h", };
		static const initializer_list<string> subkeysXYWH = { "x", "y", "w", "h", };
		static const initializer_list<string> subkeysRGBA = { "r", "g", "b", "a", };

		inline ptree &get_child(ptree &node, ptree::path_type key) {
			if (key.empty()) {
				return node;
			}

			auto head = key.reduce();
			auto name = !head.empty() && isdigit(head.front()) ? "" : head;
			auto index = !head.empty() && name.empty() ? std::stol(head) : 0;

			auto matches = node.equal_range(name);
			if (matches.first == matches.second) {
				node.push_back(make_pair(name, ptree()));
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
				node.push_back(make_pair(name, ptree()));
			} while (index-- > 0);
			return get_child(node.back().second, key);
		}

		inline optional<ptree> get_child_optional(ptree &node, ptree::path_type key) {
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

		inline optional<const ptree> get_child_optional(const ptree &node, ptree::path_type key) {
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

		template<typename Vt, typename Vp = vec_traits<Vt>::scalar_type>
		Vt get(const optional<ptree> &pNode, const string &key = "") {
			C<V> ret = {};
			if (pNode) {
				for (auto &ch : pNode->get_child(key)) {
					ret.push_back(ch.second.get<Vp>(""));
				}
			}
			return ret;
		}

		template<typename Vt, typename Vp = vec_traits<Vt>::scalar_type>
		Vt get(const optional<ptree> &pNode, const Vp &defvalue = 0) {
			static const initializer_list<string> keys = { "x", "y", "z", "w", };
			Vt ret = {};
			auto it = keys.begin();
			for (auto &e : ret.a) {
				e = pNode ? pNode->get<Vp>(*it++, defvalue) : defvalue;
			}
			return ret;
		}

		template<typename V>
		class Property {
		private:
			ptree &m_node;
			const string m_key;
			const V m_defval;

		public:
			explicit Property(ptree &node, const string &key, const V &defval = 0)
				: m_node(node), m_key(key), m_defval(defval)
			{
			}

			V get() const {
				return m_node.get<V>(m_key, m_defval);
			}

			void set(const V &val) {
				m_node.put<V>(m_key, val);
			}

			ostream &to_string(ostream &os) const {
				auto val = get();
				os << m_key << ": " << val << "]";
				return os;
			}
		};

		template<class C>
		class PropertyA {
		private:
			ptree &m_node;
			const string m_key;

		public:
			explicit PropertyA(ptree &node, const string &key)
				: m_node(get_child(node, key)), m_key(key)
			{
			}

			C get() const {
				C val = {};
				for (auto &ch : m_node) {
					val.push_back(ch.second.get<typename C::value_type>(""));
				}
				return val;
			}

			void set(const C &val) {
				m_node.clear();
				for (const auto &elm : val) {
					m_node.put("", elm);
				}
			}

			ostream &to_string(ostream &os) const {
				auto it = m_subkeys.begin();
				auto val = get();
				os << m_key << ": [ ";
				for (auto &e : val.a) {
					os << e << ", ";
				}
				os << "]";
				return os;
			}
		};

		template<class C>
		class PropertyAA : public vector<PTree::PropertyA<C> > {
		private:
			ptree &m_node;
			const string m_key;

		public:
			explicit PropertyAA(ptree &node, const string &key)
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

		template<typename Vt, typename Vp = vec_traits<Vt>::scalar_type>
		class PropertyV {
		private:
			ptree &m_node;
			const string m_key;
			const Vp m_defval;
			const initializer_list<string> m_subkeys;

		public:
			explicit PropertyV(ptree &node, const string &key, const Vp defval = 0, const initializer_list<string> subkeys = subkeysXYZW)
				: m_node(get_child(node, key)), m_key(key), m_defval(defval), m_subkeys(subkeys)
			{
			}

			explicit PropertyV(ptree &node, const string &key, const Vt &val, const initializer_list<string> subkeys = subkeysXYZW)
				: m_node(get_child(node, key)), m_key(key), m_defval(0), m_subkeys(subkeys)
			{
				set(val);
			}

			Vt get() const {
				return get(m_node, m_defval, m_subkeys);
			}

			void set(const Vt &val) {
				auto it = m_subkeys.begin();
				for (auto &elm : val.a) {
					m_node.put<Vp>(*it++, elm);
				}
			}

			static Vt get(const ptree &node, const Vp defval = 0, const initializer_list<string> subkeys = subkeysXYZW) {
				Vt val = {};
				auto it = subkeys.begin();
				for (auto &e : val.a) {
					e = node.get<Vp>(*it++, defval);
				}
				return val;
			}

			static Vt get(const ptree &node, const string &key, const Vp defval = 0, const initializer_list<string> subkeys = subkeysXYZW) {
				auto pNode = get_child_optional(node, key);
				assert(pNode);
				return get(*pNode, defval, subkeys);
			}

			ostream &to_string(ostream &os) const {
				auto it = m_subkeys.begin();
				auto val = get();
				os << m_key << ": [ ";
				for (auto &e : val.a) {
					os << *it++ << ": " << e << ", ";
				}
				os << "]";
				return os;
			}
		};

		template<typename V, typename Vp>
		static function<void(V &, const ptree &)> counter(const string &key, const function<Vp &(V &)> &setter) {
			return [key, setter](V &v, const ptree &pt) {
				auto node = pt.get_child_optional(key);
				setter(v) = node ? node->size() : 0;
			};
		}

		template<typename V, typename Vp>
		static function<void(V &, const ptree &)> parse(const string &key, const Vp &defval, const function<Vp &(V &)> &setter) {
			return [key, defval, setter](V &v, const ptree &pt) {
				setter(v) = pt.get<Vp>(key, defval);
			};
		}

		template<typename V, typename Vp>
		static function<void(V &, const ptree &)> parse(const string &key, const initializer_list<string> &subkey, const Vp &defval, const function<Vp &(V &)> &setter) {
			return [key, subkey, defval, setter](V &v, const ptree &pt) {
				Vp r = defval;
				auto it = subkey.begin();
				for (auto &e : r.a) {
					if (it) {
						if (auto d = pt.get_optional<vec_traits<Vp>::scalar_type>(key + "." + *it++)) {
							e = *d;
						}
					}
				}
				setter(v) = r;
			};
		}

		template<typename V, typename Vp>
		static function<void(V &, const ptree &)> inserter(const string &key, const function<back_insert_iterator<Vp>(V &)> &setter, const function<typename Vp::value_type(const ptree &)> &subloader) {
			return [key, setter, subloader](V &v, const ptree &pt) {
				if (auto node = pt.get_child_optional(key)) {
					for (auto &sub : *node) {
						setter(v) = subloader(sub.second);
					}
				}
			};
		}
	}

	template<typename V>
	ostream &operator<<(ostream &os, const PTree::Property<V> &rhs) {
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
	template<typename Vt, typename Vp = vec_traits<Vt>::scalar_type>
	ostream &operator<<(ostream &os, const PTree::PropertyV<Vt, Vp> &rhs) {
		return rhs.to_string(os);
	}
}
