#pragma once
#include <easing.h>
#include <ptree.h>
#include <cassert>
#include <climits>
#include <cstdint>
#include <deque>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>

namespace tte {
	using namespace boost;
	using namespace std;

	class Timeline {
		//
		// definitions
		//
	private:
		struct Key {
			typedef Key this_type;
			int32_t frame;
			string easing;
			float value;

			static this_type load(const property_tree::ptree &pt) {
				this_type v;
				PTree::setter<this_type, int32_t>("frame", 0, [](this_type &v) -> auto & { return v.frame; })(v, pt);
				PTree::setter<this_type, string>("easing", "linear:in", [](this_type &v) -> auto & { return v.easing; })(v, pt);
				PTree::setter<this_type, float>("value", 0, [](this_type &v) -> auto & { return v.value; })(v, pt);
				return v;
			}

			template<typename Vt>
			const function<Vt(Vt, Vt, float)> & easingFunc() const {
				static const unordered_map<string, function<Vt(Vt, Vt, Vt)> > func = {
					{ "linear", Easing::Function::linear<Vt>, },
					{ "quadratic", Easing::Function::quadratic<Vt>, },
					{ "cubic", Easing::Function::cubic<Vt>, },
					{ "cyclic", Easing::Function::cyclic<Vt>, },
					{ "sine", Easing::Function::sine<Vt>, },
					{ "elastic", Easing::Function::elastic<Vt>, },
				};
				auto name = easing.substr(0, easing.find_first_of(':'));
				return func.at(name);
			}

			template<typename Vt>
			const function<Vt(Vt, Vt, float, const function<Vt(Vt, Vt, float)> &)> & easingKind() const {
				static const unordered_map<string, function<Vt(Vt, Vt, float, const function<Vt(Vt, Vt, float)> &)> > func = {
					{ ":in", Easing::in<Vt>, },
					{ ":out", Easing::out<Vt>, },
					{ ":inout", Easing::inout<Vt>, },
				};
				auto name = easing.substr(easing.find_first_of(':'));
				return func.at(name);
			}

			template<typename Vt>
			Vt ease(Vt value1, Vt value2, float ratio) const {
				return easingKind<Vt>()(value1, value2, ratio, easingFunc<Vt>());
			}
		};

		struct Channel {
			typedef Channel this_type;
			string access;
			string target;
			string type;
			deque<Key> keys;
			float lastValue;

			static this_type load(const property_tree::ptree &pt) {
				this_type v;
				PTree::setter<this_type, string>("access", "overwrite", [](this_type &v) -> auto & { return v.access; })(v, pt);
				PTree::setter<this_type, string>("target", "empty", [](this_type &v) -> auto & { return v.target; })(v, pt);
				PTree::setter<this_type, string>("type", "float", [](this_type &v) -> auto & { return v.type; })(v, pt);
				PTree::inserter<this_type, deque<Key>, Key>("keys", [](this_type &v) -> back_insert_iterator<deque<Key> > { return back_inserter<deque<Key> >(v.keys); }, Key::load)(v, pt);
				v.lastValue = 0.f;
				return v;
			}
		};

		struct Animation {
			typedef Animation this_type;
			int32_t frameLength;
			int32_t frameDelay;
			size_t repeatCount;
			size_t channelCount;
			deque<Channel> channels;
			int32_t frameBegin;

			static this_type load(const property_tree::ptree &pt) {
				this_type v;
				PTree::setter<this_type, int32_t>("frameLength", 0, [](this_type &v) -> auto & { return v.frameLength; })(v, pt);
				PTree::setter<this_type, int32_t>("frameDelay", 0, [](this_type &v) -> auto & { return v.frameDelay; })(v, pt);
				PTree::setter<this_type, size_t>("repeatCount", 0, [](this_type &v) -> auto & { return v.repeatCount; })(v, pt);
				PTree::counter<this_type, size_t>("channels", [](this_type &v) -> auto & { return v.channelCount; })(v, pt);
				PTree::inserter<this_type, deque<Channel>, Channel>("channels", [](this_type &v) -> back_insert_iterator<deque<Channel> > { return back_inserter<deque<Channel> >(v.channels); }, Channel::load)(v, pt);
				v.frameBegin = INT_MAX;
				return v;
			}
		};

		//
		// member variables
		//
	private:
		bool m_bPlaying;
		Animation m_animation;

		//
		// public methods
		//
	public:
		explicit Timeline(
			const property_tree::ptree &props = property_tree::ptree()
		) : m_bPlaying(false), m_animation(Animation::load(props)) {
		}

		virtual ~Timeline() {
		}

		Timeline & play() {
			m_bPlaying = true;
			return *this;
		}

		Timeline & pause() {
			m_bPlaying = false;
			return *this;
		}

		void tick(int32_t frameSrc, property_tree::ptree &out) {
			if (m_bPlaying) {
				(m_animation.frameBegin == INT_MAX) && (m_animation.frameBegin = frameSrc);
				while (frameSrc - m_animation.frameBegin - m_animation.frameDelay >= m_animation.frameLength) {
					if (m_bPlaying && (m_animation.repeatCount == 0)) {
						m_bPlaying = false;
						out.add<string>("eventPool", "playend");
						break;
					}
					m_animation.repeatCount--;
					m_animation.frameBegin += m_animation.frameLength;
				}
			} else {
				m_animation.frameBegin++;
			}

			auto framePoint = frameSrc - m_animation.frameBegin - m_animation.frameDelay;
			for (auto &channel : m_animation.channels) {
				tick(channel, framePoint, out);
			}
		}

		//
		// property methods
		//
	public:
		bool isPlaying() const {
			return m_bPlaying;
		}

		//
		// others
		//
	private:
		void tick(Channel &channel , int32_t frame, property_tree::ptree &out) {
			auto value = getValue(channel, frame);
			auto func = (channel.access.compare("delta") == 0) ? delta : overwrite;
			func(out, channel, value);
			channel.lastValue = value;
		}

		float getValue(Channel &channel, int32_t frame) {
			int32_t keyframeBegin = 0;
			float valueBegin = 0.f;
			(frame < 0) && (frame = 0);
			for (auto &key : channel.keys) {
				if (frame < key.frame) {
					float ratio = static_cast<float>(frame - keyframeBegin) / (key.frame - keyframeBegin);
					return key.ease<float>(valueBegin, key.value, ratio);	// TODO: cast as specified.
				}
				keyframeBegin = key.frame;
				valueBegin = key.value;
			}
			return valueBegin;
		}

		static void delta(property_tree::ptree &out, Channel &channel, float value) {
			value = out.get<float>(channel.target, value) - channel.lastValue + value;
			out.put<float>(channel.target, value);
		}

		static void overwrite(property_tree::ptree &out, Channel &channel, float value) {
			out.put<float>(channel.target, value);
		}
	};
}
