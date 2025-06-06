#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <unordered_map>

#include "consoleLayout.h"
#include "Display.h"
#include "gFunctions.h"

gFunctions gF;
Display disp;
using namespace std;

int main() {
	consoleLayout layout;

    std::string initializer = "notinitialized";
	std::string start = "true";
	int clearlater = 0;

	layout.controller(initializer);

}
