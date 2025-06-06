#include <iostream>
#include "Functions.h"
#include "Process.h"

Process pr;

int Functions::screen() {
	std::cout << "Screen command detected doing something";
	return 0;
}

int Functions::schedulerTest() {
	std::cout << "scheduler-test command detected doing something";
	return 0;
}

int Functions::schedulerStop() {
	std::cout << "scheduler-stop command detected doing something";
	return 0;
}

int Functions::reportUtil() {
	pr.displayAllProcessInfo();
	return 0;
}