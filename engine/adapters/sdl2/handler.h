#pragma once
#include <actor.h>
#include <asset.h>
#include <components/material_component.h>
#include <components/renderer2d_component.h>
#include <components/transform_component.h>
#include <finder.h>
#include <functional>
#include <geometry.h>
#include <helpers/actor_actions.h>
#include <memory>
#include <SDL.h>

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
		static constexpr char rootActor[] = "sys:root";
		static constexpr char resourceActor[] = "sys:resource";
		static constexpr char renderer2dActor[] = "sys:renderer2d";
		static constexpr char inputActor[] = "sys:input";
		static constexpr char inputKbdActor[] = "sys:inputKbd";

		namespace Material {
			inline SDL_Texture *get(const tte::Material &material) {
				const Asset &a = material;
				return a.handle<SDL_Texture>().get();
			}
		};

		namespace Renderer2d {
			inline SDL_Renderer *get(const tte::Renderer2d &renderer) {
				const Renderer2dInterface &renderer2d = renderer;
				return renderer2d.handle<SDL_Renderer>().get();
			}
		};
	}
}
