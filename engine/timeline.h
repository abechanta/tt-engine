#pragma once
#include <cassert>
#include <climits>
#include <cstdint>
#include <deque>
#include <easing.h>
#include <iostream>
#include <iterator>
#include <ptree.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace tte {
	using namespace boost;
	using namespace std;

	struct Animation {
		//
		// definitions
		//
		typedef Animation this_type;

		struct Key {
			typedef Key this_type;
			int32_t frame;
			string easing;
			float value;

			static this_type parse(const property_tree::ptree &pt) {
				this_type v;
				PTree::parse<this_type, int32_t>("frame", 0, [](this_type &v) -> auto & { return v.frame; })(v, pt);
				PTree::parse<this_type, string>("easing", "linear:in", [](this_type &v) -> auto & { return v.easing; })(v, pt);
				PTree::parse<this_type, float>("value", 0, [](this_type &v) -> auto & { return v.value; })(v, pt);
				return v;
			}

			template<typename Vt>
			const function<Vt(Vt, Vt, float)> &easingFunc() const {
				static const unordered_map<string, function<Vt(Vt, Vt, Vt)> > func = {
					{ "linear", Easing::Function::linear<Vt>, },
					{ "quadratic", Easing::Function::quadratic<Vt>, },
					{ "cubic", Easing::Function::cubic<Vt>, },
					{ "cyclic", Easing::Function::cyclic<Vt>, },
					{ "sine", Easing::Function::sine<Vt>, },
					{ "back", Easing::Function::back<Vt>, },
					{ "elastic", Easing::Function::elastic<Vt>, },
					{ "stepping", Easing::Function::stepping<Vt>, },
				};
				auto name = easing.substr(0, easing.find_first_of(':'));
				return func.at(name);
			}

			template<typename Vt>
			const function<Vt(Vt, Vt, float, const function<Vt(Vt, Vt, float)> &)> &easingKind() const {
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

			static this_type parse(const property_tree::ptree &pt) {
				this_type v;
				PTree::parse<this_type, string>("access", "overwrite", [](this_type &v) -> auto & { return v.access; })(v, pt);
				PTree::parse<this_type, string>("target", "empty", [](this_type &v) -> auto & { return v.target; })(v, pt);
				PTree::parse<this_type, string>("type", "float", [](this_type &v) -> auto & { return v.type; })(v, pt);
				PTree::inserter<this_type, deque<Key>, Key>("keys", [](this_type &v) -> back_insert_iterator<deque<Key> > { return back_inserter<deque<Key> >(v.keys); }, Key::parse)(v, pt);
				return v;
			}
		};

		//
		// member variables
		//
		string name;
		int32_t frameLength;
		int32_t frameDelay;
		size_t repeatCount;
		size_t channelCount;
		deque<Channel> channels;

		//
		// public methods
		//
		static this_type parse(const property_tree::ptree &pt) {
			this_type v;
			PTree::parse<this_type, string>("name", "", [](this_type &v) -> auto & { return v.name; })(v, pt);
			PTree::parse<this_type, int32_t>("frameLength", 0, [](this_type &v) -> auto & { return v.frameLength; })(v, pt);
			PTree::parse<this_type, int32_t>("frameDelay", 0, [](this_type &v) -> auto & { return v.frameDelay; })(v, pt);
			PTree::parse<this_type, size_t>("repeatCount", 0, [](this_type &v) -> auto & { return v.repeatCount; })(v, pt);
			PTree::counter<this_type, size_t>("channels", [](this_type &v) -> auto & { return v.channelCount; })(v, pt);
			PTree::inserter<this_type, deque<Channel>, Channel>("channels", [](this_type &v) -> back_insert_iterator<deque<Channel> > { return back_inserter<deque<Channel> >(v.channels); }, Channel::parse)(v, pt);
			return v;
		}
	};

	class AnimationSet {
		//
		// definitions
		//
	private:
		typedef AnimationSet this_type;

		//
		// member variables
		//
	private:
		deque<Animation> m_animations;
		static const Animation s_noAnimation;

		//
		// public methods
		//
	public:
		AnimationSet(const property_tree::ptree &pt)
			: m_animations()
		{
			PTree::inserter<this_type, deque<Animation>, Animation>("animation", [](this_type &v) -> back_insert_iterator<deque<Animation> > { return back_inserter<deque<Animation> >(v.m_animations); }, Animation::parse)(*this, pt);
		}

		const Animation &get(const string &animname) const {
			auto pAnimation = find_if(m_animations.begin(), m_animations.end(), [&animname](auto &anim) -> bool {
				return anim.name == animname;
			});
			return (pAnimation == m_animations.end()) ? s_noAnimation : *pAnimation;
		}
	};

	class Timeline {
		//
		// definitions
		//

		//
		// member variables
		//
	private:
		property_tree::ptree &m_out;
		const Animation &m_animation;
		bool m_bPlaying;
		bool m_bDone;
		int32_t m_frameBegin;
		int32_t m_repeatLeft;
		vector<float> m_lastValues;

		//
		// public methods
		//
	public:
		explicit Timeline(property_tree::ptree &out, const Animation &animation, bool bImmediate = true)
			: m_out(out), m_animation(animation), m_bPlaying(false), m_bDone(false), m_frameBegin(INT_MAX), m_repeatLeft(0)
		{
			m_lastValues.resize(m_animation.channelCount, 0.f);
			if (bImmediate) {
				replay();
			}
		}

		virtual ~Timeline() {
		}

		Timeline &replay() {
			m_bPlaying = true;
			return *this;
		}

		Timeline &pause() {
			m_bPlaying = false;
			return *this;
		}

		bool tick(int32_t frameSrc) {
			if (m_bPlaying) {
				if (m_frameBegin == INT_MAX) {
					m_frameBegin = frameSrc;
					m_repeatLeft = m_animation.repeatCount;
				}
				while (frameSrc - m_frameBegin - m_animation.frameDelay >= m_animation.frameLength) {
					if (m_bPlaying && (m_repeatLeft == 0)) {
						m_bPlaying = false;
						m_bDone = true;
						break;
					}
					m_repeatLeft--;
					m_frameBegin += m_animation.frameLength;
				}
			} else {
				m_frameBegin++;
			}

			auto framePoint = frameSrc - m_frameBegin - m_animation.frameDelay;
			auto pLastValue = m_lastValues.begin();
			for (auto &channel : m_animation.channels) {
				tick(channel, framePoint, *pLastValue++);
			}

			return m_bDone;
		}

		//
		// property methods
		//
	public:
		bool isPlaying() const {
			return m_bPlaying;
		}

		bool isDone() const {
			return m_bDone;
		}

		//
		// others
		//
	private:
		void tick(const Animation::Channel &channel, int32_t frame, float &lastValue) {
			auto value = getValue(channel, frame);
			auto func = (channel.access.compare("delta") == 0) ? delta : overwrite;
			func(m_out, channel, value, lastValue);
			lastValue = value;
		}

		float getValue(const Animation::Channel &channel, int32_t frame) {
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

		static void delta(property_tree::ptree &out, const Animation::Channel &channel, float value, float lastValue) {
			value = out.get<float>(channel.target, value) - lastValue + value;
			out.put<float>(channel.target, value);
		}

		static void overwrite(property_tree::ptree &out, const Animation::Channel &channel, float value, float) {
			out.put<float>(channel.target, value);
		}
	};

#if defined(tte_declare_static_variables)
	const Animation AnimationSet::s_noAnimation = Animation();
#endif	// defined(tte_declare_static_variables)
}
