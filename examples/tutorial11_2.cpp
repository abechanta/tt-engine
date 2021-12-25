#include <app.h>
#include <SDL.h>
#include <SDL_image.h>
#include <cstdint>
#include <iostream>
#include <memory>
using namespace tte;

class Tutorial11_2 : public App {
private:
	static constexpr int32_t screenWidth = 256;
	static constexpr int32_t screenHeight = 240;

private:
	bool m_bQuit;
	SDL_Window *m_pMainWindow;
	SDL_Surface *m_pMainSurface;
	SDL_Surface *m_pSplashSurface;

public:
	explicit Tutorial11_2()
		: m_bQuit(false), m_pMainWindow(nullptr), m_pMainSurface(nullptr), m_pSplashSurface(nullptr)
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

	virtual ~Tutorial11_2() override {
		cout << __FUNCTION__ << endl;
		IMG_Quit();
		SDL_Quit();
	}

	virtual void initialize() override {
		cout << __FUNCTION__ << endl;
		m_bQuit = false;
		m_pMainWindow = SDL_CreateWindow(__FILE__, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
		assert(m_pMainWindow);
		m_pMainSurface = SDL_GetWindowSurface(m_pMainWindow);
		assert(m_pMainSurface);
		m_pSplashSurface = IMG_Load("asset/boot/splash.png");
		assert(m_pSplashSurface);
	}

	virtual void finalize() override {
		cout << __FUNCTION__ << endl;
		SDL_FreeSurface(m_pSplashSurface), m_pSplashSurface = nullptr;
		SDL_DestroyWindow(m_pMainWindow), m_pMainWindow = nullptr, m_pMainSurface = nullptr;
	}

	virtual bool isRunning() override {
		return !m_bQuit;
	}

	virtual void tick() override {
		cout << __FUNCTION__ << endl;
		SDL_BlitSurface(m_pSplashSurface, nullptr, m_pMainSurface, nullptr);
		SDL_UpdateWindowSurface(m_pMainWindow);
		SDL_Delay(200);
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

extern "C" int tutorial11_2() {
	cout << __FUNCTION__ << endl;
	{
		auto app = make_unique<Tutorial11_2>();
		App::Runner runner(std::move(app));
		runner.run();
	}
	return 0;
}
