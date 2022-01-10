#pragma once
#include <actor.h>
#include <adapters/sdl2/handler.h>
#include <asset.h>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <components/input_component.h>
#include <components/material_component.h>
#include <components/renderer2d_component.h>
#include <components/transform_component.h>
#include <cstdint>
#include <deque>
#include <finder.h>
#include <helpers/actor_actions.h>
#include <helpers/actor_modifiers.h>
#include <helpers/actor_triggers.h>
#include <iostream>
#include <memory>
#include <ptree.h>
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace tte {
	namespace sdl2 {
		using namespace boost;
		using namespace std;
		using ptree = property_tree::ptree;

		class Adapter {
			//
			// definitions
			//
		private:
			struct WindowProperty {
				typedef WindowProperty this_type;
				string title;
				vector2i pos, size;
				uint32_t flags;
				uint32_t refreshRate;

				static this_type parse(const ptree &pt) {
					this_type v;
					PTree::parse<this_type, string>("title", "<title>", [](this_type &v) -> string & { return v.title; })(v, pt);
					PTree::parse<this_type, vector2i>("pos", PTree::subkeysXYZW, { SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, }, [](this_type &v) -> vector2i & { return v.pos; })(v, pt);
					PTree::parse<this_type, vector2i>("size", PTree::subkeysWH, { 640, 480, }, [](this_type &v) -> vector2i & { return v.size; })(v, pt);
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

				static this_type parse(const ptree &pt) {
					this_type v;
					PTree::parse<this_type, int32_t>("index", -1, [](this_type &v) -> int32_t & { return v.index; })(v, pt);
					PTree::parse<this_type, uint32_t>("flags", SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC, [](this_type &v) -> uint32_t & { return v.flags; })(v, pt);
					PTree::parse<this_type, vec<uint8_t, 4> >("clearColor", PTree::subkeysRGBA, { 0, 0, 0, 255, }, [](this_type &v) -> vec<uint8_t, 4> & { return v.clearColor; })(v, pt);
					return v;
				}
			};

			struct VKey {
				typedef VKey this_type;
				string alias;
				int32_t scancode;

				static this_type parse(const ptree &pt) {
					this_type v;
					PTree::parse<this_type, string>("alias", "empty", [](this_type &v) -> string & { return v.alias; })(v, pt);
					PTree::parse<this_type, int32_t>("scancode", 0, [](this_type &v) -> int32_t & { return v.scancode; })(v, pt);
					return v;
				}
			};

			struct VKeys {
				typedef VKeys this_type;
				deque<VKey> vkeys;

				static this_type parse(const ptree &pt) {
					this_type v;
					PTree::inserter<this_type, deque<VKey> >("vkeys", [](this_type &v) -> back_insert_iterator<deque<VKey> > { return back_inserter<deque<VKey> >(v.vkeys); }, VKey::parse)(v, pt);
					return v;
				}
			};

			class Renderer2dImpl : public Renderer2dInterface {
			private:
				tte::Renderer2d &m_renderer;

			public:
				explicit Renderer2dImpl(SDL_Renderer *renderer2d, tte::Renderer2d &renderer)
					: m_renderer(renderer)
				{
					handle<SDL_Renderer>().reset(renderer2d);
				}

				virtual ~Renderer2dImpl() override {
					// handle<SDL_Renderer>().reset();	// intentionally not releasing here.
				}

				virtual void drawPoint(Actor &a, const vector2 &point) const override {}
				virtual void drawPoints(Actor &a, const vector<vector2> &points) const override {}
				virtual void drawLine(Actor &a, const vector2 &lineS, const vector2 &lineE) const override {}
				virtual void drawLines(Actor &a, const vector<vector2> &lineS, const vector<vector2> &lineE) const override {}
				virtual void drawLines(Actor &a, const vector<vector2> &points, const vector<int32_t> &lines) const override {}

				virtual void drawRect(Actor &a, const vector2 &pos, const vector2 &size, const vector2 &anchor, const vec<bool, 2> &flip) const override {
					a.getComponent<tte::Material>([this, &pos, &size, &anchor, &flip, &a](auto &material) {
						const vector2i &s = scalar_cast<int32_t>(size);
						const vector2i &uv0 = material.to_pixel(material._uv0());
						const vector2i &uv1 = material.to_pixel(material._uv1());
						const SDL_Rect &srcRect = SDL_Rect{ X(uv0), Y(uv0), X(uv1) - X(uv0), Y(uv1) - Y(uv0), };
						const vector3i t = scalar_cast<int32_t>(Geometry::pos(m_renderer.matrix(), zero_vec<float, 3>()));
						const SDL_Rect &dstRect = SDL_Rect{ static_cast<int32_t>(X(pos) + X(t)), static_cast<int32_t>(Y(pos) + Y(t)), X(s), Y(s), };
						const float rotZ = Geometry::angZ(m_renderer.matrix()) * Geometry::rad2deg;
						const SDL_Point &center = SDL_Point{ static_cast<int32_t>(X(anchor) * X(s)), static_cast<int32_t>(Y(anchor) * Y(s)), };
						const auto flipFlag = (X(flip) ? SDL_FLIP_HORIZONTAL : 0) | (Y(flip) ? SDL_FLIP_VERTICAL : 0);
						SDL_RenderCopyEx(
							Renderer2d::get(m_renderer),
							Material::get(material),
							&srcRect,
							&dstRect,
							rotZ,
							&center,
							static_cast<SDL_RendererFlip>(flipFlag)
						);
					});
				}
			};

			//
			// member variables
			//
		private:
			std::unique_ptr<SDL_Window> m_mainWindow;
			std::unique_ptr<SDL_Renderer> m_renderer2d;
			WindowProperty m_windowProps;
			RendererProperty m_rendererProps;
			VKeys m_vkeys;

			//
			// public methods
			//
		public:
			explicit Adapter(Asset &config)
				: m_mainWindow(),
				m_renderer2d(),
				m_windowProps(WindowProperty::parse(config.props("window/sdl2"))),
				m_rendererProps(RendererProperty::parse(config.props("renderer/sdl2"))),
				m_vkeys(VKeys::parse(config.props("input/sdl2")))
			{
				if (SDL_Init(SDL_INIT_VIDEO) < 0) {
					cout << SDL_GetError() << endl;
					assert(false);
				}
				if (IMG_Init(IMG_INIT_PNG) <= 0) {
					cout << IMG_GetError() << endl;
					assert(false);
				}
				m_mainWindow.reset(SDL_CreateWindow(
					m_windowProps.title.c_str(),
					X(m_windowProps.pos),
					Y(m_windowProps.pos),
					X(m_windowProps.size),
					Y(m_windowProps.size),
					m_windowProps.flags
				));
				assert(m_mainWindow);
				m_renderer2d.reset(SDL_CreateRenderer(
					m_mainWindow.get(),
					m_rendererProps.index,
					m_rendererProps.flags
				));
				assert(m_renderer2d);
			}

			virtual ~Adapter() {
				m_renderer2d.reset();
				m_mainWindow.reset();
				IMG_Quit();
				SDL_Quit();
			}

		public:
			//
			// actor management
			//
			Actor *initializeActors() {
				auto root = new Actor(Actor::noAction, Indexer::append(rootActor) + put<bool>("_.reset", true));
				auto input = createInputActor(Indexer::append(inputActor));
				auto inputKbd = createKeyboardActor(Indexer::append(inputKbdActor));
				auto resource = new Actor(Actor::noAction, Indexer::append(resourceActor));
				auto renderer = createRendererActor(Indexer::append(renderer2dActor));

				root->appendChild(input->appendChild(inputKbd));
				root->appendChild(resource);
				root->appendChild(renderer);
				return root;
			}

		private:
			//
			// renderer management
			//
			Actor *createRendererActor(const Actor::Action &initializer) {
				return new Actor(
					withComponent<tte::Renderer2d>([this](Actor &, auto &renderer) {
						SDL_RenderPresent(Renderer2d::get(renderer));
						SDL_SetRenderDrawColor(
							Renderer2d::get(renderer),
							X(m_rendererProps.clearColor),
							Y(m_rendererProps.clearColor),
							Z(m_rendererProps.clearColor),
							W(m_rendererProps.clearColor)
						);
						SDL_RenderClear(Renderer2d::get(renderer));
						// SDL_Delay(1000 / m_windowProps.refreshRate);
					}),
					tte::Renderer2d::append([this](tte::Renderer2d &renderer) {
						renderer.setRenderer(
							new Renderer2dImpl(m_renderer2d.get(), renderer)
						);
					}) + initializer
				);
			}

			//
			// input management
			//
			Actor *createInputActor(const Actor::Action &initializer) {
				return new Actor(
					withComponent<Input>([](Actor &a, auto &input) {
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
						input.update();
					}),
					Input::append() + put<bool>("quit", false) + initializer
				);
			}

			Actor *createKeyboardActor(const Actor::Action &initializer) {
				return new Actor(
					findThen(inputActor, withComponent<Input>([this](Actor &, auto &input) {
						auto kbdInput = SDL_GetKeyboardState(nullptr);
						for (auto &vkey : m_vkeys.vkeys) {
							input.buttons(vkey.alias).update(kbdInput[vkey.scancode]);
						}
					})),
					findThen(inputActor, withComponent<Input>([this](Actor &, auto &input) {
						for (auto &vkey : m_vkeys.vkeys) {
							input.buttons(vkey.alias).update(0);
						}
					})) + initializer
				);
			}

		public:
			static void typeBmp(Asset &a) {
				cout << __FUNCTION__ << ": " << a.path() << endl;
				typeAny(a, "image/bmp");
			}

			static void typeGif(Asset &a) {
				cout << __FUNCTION__ << ": " << a.path() << endl;
				typeAny(a, "image/gif");
			}

			static void typeJpg(Asset &a) {
				cout << __FUNCTION__ << ": " << a.path() << endl;
				typeAny(a, "image/jpg");
			}

			static void typePng(Asset &a) {
				cout << __FUNCTION__ << ": " << a.path() << endl;
				typeAny(a, "image/png");
			}

			static void typeTga(Asset &a) {
				cout << __FUNCTION__ << ": " << a.path() << endl;
				typeAny(a, "image/tga");
			}

		private:
			static void typeAny(Asset &a, const string &mimeType) {
				assert(filesystem::is_regular_file(a.path()));
				a.setHandler([mimeType](Asset &a, bool bLoad) -> bool {
					if (bLoad) {
						std::unique_ptr<SDL_Surface> surface(IMG_Load(a.path().string().c_str()));
						assert(surface);
						a.props().put<string>("contentType", mimeType);
						a.props().put<int32_t>("size.w", surface->w);
						a.props().put<int32_t>("size.h", surface->h);
						a.props().put<uint32_t>("format", surface->format->format);
						Finder<Actor>::find<tte::Renderer2d>(renderer2dActor, [&a, &surface](auto &renderer) {
							a.handle<SDL_Texture>().reset(SDL_CreateTextureFromSurface(
								Renderer2d::get(renderer),
								surface.get()
							));
						});
					} else {
						a.handle<SDL_Texture>().reset();
						a.props().clear();
					}
					return true;
				});
			}
		};
	}
}
