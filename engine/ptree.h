#pragma once
#include <cassert>
#include <cstdint>
#include <functional>
#include <iterator>
#include <boost/property_tree/ptree.hpp>
#include <string>

namespace tte {
	using namespace boost;
	using namespace std;

	class PTree {
	public:
		template<typename V, typename Vp>
		static function<void(V &, const property_tree::ptree &)> counter(const string &key, const function<Vp &(V &)> &setter) {
			return [key, setter](V &v, const property_tree::ptree &pt) {
				auto node = pt.get_child_optional(key);
				setter(v) = node ? node->size() : 0;
			};
		}

		template<typename V, typename Vp>
		static function<void(V &, const property_tree::ptree &)> setter(const string &key, const Vp &defvalue, const function<Vp &(V &)> &setter) {
			return [key, defvalue, setter](V &v, const property_tree::ptree &pt) {
				setter(v) = pt.get<Vp>(key, defvalue);
			};
		}

		template<typename V, typename Vp, typename Vp2>
		static function<void(V &, const property_tree::ptree &)> setter(const string &key, const initializer_list<string> &subkey, const Vp &defvalue, const function<Vp &(V &)> &setter) {
			return [key, subkey, defvalue, setter](V &v, const property_tree::ptree &pt) {
				Vp r = defvalue;
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
	};
}
