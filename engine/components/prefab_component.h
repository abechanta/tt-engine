#pragma once
#include <actor.h>
#include <asset.h>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <clist.h>
#include <components/animator_component.h>
#include <components/indexer_component.h>
#include <components/input_component.h>
#include <components/material_component.h>
#include <components/prefab_component.h>
// #include <components/renderer2d_component.h>
#include <components/resource_component.h>
#include <components/shape_component.h>
#include <components/transform_component.h>
#include <cstdint>
#include <finder.h>
#include <string>

namespace tte {
	using namespace boost;
	using namespace std;
	using ptree = property_tree::ptree;

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
		ptree &m_node;

		//
		// public methods
		//
	public:
		explicit Prefab(Asset &assetBase, Actor &a, ptree &node)
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

		static Actor &appendComponents(Actor &a, Asset &assetBase) {
			for (auto &component : a.props("components")) {
				auto componentType = component.second.get<string>("", "");
				if (componentType == "animator") {
					Animator::append()(a);
				}
				if (componentType == "indexer") {
					Indexer::append()(a);
				}
				if (componentType == "input") {
					Input::append()(a);
				}
				if (componentType == "material") {
					Material::append(assetBase, true)(a);
				}
				if (componentType == "prefab") {
					Prefab::append(assetBase)(a);
				}
				if (componentType == "resource") {
					Resource::append(assetBase, true)(a);
				}
				if (componentType == "sprite") {
					Shape::append<ShapeSprite>()(a);
				}
				if (componentType == "tilemap") {
					Shape::append<ShapeTilemap>()(a);
				}
				if (componentType == "text") {
					Shape::append<ShapeText>()(a);
				}
				if (componentType == "transform") {
					Transform::append()(a);
				}
			}
			return a;
		}

		void loadResources() {
			for (auto &resource : m_node.get_child("resources")) {
				auto name = resource.second.get<string>("name", "");
				auto propsName = resource.second.get<string>("property");
				auto parentName = resource.second.get<string>("parent");
				auto appendChildTo = [this, name, propsName](Actor &p) {
					p.appendChild(new Actor(
						Actor::noAction,
						[this, name, propsName](Actor &a) {
							auto &asset = m_assetBase.find(propsName);
							asset.load();
							a.importProps(asset.props());
							asset.unload();
							if (!name.empty()) {
								a.props().put<string>("_.name", name);
							}
							appendComponents(a, m_assetBase);
						}
					));
				};

				Finder<Actor>::find(parentName, appendChildTo);
			}
		}

		void createActors() {
			for (auto &actor : m_node.get_child("actors")) {
				auto name = actor.second.get<string>("name", "");
				auto propsName = actor.second.get<string>("property", "");
				auto parentName = actor.second.get<string>("parent", "");
				auto appendChildTo = [this, name, propsName](Actor &p) {
					p.appendChild(new Actor(
						Actor::noAction,
						[this, name, propsName](Actor &a) {
							auto &asset = m_assetBase.find(propsName);
							a.importProps(asset.props());
							if (!name.empty()) {
								a.props().put<string>("_.name", name);
							}
							appendComponents(a, m_assetBase);
						}
					));
				};

				if (parentName.empty()) {
					appendChildTo(m_actor);
				} else {
					Finder<Actor>::find(parentName, appendChildTo);
				}
			}
		}

		void unload() {
			Finder<Actor>::find("sys:root", [this](Actor &a) {
				for (auto &resource : m_node.get_child("resources")) {
					auto resourceName = resource.second.get<string>("indexer.name", "<noname>");
					a.appendAction([resourceName](Actor &) {
						Finder<Actor>::find(resourceName, [](Actor &a) {
							delete &a;
						});
					});
				}
				a.appendAction([this](Actor &) {
					delete &m_actor;
				});
			});
		}
	};
}
