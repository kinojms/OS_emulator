#include <iostream>
#include <cstdlib>
#include "gFunctions.h"
#include "windows_console_minimal.h"

int gFunctions::clearScreen() {
#ifdef _WIN32
	return system("cls");
#else
	return system("clear");
#endif
}

void gFunctions::gotoxy(int x, int y)
{
    COORD coord = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void gFunctions::clearLine(int y, int width) {
    gotoxy(0, y);
    std::cout << std::string(width, ' ');
}