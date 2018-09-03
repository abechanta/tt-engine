#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>
#include <boost/property_tree/json_parser.hpp>
using namespace boost;
using namespace std;

void checkMemoryLeak() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}

void dump(const property_tree::ptree &p) {
	write_json(cout, p);
}

extern "C" int tutorial();
int main() {
	checkMemoryLeak();
	return tutorial();
}
