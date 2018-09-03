#pragma once
#include <actor.h>
#include <clist.h>
#include <finder.h>
#include <cassert>
#include <cstdint>
#include <string>
#include <utility>

namespace tte {
	class Indexer : public CList {
		//
		// public definitions
		//
	public:
		static constexpr uint32_t tag = makeTag("INDX");

		//
		// member variables
		//
	private:
		Finder<Actor>::Indexer m_indexer;

		//
		// public methods
		//
	public:
		Indexer(
			const string &name,
			Actor &a
		) : CList(tag), m_indexer(std::move(Finder<Actor>::create(name, a))) {
		}

		virtual ~Indexer() override {
		}

		static Actor::Action append(const string &name) {
			return [&name](Actor &a) {
				a.appendComponent(new Indexer(name, a));
			};
		}
	};
}
