#if defined(_WINDOWS)
#	define _CRTDBG_MAP_ALLOC
#	include <crtdbg.h>
#endif // defined(_WINDOWS)
#include <stdlib.h>
#include <functional>
#include <iostream>
#include <boost/property_tree/json_parser.hpp>
#include <map>
#include <string>
using namespace boost;
using namespace std;

extern "C" int tutorial1();
extern "C" int tutorial2();
extern "C" int tutorial3();
extern "C" int tutorial4();
extern "C" int tutorial5();
extern "C" int tutorial6();
extern "C" int tutorial7();
extern "C" int tutorial8();
extern "C" int tutorial9();
extern "C" int tutorial10();
extern "C" int tutorial11_1();
extern "C" int tutorial11_2();
extern "C" int column1();
extern "C" int column2();
extern "C" int showcase1();
map<string, function<int()> > invokeTable = {
	{ "1", tutorial1, },
	{ "2", tutorial2, },
	{ "3", tutorial3, },
	{ "4", tutorial4, },
	{ "5", tutorial5, },
	{ "6", tutorial6, },
	{ "7", tutorial7, },
	{ "8", tutorial8, },
	{ "9", tutorial9, },
	{ "10", tutorial10, },
	{ "11_1", tutorial11_1, },
	{ "11_2", tutorial11_2, },
	{ "c1", column1, },
	{ "c2", column2, },
	{ "s1", showcase1, },
};

void checkMemoryLeak() {
#if defined(_WINDOWS)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // defined(_WINDOWS)
}

void dump(const property_tree::ptree &p) {
	write_json(cout, p);
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
