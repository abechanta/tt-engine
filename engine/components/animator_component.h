#pragma once
#include <actor.h>
#include <asset.h>
#include <cassert>
#include <clist.h>
#include <cstdint>
#include <iostream>
#include <timeline.h>

namespace tte {
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
		property_tree::ptree m_params;
		Timeline m_timeline;

		//
		// public methods
		//
	public:
		explicit Animator(const property_tree::ptree &props)
			: CList(tag), m_params(props), m_timeline()
		{
		}

		virtual ~Animator() override {
		}

		static Actor::Action append(Asset &asset) {
			return [&asset](Actor &a) {
				a.appendComponent(new Animator(asset.props("animation")));
				a.appendAction([](Actor &a) {
					a.getComponent<Animator>([&a](auto &animator) {
						animator.tick(a);
					});
				});
			};
		}

		void play(const string &animname) {
			m_timeline = Timeline(m_params.get_child(animname)).play();
			cout << "play" << endl;
		}

		void tick(Actor &a) {
			auto count0 = a.get<int32_t>("count0", 0);
			m_timeline.tick(count0, a.props());
		}
	};
}
