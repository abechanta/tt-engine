#pragma once
#include <mtree.h>
#include <cassert>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <string>

namespace tte {
	using namespace boost;
	using namespace std;
	namespace fs = experimental::filesystem::v1;

	template<typename Dx>
	struct empty_delete : public std::default_delete<Dx> {
		void operator()(Dx *) {
		}
	};

	class Asset : public MTree<Asset> {
		//
		// public definitions
		//
	public:
		typedef fs::path Path;

		//
		// member variables
		//
	private:
		Path m_path;
		property_tree::ptree m_props;
		std::unique_ptr<uint32_t, empty_delete<uint32_t> > m_handle;
		function<bool(Asset &, bool)> m_loader;
		uint32_t m_loaded;

		//
		// public methods
		//
	public:
		Asset(
			const Path &path,
			const function<void(Asset &)> &initializer
		) : MTree(), m_path(path), m_props(), m_handle(), m_loader(noLoader), m_loaded(0) {
			initializer(*this);
		}

		virtual ~Asset() override {
			cout << "Asset::dtor" << endl;
			assert(m_loaded == 0);
			m_props.clear();
		}

		void idle() {
			cout << "Asset::idle: " << m_path << endl;
		}

		bool load() {
			if (m_loaded == 0) {
				if (!m_loader(self(), true)) {
					return false;
				}
				cout << "Asset::load: " << m_path << endl;
			}
			m_loaded++;
			return true;
		}

		bool unload() {
			if (m_loaded == 1) {
				if (!m_loader(self(), false)) {
					return false;
				}
				cout << "Asset::unload: " << m_path << endl;
			}
			(m_loaded > 0) && m_loaded--;
			return true;
		}

		//
		// property methods
		//
	public:
		const Path & path() const {
			return m_path;
		}

		property_tree::ptree & props() {
			return m_props;
		}

		const property_tree::ptree & props() const {
			return m_props;
		}

		template<typename V>
		std::unique_ptr<V> & handle() {
			return *reinterpret_cast<std::unique_ptr<V> *>(&m_handle);
		}

		template<typename V>
		const std::unique_ptr<V> & handle() const {
			return *reinterpret_cast<const std::unique_ptr<V> *>(&m_handle);
		}

		void setLoader(const function<bool(Asset &, bool)> &loader) {
			m_loader = loader;
		}

		//
		// utility methods
		//
	public:
		Asset & find(const Path &path) {
			return path.empty() ? *this : find(path.parent_path()).findChild(path.filename());
		}

		Asset & findChild(const Path &path) {
			auto it = find_if(begin_children(), end_children(), [&pathCurrent = m_path, &path](Asset &a) {
				return (a.path().compare(pathCurrent / path) == 0);
			});
			return (it == end_children()) ? noAsset() : *it;
		}

		//
		// utility operators
		//
	public:
		static bool noLoader(Asset &, bool) {
			return false;
		}

		static Asset & noAsset() {
			static Asset s_assetDummy(L"<undef>", [](Asset &) {});
			return s_assetDummy;
		}

	private:
		Asset & self() {
			return *this;
		}
	};
}

namespace std {
	using namespace tte;

	template<>
	struct hash<Asset::Path> {
		size_t operator()(const Asset::Path &_Keyval) const {
			return (_Hash_array_representation(_Keyval.c_str(), _Keyval.string().size()));
		}
	};
}
