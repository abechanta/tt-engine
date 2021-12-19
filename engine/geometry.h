#pragma once
#include <cassert>
#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <boost/qvm/all.hpp>
#include <string>
#include <unordered_map>

namespace tte {
	using namespace std;
	using namespace boost::qvm;

	using quaternion = boost::qvm::quat<float>;
	using vector2 = boost::qvm::vec<float, 2>;
	using vector3 = boost::qvm::vec<float, 3>;
	using vector4 = boost::qvm::vec<float, 4>;
	using matrix3x4 = boost::qvm::mat<float, 3, 4>;
	using vector2i = boost::qvm::vec<int32_t, 2>;
	using vector3i = boost::qvm::vec<int32_t, 3>;
	using vector4i = boost::qvm::vec<int32_t, 4>;

	namespace Geometry {
		constexpr float pi = 3.14159265358979323846f;	// M_PI
		constexpr float deg2rad = pi / 180.f;
		constexpr float fixed2rad = pi / 32768.f;
		constexpr float rad2deg = 180.f / pi;
		constexpr float rad2fixed = 32768.f / pi;
		typedef float Radian;
		typedef float Degree;
		typedef int32_t Fixed32;
		//using vector3r = boost::qvm::vec<Radian, 3>;	// TODO

		inline int32_t modulo(int32_t a, int32_t b) {
			return ((a %= b) < 0) ? b + a : a;
		}

		static const unordered_map<string, float> rotationConv = {
			{ "radian", 1.f, },
			{ "degree", Geometry::deg2rad, },
			{ "fixed32", Geometry::fixed2rad, },
		};

		static const unordered_map<string, float> alignmentConv = {
			{ "begin", 0.f, },
			{ "center", 0.5f, },
			{ "end", 1.f, },
		};

		template<typename V>
		V get(const initializer_list<float> &vec = {}, const float &defvalue = 0.f) {
			V ret = {};
			auto it = vec.begin();
			for (auto &e : ret.a) {
				e = it ? *it++ : defvalue;
			}
			return ret;
		}

		template<typename V>
		V mul_elements(const V &lhs, const V &rhs) {
			V ret = {};
			for (auto i = 0; i < vec_traits<V>::dim; i++) {
				vec_traits<V>::write_element_idx(i, ret) = vec_traits<V>::read_element_idx(i, lhs) * vec_traits<V>::read_element_idx(i, rhs);
			}
			return ret;
		}

		template<typename V>
		V div_elements(const V &lhs, const V &rhs) {
			V ret = {};
			for (auto i = 0; i < vec_traits<V>::dim; i++) {
				// assert(vec_traits<V>::read_element_idx(i, rhs) != 0);
				vec_traits<V>::write_element_idx(i, ret) = vec_traits<V>::read_element_idx(i, lhs) / vec_traits<V>::read_element_idx(i, rhs);
			}
			return ret;
		}

		template<typename Vt, typename Vp = vec_traits<Vt>::scalar_type>
		Vt conv_elements(const unordered_map<string, Vp> &mapping, const initializer_list<string> &vec) {
			Vt ret = {};
			auto it = vec.begin();
			for (auto i = 0; i < vec_traits<Vt>::dim; i++) {
				vec_traits<Vt>::write_element_idx(i, ret) = mapping.at(*it++);
			}
			return ret;
		}

		template<typename Vt, typename Vp = vec_traits<Vt>::scalar_type, int N = vec_traits<Vt>::dim>
		Vt conv_elements(const unordered_map<string, Vp> &mapping, const array<string, N> &vec) {
			Vt ret = {};
			auto it = vec.begin();
			for (auto i = 0; i < N; i++) {
				vec_traits<Vt>::write_element_idx(i, ret) = mapping.at(*it++);
			}
			return ret;
		}

		inline matrix3x4 &identity(matrix3x4 &m) {
			m = del_row<3>(identity_mat<float, 4>());
			return m;
		}

		inline matrix3x4 &trs2d(matrix3x4 &m, const vector3 &translation, const vector3 &rotation, const vector3 &scaling) {
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

		inline vector3 pos(const matrix3x4 &m, const vector3 &pos) {
			vec<float, 4> p = XYZ1(pos);
			return m * p;
		}

		inline float angZ(const matrix3x4 &m) {
			return atan2(A10(m), A00(m));
		}
	};
}
