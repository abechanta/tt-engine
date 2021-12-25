#include <app.h>
#include <cstdint>
#include <iostream>
#include <memory>
using namespace tte;

class Tutorial11_1 : public App {
private:
	bool m_bQuit;

public:
	explicit Tutorial11_1()
		: m_bQuit(false)
	{
		cout << __FUNCTION__ << endl;
	}

	virtual ~Tutorial11_1() override {
		cout << __FUNCTION__ << endl;
	}

	virtual void initialize() override {
		cout << __FUNCTION__ << endl;
	}

	virtual void finalize() override {
		cout << __FUNCTION__ << endl;
	}

	virtual bool isRunning() override {
		return !m_bQuit;
	}

	virtual void tick() override {
		cout << __FUNCTION__ << endl;
		m_bQuit = true;
	}
};

extern "C" int tutorial11_1() {
	cout << __FUNCTION__ << endl;
	{
		auto app = make_unique<Tutorial11_1>();
		App::Runner runner(std::move(app));
		runner.run();
	}
	return 0;
}
