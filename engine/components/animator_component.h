#pragma once
#include <actor.h>
#include <asset.h>
#include <asset_handler.h>
#include <cassert>
#include <clist.h>
#include <cstdint>
#include <iostream>
#include <list>
#include <timeline.h>
#include <utility>

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
		property_tree::ptree &m_out;
		list<pair<string, unique_ptr<Timeline> > > m_timelines;

		//
		// public methods
		//
	public:
		explicit Animator(property_tree::ptree &props)
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
}
