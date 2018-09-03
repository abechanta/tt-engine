#pragma once
#include <actor.h>
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <string>

namespace tte {
	using namespace std;

	struct Col4 {
		uint8_t r, g, b, a;

		Col4(
			const initializer_list<uint8_t> &col4 = { 0, 0, 0, 0, }
		) : r(0), g(0), b(0), a(0) {
			auto it = col4.begin();
			r = *it++;
			g = *it++;
			b = *it++;
			a = *it++;
		}

		Col4(
			const Actor &actor, const string &key
		) : r(0), g(0), b(0), a(0) {
			r = actor.get<uint8_t>(key + ".r", 0);
			g = actor.get<uint8_t>(key + ".g", 0);
			b = actor.get<uint8_t>(key + ".b", 0);
			a = actor.get<uint8_t>(key + ".a", 255);
		}

		Col4(
			uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_
		) : r(r_), g(g_), b(b_), a(a_) {
		}

		Col4(
			uint32_t code
		) : r(decode(code, 24)), g(decode(code, 16)), b(decode(code, 8)), a(decode(code, 0)) {
		}

		uint32_t encode_r8g8b8a8() const {
			return (r << 24) | (g << 16) | (b << 8) | (a << 0);
		}

		uint32_t encode_a8b8g8r8() const {
			return (a << 24) | (b << 16) | (g << 8) | (r << 0);
		}

		uint32_t encode_a8r8g8b8() const {
			return (a << 24) | (r << 16) | (g << 8) | (b << 0);
		}

		uint32_t encode_b8g8r8a8() const {
			return (b << 24) | (g << 16) | (r << 8) | (a << 0);
		}

		static uint8_t decode(uint32_t code, uint32_t shift) {
			return (code >> shift) & 0xff;
		}
	};
}
