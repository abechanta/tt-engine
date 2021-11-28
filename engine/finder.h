#pragma once
#include <actor.h>
#include <cassert>
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

namespace tte {
	using namespace std;

	template<typename V>
	class Finder {
		//
		// public definitions
		//
	public:
		class Indexer {
			friend class Finder;

			//
			// member variables
			//
		private:
			string m_key;

		private:
			Indexer(
				const string &key, V &value
			) : m_key(key) {
				add(m_key, value);
			}

			//
			// public methods
			//
		public:
			Indexer(
				Indexer &&rhs
			) : m_key(rhs.m_key) {
				rhs.m_key = "";
			}

			~Indexer() {
				if (!m_key.empty()) {
					del(m_key);
				}
			}
		};

		//
		// member variables
		//
	private:
		static unordered_map<string, V *> s_registory;

		//
		// public methods
		//
	public:
		static void find(const string &key, const function<void(V &)> &operation) {
			if (auto it = s_registory[key]) {
				operation(*it);
			}
		}

		template<typename Vc1>
		static void find(const string &key, const function<void(Vc1 &)> &operation) {
			if (auto it = s_registory[key]) {
				it->getComponent<Vc1>([&operation](Vc1 &component1) {
					operation(component1);
				});
			}
		}

		template<typename Vc1, typename Vc2>
		static void find(const string &key, const function<void(Vc1 &, Vc2 &)> &operation) {
			if (auto it = s_registory[key]) {
				it->getComponent<Vc1>([it, &operation](Vc1 &component1) {
					it->getComponent<Vc2>([&operation, &component1](Vc2 &component2) {
						operation(component1, component2);
					});
				});
			}
		}

		template<typename Vt>
		static Vt find(const string &key, Vt &&defvalue, const function<Vt(V &)> &operation) {
			auto it = s_registory[key];
			return it ? operation(*it) : defvalue;
		}

		static Indexer create(const string &key, V &value) {
			return Indexer(key, value);
		}

		//
		// utility methods
		//
	private:
		static void add(const string &key, V &value) {
			s_registory[key] = &value;
		}

		static void del(const string &key) {
			s_registory.erase(key);
		}

		//
		// others
		//
	private:
		Finder() {}
	};

#if defined(tte_declare_static_variables)
	unordered_map<string, Actor *> Finder<Actor>::s_registory = {};
#endif	// defined(tte_declare_static_variables)
}
