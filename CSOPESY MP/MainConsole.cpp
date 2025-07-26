#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "consoleLayout.h"
#include "Display.h"
#include "Process.h"
#include "Functions.h"
#include <fstream>
#include "CommandManager.h"

Display dp;
Functions fun;
std::shared_ptr<Display> dp_ptr = std::make_shared<Display>();
std::shared_ptr<Functions> fun_ptr = std::make_shared<Functions>();
CommandManager cmdManager(fun_ptr, dp_ptr);

bool firstRun = true;
bool running = true;

void consoleLayout::controller(std::string initializer) {
    std::string line;
    dp.displayIntro();
    std::cout << "\nEnter a command: ";
    std::getline(std::cin, initializer);

    while (running) {
        if (initializer == "initialize") {
            if (firstRun) {
                std::cout << "Initialized successfully." << std::endl;
                cmdManager.initializeFromConfig("config.txt");
                firstRun = false;
            }
            while (running) {
                std::cout << "\nEnter a command: ";
                std::getline(std::cin, line);
                cmdManager.execute(line);
            }
        } else {
            std::cout << "You must initialize before using other commands.\n";
            std::cout << "\nEnter a command: ";
            std::getline(std::cin, initializer);
        }
    }
}