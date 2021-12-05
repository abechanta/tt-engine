#pragma once
#include <actor.h>
#include <adapters/sdl2/handler.h>
#include <asset.h>
#include <button.h>
#include <cassert>
#include <components/input_component.h>
#include <components/material_component.h>
#include <components/renderer2d_component.h>
#include <components/transform_component.h>
#include <cstdint>
#include <deque>
#include <finder.h>
#include <iostream>
#include <memory>
#include <ptree.h>
#include <renderer2d.h>
#include <SDL.h>
#include <SDL_image.h>
#include <unordered_map>
#include <vector>

namespace tte {
	namespace sdl2 {
		class Adapter {
			//
			// public definitions
			//
		private:
			struct WindowProperty {
				typedef WindowProperty this_type;
				string title;
				vector2i pos, size;
				uint32_t flags;
				uint32_t refreshRate;

				static this_type parse(const property_tree::ptree &pt) {
					this_type v;
					PTree::parse<this_type, string>("title", "<title>", [](this_type &v) -> string & { return v.title; })(v, pt);
					PTree::parse<this_type, vector2i, int32_t>("pos", PTree::subkeysXYZW, { SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, }, [](this_type &v) -> vector2i & { return v.pos; })(v, pt);
					PTree::parse<this_type, vector2i, int32_t>("size", PTree::subkeysWH, { 640, 480, }, [](this_type &v) -> vector2i & { return v.size; })(v, pt);
					PTree::parse<this_type, uint32_t>("flags", SDL_WINDOW_SHOWN, [](this_type &v) -> uint32_t & { return v.flags; })(v, pt);
					PTree::parse<this_type, uint32_t>("refreshRate", 30, [](this_type &v) -> uint32_t & { return v.refreshRate; })(v, pt);
					return v;
				}
			};

			struct RendererProperty {
				typedef RendererProperty this_type;
				int32_t index;
				uint32_t flags;
				vec<uint8_t, 4> clearColor;

				static this_type parse(const property_tree::ptree &pt) {
					this_type v;
					PTree::parse<this_type, int32_t>("index", -1, [](this_type &v) -> int32_t & { return v.index; })(v, pt);
					PTree::parse<this_type, uint32_t>("flags", SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC, [](this_type &v) -> uint32_t & { return v.flags; })(v, pt);
					PTree::parse<this_type, vec<uint8_t, 4>, uint8_t>("clearColor", PTree::subkeysRGBA, { 0, 0, 0, 255, }, [](this_type &v) -> vec<uint8_t, 4> & { return v.clearColor; })(v, pt);
					return v;
				}
			};

			class Renderer2dInterface : public tte::Renderer2dInterface {
			private:
				tte::Renderer2d &m_renderer;

			public:
				explicit Renderer2dInterface(std::unique_ptr<SDL_Window> &mainWindow, RendererProperty &rendererProps, tte::Renderer2d &renderer)
					: m_renderer(renderer)
				{
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
						auto &uv0 = material.to_vector2i(material._uv0());
						auto &uv1 = material.to_vector2i(material._uv1());
						auto srcRect = SDL_Rect{ X(uv0), Y(uv0), X(uv1) - X(uv0), Y(uv1) - Y(uv0), };
						const vector3i &t = Geometry::pos(m_renderer.mat(), vector3{ 0.f, 0.f, 0.f, });
						auto dstRect = SDL_Rect{ static_cast<int32_t>(X(t) + X(pos)), static_cast<int32_t>(Y(t) + Y(pos)), X(s), Y(s), };
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

				static this_type parse(const property_tree::ptree &pt) {
					this_type v;
					PTree::parse<this_type, string>("alias", "empty", [](this_type &v) -> string & { return v.alias; })(v, pt);
					PTree::parse<this_type, int32_t>("scancode", 0, [](this_type &v) -> int32_t & { return v.scancode; })(v, pt);
					return v;
				}
			};

			struct VKeys {
				typedef VKeys this_type;
				deque<VKey> vkeys;

				static this_type parse(const property_tree::ptree &pt) {
					this_type v;
					PTree::inserter<this_type, deque<VKey>, VKey>("vkeys", [](this_type &v) -> back_insert_iterator<deque<VKey> > { return back_inserter<deque<VKey> >(v.vkeys); }, VKey::parse)(v, pt);
					return v;
				}
			};


			//
			// member variables
			//
		private:
			std::unique_ptr<SDL_Window> m_mainWindow;
			std::unique_ptr<SDL_Surface, empty_delete<SDL_Surface> > m_mainSurface;
			WindowProperty m_windowProps;
			RendererProperty m_rendererProps;
			VKeys m_vkeys;

			//
			// public methods
			//
		private:
			explicit Adapter()
				: m_mainWindow(), m_mainSurface()
			{
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

			static std::unique_ptr<Adapter> &create() {
				static std::unique_ptr<Adapter> s_adapter;
				assert(!s_adapter);
				s_adapter.reset(new Adapter());
				return s_adapter;
			}

			//
			// window management
			//
		public:
			Actor *createWindowActor(Asset &config, const Actor::Action &initializer) {
				return new Actor(
					[this](Actor &a) {
						// SDL_UpdateWindowSurface(this->m_mainWindow.get());
						SDL_Delay(1000 / m_windowProps.refreshRate);
					},
					[this, &config](Actor &a) {
						m_windowProps = WindowProperty::parse(config.props("window/sdl2"));
						m_mainWindow.reset(SDL_CreateWindow(
							m_windowProps.title.c_str(),
							X(m_windowProps.pos),
							Y(m_windowProps.pos),
							X(m_windowProps.size),
							Y(m_windowProps.size),
							m_windowProps.flags
						));
						assert(m_mainWindow);
						// this->m_mainSurface.reset(SDL_GetWindowSurface(this->m_mainWindow.get()));
						// assert(m_mainSurface);
					} + initializer
				);
			}

			//
			// renderer management
			//
			Actor *createRendererActor(Asset &config, const Actor::Action &initializer) {
				return new Actor(
					[this](Actor &a) {
						a.getComponent<tte::Renderer2d>([this, &a](auto &renderer) {
							SDL_RenderPresent(sdl2::Renderer2d::get(renderer));
							SDL_SetRenderDrawColor(
								sdl2::Renderer2d::get(renderer),
								X(m_rendererProps.clearColor),
								Y(m_rendererProps.clearColor),
								Z(m_rendererProps.clearColor),
								W(m_rendererProps.clearColor)
							);
							SDL_RenderClear(sdl2::Renderer2d::get(renderer));
						});
					},
					[this, &config](Actor &a) {
						m_rendererProps = RendererProperty::parse(config.props("renderer/sdl2"));
						tte::Renderer2d::append([this](Actor &, tte::Renderer2d &renderer) {
							new Renderer2dInterface(m_mainWindow, m_rendererProps, renderer);
						})(a);
					} + initializer
				);
			}

			//
			// input management
			//
			Actor *createInputActor(Asset &config, const Actor::Action &initializer) {
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
						Input::append()(a);
						a.props().put<bool>("quit", false);
					} + initializer
				);
			}

			Actor *createKeyboardActor(Asset &config, const Actor::Action &initializer) {
				return new Actor(
					[this](Actor &a) {
						Finder<Actor>::find<Input>(inputActor, [this, &a](auto &input) {
							auto kbdInput = SDL_GetKeyboardState(nullptr);
							for (auto &vkey : m_vkeys.vkeys) {
								input.buttons(vkey.alias).update(kbdInput[vkey.scancode]);
							}
						});
					},
					[this, &config](Actor &a) {
						Finder<Actor>::find<Input>(inputActor, [this, &config](auto &input) {
							m_vkeys = VKeys::parse(config.props("input/sdl2"));
							for (auto &vkey : m_vkeys.vkeys) {
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
