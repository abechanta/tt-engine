#pragma once
#include <actor.h>
#include <asset.h>
#include <button.h>
#include <finder.h>
#include <adapters/sdl2/handler.h>
#include <components/input_component.h>
#include <components/material_component.h>
#include <ptree.h>
#include <renderer2d.h>
#include <components/renderer2d_component.h>
#include <components/transform_component.h>
#include <SDL.h>
#include <SDL_image.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

namespace tte {
	namespace sdl2 {
		class Adapter {
			//
			// public definitions
			//
		private:
			struct Window {
				typedef Window this_type;
				string title;
				vector2i pos, size;
				uint32_t flags;

				static this_type load(const property_tree::ptree &pt) {
					this_type v;
					PTree::setter<this_type, string>("title", "<title>", [](this_type &v) -> string & { return v.title; })(v, pt);
					PTree::setter<this_type, vector2i, int32_t>("pos", { ".x", ".y", }, { SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, }, [](this_type &v) -> vector2i & { return v.pos; })(v, pt);
					PTree::setter<this_type, vector2i, int32_t>("size", { ".w", ".h", }, { 640, 480, }, [](this_type &v) -> vector2i & { return v.size; })(v, pt);
					PTree::setter<this_type, uint32_t>("flags", SDL_WINDOW_SHOWN, [](this_type &v) -> uint32_t & { return v.flags; })(v, pt);
					return v;
				}
			};

			struct Renderer {
				typedef Renderer this_type;
				int32_t index;
				uint32_t flags;
				vec<uint8_t, 4> clearColor;

				static this_type load(const property_tree::ptree &pt) {
					this_type v;
					PTree::setter<this_type, int32_t>("index", -1, [](this_type &v) -> int32_t & { return v.index; })(v, pt);
					PTree::setter<this_type, uint32_t>("flags", SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC, [](this_type &v) -> uint32_t & { return v.flags; })(v, pt);
					PTree::setter<this_type, vec<uint8_t, 4>, uint8_t>("clearColor", { ".r", ".g", ".b", ".a", }, { 0, 0, 0, 255, }, [](this_type &v) -> vec<uint8_t, 4> & { return v.clearColor; })(v, pt);
					return v;
				}
			};

			class Renderer2dInterface : public tte::Renderer2dInterface {
			private:
				tte::Renderer2d &m_renderer;

			public:
				Renderer2dInterface(
					std::unique_ptr<SDL_Window> &mainWindow,
					Renderer &rendererProps,
					tte::Renderer2d &renderer
				) : m_renderer(renderer) {
					handle<SDL_Renderer>().reset(SDL_CreateRenderer(
						mainWindow.get(),
						rendererProps.index,
						rendererProps.flags
					));
					assert(handle<SDL_Renderer>());
					m_renderer.setRenderer(this);
				}

				virtual ~Renderer2dInterface() {
				}

				virtual void drawPoint(Actor &a, const vector2 &point) const override {}
				virtual void drawPoints(Actor &a, const vector<vector2> &points) const override {}
				virtual void drawLine(Actor &a, const vector2 &lineS, const vector2 &lineE) const override {}
				virtual void drawLines(Actor &a, const vector<vector2> &lineS, const vector<vector2> &lineE) const override {}
				virtual void drawLines(Actor &a, const vector<vector2> &points, const vector<int32_t> &lines) const override {}

				virtual void drawRect(Actor &a, const vector2 &pos, const vector2 &size, const vector2 &anchor, const vec<bool, 2> &flip) const override {
					a.getComponent<tte::Material>([this, &pos, &size, &anchor, &flip, &a](auto &material) {
						const vector2i s = size;
						auto &uv0 = material.to_vector2i(material.uv0());
						auto &uv1 = material.to_vector2i(material.uv1());
						auto srcRect = SDL_Rect{ X(uv0), Y(uv0), X(uv1) - X(uv0), Y(uv1) - Y(uv0), };
						const vector3i &t = Geometry::pos(m_renderer.mat(), XY0(pos));
						auto dstRect = SDL_Rect{ X(t), Y(t), X(s), Y(s), };
						float rotZ = Geometry::angZ(m_renderer.mat()) * Geometry::rad2deg;
						auto center = SDL_Point{ static_cast<int32_t>(X(anchor) * X(s)), static_cast<int32_t>(Y(anchor) * Y(s)), };
						auto flipFlag = (X(flip) ? SDL_FLIP_HORIZONTAL : 0) | (Y(flip) ? SDL_FLIP_VERTICAL : 0);
						SDL_RenderCopyEx(Renderer2d::get(m_renderer), Material::get(material), &srcRect, &dstRect, rotZ, &center, static_cast<SDL_RendererFlip>(flipFlag));
					});
				}
			};

			struct VKey {
				typedef VKey this_type;
				string alias;
				int32_t scancode;

				static this_type load(const property_tree::ptree &pt) {
					this_type v;
					PTree::setter<this_type, string>("alias", "empty", [](this_type &v) -> string & { return v.alias; })(v, pt);
					PTree::setter<this_type, int32_t>("scancode", 0, [](this_type &v) -> int32_t & { return v.scancode; })(v, pt);
					return v;
				}
			};

			//
			// member variables
			//
		private:
			std::unique_ptr<SDL_Window> m_mainWindow;
			std::unique_ptr<SDL_Surface, empty_delete<SDL_Surface> > m_mainSurface;
			Window m_window;
			Renderer m_renderer;
			vector<VKey> m_vkeys;

			//
			// public methods
			//
		private:
			Adapter() : m_mainWindow(), m_mainSurface() {
				if (SDL_Init(SDL_INIT_VIDEO) < 0) {
					cout << SDL_GetError() << endl;
					assert(false);
				}
				if (IMG_Init(IMG_INIT_PNG) <= 0) {
					cout << IMG_GetError() << endl;
					assert(false);
				}
			}

		public:
			virtual ~Adapter() {
				m_mainSurface.reset();
				m_mainWindow.reset();
				IMG_Quit();
				SDL_Quit();
			}

			static std::unique_ptr<Adapter> & create() {
				static std::unique_ptr<Adapter> s_adapter;
				assert(!s_adapter);
				s_adapter.reset(new Adapter());
				return s_adapter;
			}

			//
			// window management
			//
		public:
			Actor * createWindowActor(const Asset &config, const Actor::Action &initializer) {
				return new Actor(
					[this](Actor &a) {
						//SDL_UpdateWindowSurface(this->m_mainWindow.get());
						//uint32_t refreshRate = a.get<uint32_t>("refreshRate", 30);
						//SDL_Delay(1000 / refreshRate);
					},
					[this, &config](Actor &a) {
						a.importProps(config.props().get_child("window/sdl2"));
						m_window = Window::load(a.props());
						m_mainWindow.reset(SDL_CreateWindow(
							m_window.title.c_str(),
							X(m_window.pos),
							Y(m_window.pos),
							X(m_window.size),
							Y(m_window.size),
							m_window.flags
						));
						assert(m_mainWindow);
						//this->m_mainSurface.reset(SDL_GetWindowSurface(this->m_mainWindow.get()));
						//assert(m_mainSurface);
					} + initializer
				);
			}

			//
			// renderer management
			//
			Actor * createRendererActor(const Asset &config, const Actor::Action &initializer) {
				return new Actor(
					[this](Actor &a) {
						a.getComponent<tte::Renderer2d>([this, &a](auto &renderer) {
							SDL_RenderPresent(sdl2::Renderer2d::get(renderer));
							SDL_SetRenderDrawColor(
								sdl2::Renderer2d::get(renderer),
								X(m_renderer.clearColor),
								Y(m_renderer.clearColor),
								Z(m_renderer.clearColor),
								W(m_renderer.clearColor)
							);
							SDL_RenderClear(sdl2::Renderer2d::get(renderer));
						});
					},
					[this, &config](Actor &a) {
						a.importProps(config.props().get_child("renderer/sdl2"));
						m_renderer = Renderer::load(a.props());
						tte::Renderer2d::append([this](Actor &, tte::Renderer2d &renderer) {
							new Renderer2dInterface(m_mainWindow, m_renderer, renderer);
						})(a);
					} + initializer
				);
			}

			//
			// input management
			//
			Actor * createInputActor(const Asset &config, const Actor::Action &initializer) {
				return new Actor(
					[](Actor &a) {
						SDL_Event e;
						while (SDL_PollEvent(&e) != 0) {
							//if ((e.type == SDL_MOUSEBUTTONDOWN) || (e.type == SDL_MOUSEBUTTONUP)) {
							//	continue;
							//}
							//if (e.type == SDL_MOUSEWHEEL) {
							//	continue;
							//}
							//if (e.type == SDL_MOUSEMOTION) {
							//	continue;
							//}
							if (e.type == SDL_QUIT) {
								a.props().put<bool>("quit", true);
								continue;
							}
						}
						a.getComponent<Input>([](auto &input) {
							input.update();
						});
					},
					[&config](Actor &a) {
						a.importProps(config.props().get_child("input/sdl2"));
						Input::append()(a);
						a.props().put<bool>("quit", false);
					} + initializer
				);
			}

			Actor * createKeyboardActor(const Asset &config, const Actor::Action &initializer) {
				return new Actor(
					[this](Actor &a) {
						Finder<Actor>::find<Input>(inputActor, [this, &a](auto &input) {
							auto kbdInput = SDL_GetKeyboardState(nullptr);
							for (auto &vkey : m_vkeys) {
								input.buttons(vkey.alias).update(kbdInput[vkey.scancode]);
							}
						});
					},
					[this, &config](Actor &a) {
						a.importProps(config.props().get_child("input/sdl2"));
						Finder<Actor>::find<Input>(inputActor, [this, &a](auto &input) {
							for (auto &key : a.props().get_child("vkeys")) {
								auto vkey = VKey::load(key.second);
								m_vkeys.push_back(vkey);
								input.buttons(vkey.alias).update(0);
							}
						});
					} + initializer
				);
			}

		public:
			static function<void(Asset &)> typePng(Adapter &adapter) {
				return [&adapter](Asset &a) {
					cout << __FUNCTION__ << ": " << a.path() << endl;
					assert(filesystem::is_regular_file(a.path()));
					a.setLoader([&adapter](Asset &a, bool bLoad) -> bool {
						if (bLoad) {
							std::unique_ptr<SDL_Surface> surface(IMG_Load(a.path().string().c_str()));
							assert(surface);
							a.props().put<string>("contentType", "image/png");
							a.props().put<int32_t>("size.w", surface->w);
							a.props().put<int32_t>("size.h", surface->h);
							a.props().put<uint32_t>("format", surface->format->format);
							Finder<Actor>::find<tte::Renderer2d>(rendererActor, [&a, &surface](auto &renderer) {
								a.handle<SDL_Texture>().reset(SDL_CreateTextureFromSurface(sdl2::Renderer2d::get(renderer), surface.get()));
							});
						} else {
							a.handle<SDL_Texture>().reset();
							a.props().clear();
						}
						return true;
					});
				};
			}
		};
	}
}
