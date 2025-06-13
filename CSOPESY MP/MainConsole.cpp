#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "consoleLayout.h"
#include "Display.h"
#include "Process.h"
#include "Functions.h"

Display dp;
Functions fun;

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
				// fun.schedulerTest();
                firstRun = false;
            }

            while (running) {
                std::cout << "\nEnter a command: ";
                std::getline(std::cin, line);

                std::istringstream iss(line);
                std::string token, flag, command;
                iss >> token >> flag >> command;

                // Basic commands
                if (token == "clear") {
                    system("clear");
                    dp.displayIntro();
                    continue;
                }
                if (token == "exit") {
                    running = false;
                    break;
                }
                if (token == "command") {
                    dp.displayCommands();
                    continue;
                }
                

                // Screen commands
                if (token == "screen") {
                    if (flag != "-r" && flag != "-s" && flag != "-ls") {
                        std::cout << "You must use '-r' or '-s' and input a command to continue.\n";
                        continue;
                    }

                    if (flag == "-r") {
                        if (command.empty()) {
                            std::cout << "You must specify a process to resume using 'screen -r'.\n";
                        }
                        else {
                            // p.displayProcessInfo(command); // or implement as needed
							std::cout << "Reading process...";
                        }
                        continue;
                    }

                    if (flag == "-s") {
                        if (command.empty()) {
                            std::cout << "You must specify a command using 'screen -s'.\n";
                            continue;
                        }
                        // Scheduler commands
                        if (command == "scheduler-start") {
                            fun.schedulerTest();
                            continue;
                        }
                        else if (command     == "scheduler-stop") {
                            fun.schedulerStop();
                            continue;
                        }
                        else if (command == "report-util") {
                            fun.reportUtil();
                        }
                        else {
                            std::cout << "This command is not supported.\n";
                        }
                        continue;
                    }

                    if (flag == "-ls") {
						std::cout << "Listing all processes...\n";
						fun.screen(); // Assuming this lists all processes
						continue;
                    }

                }

                // Unknown command
                std::cout << "Unknown command.\n";
            }

        }
        else {
            std::cout << "You must initialize before using other commands.\n";
            std::cout << "\nEnter a command: ";
            std::getline(std::cin, initializer);
        }
    }
}
