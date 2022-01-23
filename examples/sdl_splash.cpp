#include <app.h>
#include <SDL.h>
#include <SDL_image.h>
#include <cstdint>
#include <iostream>
#include <memory>
using namespace tte;

class SdlSplash : public App {
private:
	bool m_bQuit;
	SDL_Window *m_pMainWindow;
	SDL_Renderer *m_pRenderer;
	SDL_Texture *m_pTexture;

public:
	explicit SdlSplash()
		: m_bQuit(false),
		m_pMainWindow(nullptr), m_pRenderer(nullptr), m_pTexture(nullptr)
	{
		cout << __FUNCTION__ << endl;
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			cout << SDL_GetError() << endl;
			assert(false);
		}
		if (IMG_Init(IMG_INIT_PNG) <= 0) {
			cout << IMG_GetError() << endl;
			assert(false);
		}
	}

	virtual ~SdlSplash() override {
		cout << __FUNCTION__ << endl;
		IMG_Quit();
		SDL_Quit();
	}

	virtual void initialize() override {
		cout << __FUNCTION__ << endl;
		m_bQuit = false;
		m_pMainWindow = SDL_CreateWindow(
			__FILE__, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			256, 240, SDL_WINDOW_SHOWN
		);
		assert(m_pMainWindow);
		m_pRenderer = SDL_CreateRenderer(
			m_pMainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
		);
		assert(m_pRenderer);
		SDL_Surface *pSurface = IMG_Load("asset/sdl/splash.png");
		assert(pSurface);
		m_pTexture = SDL_CreateTextureFromSurface(m_pRenderer, pSurface);
		assert(m_pTexture);
		SDL_FreeSurface(pSurface);
	}

	virtual void finalize() override {
		cout << __FUNCTION__ << endl;
		SDL_DestroyTexture(m_pTexture); m_pTexture = nullptr;
		SDL_DestroyRenderer(m_pRenderer); m_pRenderer = nullptr;
		SDL_DestroyWindow(m_pMainWindow); m_pMainWindow = nullptr;
	}

	virtual bool isRunning() override {
		return !m_bQuit;
	}

	virtual void tick() override {
		SDL_RenderPresent(m_pRenderer);
		SDL_SetRenderDrawColor(m_pRenderer, 192, 192, 192, 255);
		SDL_RenderClear(m_pRenderer);
		SDL_RenderCopyEx(
			m_pRenderer,
			m_pTexture,
			&SDL_Rect{0, 0, 256, 240},
			&SDL_Rect{0, 0, 256, 240},
			0.0f,
			&SDL_Point{0, 0},
			static_cast<SDL_RendererFlip>(0)
		);
		// SDL_Delay(200);
		m_bQuit = handleEvent();
	}

private:
	bool handleEvent() const {
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0) {
			if (handleWindowEvent(e) || handleKeyEvent(e) || handleMouseEvent(e)) {
				return true;
			}
		}
		return false;
	}

	bool handleWindowEvent(const SDL_Event &e) const {
		return (e.type == SDL_QUIT);
	}

	bool handleKeyEvent(const SDL_Event &e) const {
		return (e.type == SDL_KEYDOWN) && (e.key.keysym.sym == SDLK_ESCAPE);
	}

	bool handleMouseEvent(const SDL_Event &e) const {
		return (e.type == SDL_MOUSEBUTTONDOWN);
	}
};

extern "C" int sdl_splash() {
	{
		auto app = make_unique<SdlSplash>();
		App::Runner runner(std::move(app));
		runner.run();
	}
	return 0;
}
