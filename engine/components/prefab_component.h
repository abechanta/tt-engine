#pragma once
#include <actor.h>
#include <actor_modifiers.h>
#include <asset.h>
#include <cassert>
#include <clist.h>
#include <cstdint>
#include <finder.h>

namespace tte {
	class Prefab : public CList {
		//
		// public definitions
		//
	public:
		static constexpr uint32_t tag = makeTag("PFAB");

		//
		// member variables
		//
	private:
		Asset &m_assetBase;
		Actor &m_actor;
		property_tree::ptree &m_node;

		//
		// public methods
		//
	public:
		explicit Prefab(Asset &assetBase, Actor &a, property_tree::ptree &node)
			: CList(tag), m_assetBase(assetBase), m_actor(a), m_node(node)
		{
			loadResources();
			createActors();
		}

		virtual ~Prefab() override {
		}

		static Actor::Action append(Asset &assetBase) {
			return [&assetBase](Actor &a) {
				a.appendComponent(new Prefab(assetBase, a, a.props("prefab")));
			};
		}

		void loadResources() {
			for (auto &resource : m_node.get_child("resources")) {
				auto parentName = resource.second.get<string>("parent");
				Finder<Actor>::find(parentName, [this, &resource](Actor &parent) {
					parent.appendChild(new Actor(
						Actor::noAction,
						loadProps(resource.second) + appendComponents(m_assetBase)
					));
				});
			}
		}

		void createActors() {
			for (auto &actor : m_node.get_child("actors")) {
				auto name = actor.second.get<string>("name", "");
				auto props = actor.second.get<string>("property", "");
				auto parentName = actor.second.get<string>("parent", "");
				auto createAndAppendChild = [this, &name, &props](Actor &parent) {
					parent.appendChild(new Actor(
						Actor::noAction,
						loadProps(m_assetBase.find(props)) + appendComponents(m_assetBase) + put<string>("_.name", name)
					));
				};

				if (parentName.empty()) {
					createAndAppendChild(m_actor);
				} else {
					Finder<Actor>::find(parentName, createAndAppendChild);
				}
			}
		}

		void unload() {
			Finder<Actor>::find("sys:root", [this](Actor &a) {
				for (auto &resource : m_node.get_child("resources")) {
					auto resourceName = resource.second.get<string>("indexer.name", "<noname>");
					a.appendAction(findThen(resourceName, exit()));
				}
				a.appendAction([this](Actor &) {
					delete &m_actor;
				});
			});
		}
	};
}
