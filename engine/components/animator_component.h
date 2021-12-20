#pragma once
#include <actor.h>
#include <asset.h>
#include <asset_handler.h>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <climits>
#include <clist.h>
#include <cmath>
#include <cstdint>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <ptree.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace tte {
	using namespace boost;
	using namespace std;
	using ptree = property_tree::ptree;

	namespace Easing {
		//
		// public definitions
		//
		constexpr float pi = 3.14159265358979323846f;	// M_PI

		namespace Function {
			template<typename Vt>
			Vt linear(Vt value1, Vt value2, float ratio) {
				return (value2 - value1) * ratio;
			}

			template<typename Vt>
			Vt quadratic(Vt value1, Vt value2, float ratio) {
				return (value2 - value1) * ratio * ratio;
			}

			template<typename Vt>
			Vt cubic(Vt value1, Vt value2, float ratio) {
				return (value2 - value1) * ratio * ratio * ratio;
			};

			template<typename Vt>
			Vt cyclic(Vt value1, Vt value2, float ratio) {
				return (value2 - value1) * (1 - sqrt(1 - ratio * ratio));
			};

			template<typename Vt>
			Vt sine(Vt value1, Vt value2, float ratio) {
				return (value2 - value1) * (1 - sin((1 - ratio) * pi / 2));
			};

			template<typename Vt>
			Vt back(Vt value1, Vt value2, float ratio) {
				return (value2 - value1) * ratio * ratio * -cos(ratio * pi);
			};

			template<typename Vt>
			Vt elastic(Vt value1, Vt value2, float ratio) {
				return (value2 - value1) * ratio * ratio * ratio * cos((5 * ratio - 1) * pi);
			};

			template<typename Vt>
			Vt bounce(Vt value1, Vt value2, float ratio) {
				Vt ratio0 = ratio * ratio * ratio * cos((3 * ratio - 1) * pi);
				return (value2 - value1) * (ratio0 < 0.f ? -ratio0 : ratio0);
			};

			template<typename Vt>
			Vt stepping(Vt value1, Vt value2, float ratio) {
				return (ratio < 1.f) ? 0.f : value2 - value1;
			};
		}

		//
		// public methods
		//
		template<typename Vt>
		Vt in(Vt value1, Vt value2, float ratio, const function<Vt(Vt, Vt, float)> &interpolate) {
			return value1 + interpolate(value1, value2, ratio);
		}

		template<typename Vt>
		Vt out(Vt value1, Vt value2, float ratio, const function<Vt(Vt, Vt, float)> &interpolate) {
			return value2 - interpolate(value1, value2, 1 - ratio);
		}

		template<typename Vt>
		Vt inout(Vt value1, Vt value2, float ratio, const function<Vt(Vt, Vt, float)> &interpolate) {
			if (ratio < 0.5f) {
				return in<Vt>(value1, (value2 - value1) / 2, 2 * ratio, interpolate);
			}
			return out<Vt>((value2 - value1) / 2, value2, 2 * (ratio - 0.5f), interpolate);
		}
	}

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

			static this_type parse(const ptree &pt) {
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
					{ "bounce", Easing::Function::bounce<Vt>, },
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

			static this_type parse(const ptree &pt) {
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
		int32_t repeatCount;
		size_t channelCount;
		deque<Channel> channels;

		//
		// public methods
		//
		static this_type parse(const ptree &pt) {
			this_type v;
			PTree::parse<this_type, string>("name", "", [](this_type &v) -> auto & { return v.name; })(v, pt);
			PTree::parse<this_type, int32_t>("frameLength", 0, [](this_type &v) -> auto & { return v.frameLength; })(v, pt);
			PTree::parse<this_type, int32_t>("frameDelay", 0, [](this_type &v) -> auto & { return v.frameDelay; })(v, pt);
			PTree::parse<this_type, int32_t>("repeatCount", 0, [](this_type &v) -> auto & { return v.repeatCount; })(v, pt);
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
		AnimationSet(const ptree &pt)
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

		static void typeAnim(Asset &a) {
			cout << __FUNCTION__ << ": " << a.path() << endl;
			assert(filesystem::is_regular_file(a.path()));
			a.setLoader([](Asset &a, bool bLoad) -> bool {
				if (bLoad) {
					ptree pt;
					read_json(ifstream(a.path()), pt);
					a.handle<AnimationSet>().reset(new AnimationSet(pt));
				} else {
					a.handle<AnimationSet>().reset();
				}
				return true;
			});
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
		ptree &m_out;
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
		explicit Timeline(ptree &out, const Animation &animation, bool bImmediate = true)
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
					if (m_repeatLeft == 0) {
						m_frameBegin = frameSrc - m_animation.frameDelay - m_animation.frameLength;
						m_bPlaying = false;
						m_bDone = true;
						break;
					} else if (m_repeatLeft > 0) {
						m_frameBegin += m_animation.frameLength;
						m_repeatLeft--;
					}
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

		static void delta(ptree &out, const Animation::Channel &channel, float value, float lastValue) {
			value = out.get<float>(channel.target, value) - lastValue + value;
			out.put<float>(channel.target, value);
		}

		static void overwrite(ptree &out, const Animation::Channel &channel, float value, float) {
			out.put<float>(channel.target, value);
		}
	};

	class Animator : public CList {
		//
		// public definitions
		//
	public:
		static constexpr uint32_t tag = makeTag("ANIM");

		//
		// member variables
		//
	private:
		ptree &m_out;
		list<pair<string, unique_ptr<Timeline> > > m_timelines;

		//
		// public methods
		//
	public:
		explicit Animator(ptree &props)
			: CList(tag), m_out(props), m_timelines()
		{
		}

		virtual ~Animator() override {
		}

		static Actor::Action append() {
			return [](Actor &a) {
				a.appendComponent(new Animator(a.props()));
				a.appendAction([](Actor &a) {
					a.getComponent<Animator>([&a](auto &animator) {
						animator.tick(a);
					});
				});
			};
		}

		void replay(Asset &asset, const string &animname, const string &slotname = "") {
			auto name = slotname.empty() ? animname : slotname;
			m_timelines.remove_if([name](auto &t) -> bool {
				return t.first == name;
			});
			m_timelines.push_back(pair<string, unique_ptr<Timeline> >{ name, new Timeline(m_out, asset.handle<AnimationSet>()->get(animname)), });
		}

		void tick(Actor &a) {
			auto ticks = a.get<int32_t>("_.ticks", 0);
			m_timelines.remove_if([ticks](auto &t) -> bool {
				return t.second->tick(ticks);
			});
		}
	};

#if defined(tte_declare_static_variables)
	const Animation AnimationSet::s_noAnimation = Animation();
#endif	// defined(tte_declare_static_variables)
}
