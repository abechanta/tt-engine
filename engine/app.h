#pragma once
#include <cassert>
#include <cstdint>
#include <memory>

namespace tte {
	using namespace std;

	class App {
		//
		// public definitions
		//
	public:
		class Runner {
		private:
			std::unique_ptr<App> m_app;

		public:
			explicit Runner(
				std::unique_ptr<App> &&app
			) : m_app(std::move(app)) {
				assert(m_app);
			}

			virtual ~Runner() {
				m_app.reset();
			}

			void run() {
				m_app->initialize();
				while (m_app->isRunning()) {
					m_app->tick();
				}
				m_app->finalize();
			}
		};

		//
		// public methods
		//
	public:
		App() {}
		virtual ~App() {}
		virtual void initialize() = 0;
		virtual void finalize() = 0;
		virtual bool isRunning() = 0;
		virtual void tick() = 0;
	};
}
