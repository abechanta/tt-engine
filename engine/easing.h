#pragma once
#include <cassert>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>

namespace tte {
	using namespace boost;
	using namespace std;

	struct Easing {
		//
		// public definitions
		//
		static constexpr float pi = 3.14159265358979323846f;	// M_PI

		struct Function {
			template<typename Vt>
			static Vt linear(Vt value1, Vt value2, float ratio) {
				return (value2 - value1) * ratio;
			}

			template<typename Vt>
			static Vt quadratic(Vt value1, Vt value2, float ratio) {
				return (value2 - value1) * ratio * ratio;
			}

			template<typename Vt>
			static Vt cubic(Vt value1, Vt value2, float ratio) {
				return (value2 - value1) * ratio * ratio * ratio;
			};

			template<typename Vt>
			static Vt cyclic(Vt value1, Vt value2, float ratio) {
				return (value2 - value1) * (1 - sqrt(1 - ratio * ratio));
			};

			template<typename Vt>
			static Vt sine(Vt value1, Vt value2, float ratio) {
				return (value2 - value1) * (1 - sin((1 - ratio) * pi / 2));
			};

			template<typename Vt>
			static Vt back(Vt value1, Vt value2, float ratio) {
				return (value2 - value1) * ratio * ratio * -cos(ratio * pi);
			};

			template<typename Vt>
			static Vt elastic(Vt value1, Vt value2, float ratio) {
				return (value2 - value1) * ratio * ratio * ratio * cos((5 * ratio - 1) * pi);
			};

			//template<typename Vt>
			//static Vt bounce(Vt value1, Vt value2, float ratio) {
			//};

			//template<typename Vt>
			//static Vt stepping(Vt value1, Vt value2, float ratio) {
			//};
		};

		//
		// public methods
		//
		template<typename Vt>
		static Vt in(Vt value1, Vt value2, float ratio, const function<Vt(Vt, Vt, float)> &interpolate) {
			return value1 + interpolate(value1, value2, ratio);
		}

		template<typename Vt>
		static Vt out(Vt value1, Vt value2, float ratio, const function<Vt(Vt, Vt, float)> &interpolate) {
			return value2 - interpolate(value1, value2, 1 - ratio);
		}

		template<typename Vt>
		static Vt inout(Vt value1, Vt value2, float ratio, const function<Vt(Vt, Vt, float)> &interpolate) {
			if (ratio < 0.5f) {
				return in<Vt>(value1, (value2 - value1) / 2, 2 * ratio, interpolate);
			}
			return out<Vt>((value2 - value1) / 2, value2, 2 * (ratio - 0.5f), interpolate);
		}
	};
}
