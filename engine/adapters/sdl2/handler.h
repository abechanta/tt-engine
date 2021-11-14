#pragma once
#include <actor.h>
#include <actor_modifiers.h>
#include <asset.h>
#include <finder.h>
#include <geometry.h>
#include <components/material_component.h>
#include <components/primitive_component.h>
#include <components/renderer2d_component.h>
#include <components/transform_component.h>
#include <SDL.h>
#include <functional>
#include <memory>

namespace std {
	template<>
	struct default_delete<SDL_Renderer> {
		void operator()(SDL_Renderer *p) {
			SDL_DestroyRenderer(p);
		}
	};

	template<>
	struct default_delete<SDL_Surface> {
		void operator()(SDL_Surface *p) {
			SDL_FreeSurface(p);
		}
	};

	template<>
	struct default_delete<SDL_Texture> {
		void operator()(SDL_Texture *p) {
			SDL_DestroyTexture(p);
		}
	};

	template<>
	struct default_delete<SDL_Window> {
		void operator()(SDL_Window *p) {
			SDL_DestroyWindow(p);
		}
	};
}

namespace tte {
	namespace sdl2 {
		static constexpr char windowActor[] = "window:";
		static constexpr char rendererActor[] = "renderer:";
		static constexpr char inputActor[] = "input:";

		struct Material : public tte::Material {
			typedef tte::Material Component;
			typedef SDL_Texture Handle;

			static Handle * get(const Component &material) {
				const Asset &a = material;
				return a.handle<Handle>().get();
			}
		};

		struct Renderer2d : public tte::Renderer2d {
			typedef tte::Renderer2d Component;
			typedef SDL_Renderer Handle;

			static Handle * get(const Component &renderer) {
				return renderer.handle<Handle>().get();
			}
		};
	}
}
