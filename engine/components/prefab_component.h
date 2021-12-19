#pragma once
#include <actor.h>
#include <asset.h>
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

		void appendComponents(Actor &a) {
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
					Material::append(m_assetBase, true)(a);
				}
				if (componentType == "resource") {
					Resource::append(m_assetBase, true)(a);
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
		}

		void loadResources() {
			for (auto &resource : m_node.get_child("resources")) {
				auto parentName = resource.second.get<string>("parent");
				Finder<Actor>::find(parentName, [this, &resourceProps = resource.second](Actor &p) {
					p.appendChild(new Actor(
						Actor::noAction,
						[this, &resourceProps](Actor &a) {
							a.importProps(resourceProps);
							appendComponents(a);
						}
					));
				});
			}
		}

		void createActors() {
			for (auto &actor : m_node.get_child("actors")) {
				auto name = actor.second.get<string>("name", "");
				auto propsName = actor.second.get<string>("property", "");
				auto parentName = actor.second.get<string>("parent", "");
				auto createAndAppendChild = [this, name, propsName](Actor &p) {
					p.appendChild(new Actor(
						Actor::noAction,
						[this, name, propsName](Actor &a) {
							auto &asset = m_assetBase.find(propsName);
							a.importProps(asset.props());
							appendComponents(a);
							a.props().put<string>("_.name", name);
						}
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
