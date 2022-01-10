#if defined(_WINDOWS)
#	define _CRTDBG_MAP_ALLOC
#	include <crtdbg.h>
#endif // defined(_WINDOWS)
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

extern "C" int tutorial0();
extern "C" int tutorial1();
extern "C" int tutorial2();
extern "C" int tutorial3();
extern "C" int tutorial4();
extern "C" int tutorial5();
extern "C" int tutorial6();
extern "C" int tutorial7();
extern "C" int tutorial10();
extern "C" int tutorial11();
extern "C" int tutorial12();
extern "C" int tutorial13();
extern "C" int tutorial14();
extern "C" int column1();
extern "C" int column2();
extern "C" int sdl_splash();
extern "C" int showcase1();
unordered_map<string, function<int()> > invokeTable = {
	{ "0", tutorial0, },
	{ "1", tutorial1, },
	{ "2", tutorial2, },
	{ "3", tutorial3, },
	{ "4", tutorial4, },
	{ "5", tutorial5, },
	{ "6", tutorial6, },
	{ "7", tutorial7, },
	{ "10", tutorial10, },
	{ "11", tutorial11, },
	{ "12", tutorial12, },
	{ "13", tutorial13, },
	{ "14", tutorial14, },
	{ "c1", column1, },
	{ "c2", column2, },
	{ "sdl_1", sdl_splash, },
	{ "s1", showcase1, },
};

void checkMemoryLeak() {
#if defined(_WINDOWS)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // defined(_WINDOWS)
}

bool prompt(const string message, string &in) {
	cout << message;
	cin >> in;
	return (in != "q");
}

int main() {
	checkMemoryLeak();
	string in;
	while (prompt("?", in)) {
		auto it = invokeTable.find(in);
		(it != invokeTable.end()) && it->second();
	}
	return 0;
}
