#pragma once
#include <cassert>
#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <boost/property_tree/ptree.hpp>
#include <boost/qvm/all.hpp>
#include <string>

namespace tte {
	using namespace std;
	using namespace boost::qvm;

	using quaternion = boost::qvm::quat<float>;
	using vector2 = boost::qvm::vec<float, 2>;
	using vector3 = boost::qvm::vec<float, 3>;
	using vector4 = boost::qvm::vec<float, 4>;
	using matrix3x4 = boost::qvm::mat<float, 3, 4>;

	struct Geometry {
		static constexpr float pi = 3.14159265358979323846f;	// M_PI
		static constexpr float deg2rad = pi / 180.f;
		static constexpr float fixed2rad = pi / 32768.f;
		static constexpr float rad2deg = 180.f / pi;
		static constexpr float rad2fixed = 32768.f / pi;
		typedef float Radian;
		typedef float Degree;
		typedef int32_t Fixed32;
		//using vector3r = boost::qvm::vec<Radian, 3>;	// TODO

		template<typename V>
		static V get(const initializer_list<float> &vec = {}, float defvalue = 0.f) {
			V ret;
			auto it = vec.begin();
			for (auto &e : ret.a) {
				e = it ? *it++ : defvalue;
			}
			return ret;
		}

		template<template<typename, int> class C, typename V, int N>
		static C<V, N> get(const property_tree::ptree &node, const string &key = "", V defvalue = 0) {
			static const initializer_list<string> keys = { ".x", ".y", ".z", ".w", };
			C<V, N> ret;
			auto it = keys.begin();
			for (auto &e : ret.a) {
				e = node.get<V>(key + *it++, defvalue);
			}
			return ret;
		}

		template<template<typename> class C, typename V>
		static C<V> get(const property_tree::ptree &node, const string &key = "") {
			C<V> ret;
			for (auto &ch : node.get_child(key)) {
				ret.push_back(ch.second.get<V>(""));
			}
			return ret;
		}

		static matrix3x4& identity(matrix3x4 &m) {
			m = del_row<3>(identity_mat<float, 4>());
			return m;
		}

		static matrix3x4 & trs2d(matrix3x4 &m, const vector3 &translation, const vector3 &rotation, const vector3 &scaling) {
			{	// translation
				vector3 t = del_col<3>(m) * translation;
				col<3>(m) = col<3>(m) + t;
			}
			{	// rotation
				mat<float, 3, 3> mr = del_col<3>(m);
				rotate_z<mat<float, 3, 3>, float>(mr, Z(rotation));
				del_col<3>(m) = mr;
			}
			{	// scaling
				col<0>(m) *= X(scaling);
				col<1>(m) *= Y(scaling);
				col<2>(m) *= Z(scaling);
			}
			return m;
		}

		static vector3 pos(const matrix3x4 &m, const vector3 &pos) {
			vec<float, 4> p = XYZ1(pos);
			return m * p;
		}

		static float angZ(const matrix3x4 &m) {
			return atan2(A10(m), A00(m));
		}
	};
}
