#include <actor.h>
#include <clist.h>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial5 {
	class ComponentA : public CList {
	public:
		static constexpr uint32_t tag = makeTag("CMP1");
		ComponentA() : CList(tag) {}
		virtual ~ComponentA() override {
			cout << "Component1::dtor" << endl;
		}
		void execute() {
			cout << __FUNCTION__ << endl;
			cout << "\t" << "Hello world." << endl;
		}
	};

	class ComponentB : public CList {
	public:
		static constexpr uint32_t tag = makeTag("CMP2");
		ComponentB() : CList(tag) {}
		virtual ~ComponentB() override {
			cout << "Component2::dtor" << endl;
		}
		void execute(const string name) {
			cout << __FUNCTION__ << endl;
			cout << "\t" << "Hello, " << name << endl;
		}
	};

	void test1() {
		cout << __FUNCTION__ << endl;
		CList clist(0);
		auto c1 = new ComponentA();
		auto c2 = new ComponentB();
		clist.append(c1);
		clist.append(c2);
		cout << "--- exec" << endl;
		if (auto component = clist.get<ComponentA>()) {
			component->execute();
		}
		if (auto component = clist.get<ComponentB>()) {
			component->execute("Alice");
		}
		cout << "--- dtor" << endl;
		delete c1;
		delete c2;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		Actor a;
		a.appendComponent(new ComponentA());
		a.appendComponent(new ComponentB());
		cout << "--- exec" << endl;
		a.getComponent<ComponentA>([](auto &component) {
			component.execute();
		});
		a.getComponent<ComponentB>([](auto &component) {
			component.execute("Alice");
		});
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial5() {
	Tutorial5::test1();
	Tutorial5::test2();
	return 0;
}
