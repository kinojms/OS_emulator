#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "consoleLayout.h"
#include "Display.h"
#include "gFunctions.h"
#include "Functions.h"
#include "Process.h"

Process p;
gFunctions genFun;
Display dp;
Functions fun;

bool firstRun = true;
bool running = true;

void consoleLayout::controller(std::string initializer) {
    std::string line;

    dp.displayIntro();
    std::cout << "\nEnter a command: ";
    std::getline(std::cin, initializer);
    while (running == true) {

        if (initializer == "initialize") {
            //Initial display
            if (firstRun == true) {
                std::cout << "Initialized successfully." << std::endl;
                firstRun = false;
                continue;
            }


            //Enter Command
            std::cout << "\nEnter a command: ";
            std::getline(std::cin, line);

            std::istringstream iss(line);
            std::string token, flag, command;
            iss >> token >> flag >> command;


            //Command Processing
            //clear the screen
            if (token == "clear") {
                genFun.clearScreen();
                dp.displayIntro();
                continue;
            }

            //exit
            if (token == "exit") {
                break;
            }

            //Display known commands
            if (token == "command") {
                dp.displayCommands();
            }


            //check for screen
            if (token == "screen") {
                //check for -r
                if (flag == "-r") {
                    if (flag == "-r" && command.empty()) {
                        std::cout << "You must specify a process to resume using 'screen -r'." << std::endl;
                        continue;
                    }
                    else p.displayProcessInfo(command);

                }//end for -r

                //check for -s
                if (flag == "-s") {
                    if (command.empty()) {
                        std::cout << "You must specify a process to resume using 'screen -s'." << std::endl;
                    }
                    if (command == "screen") {
                        fun.screen();
                    }
                    if (command == "scheduler-test") {
                        fun.schedulerTest();
                    }
                    if (command == "scheduler-stop") {
                        fun.schedulerStop();
                    }
                    if (command == "report-util") {
                        fun.reportUtil();
                    }
                    if (command == "NivadaDummy") {
                        dp.nivadaDummy();
                    }
                    else {
                        p.createProcess(command);
                    }
                }
                if (flag != "-s" && flag != "-r") std::cout << "You must use '-r' or '-s' and input a command to continue" << std::endl;
            }
            else std::cout << "unknown command" << std::endl;//checker for screen commands
        }//checker to ensure initilize is active
        if (initializer != "initialize") {
            std::cout << "You must initialize before using other commands." << std::endl;
            std::cout << "\nEnter a command: ";
            std::getline(std::cin, initializer);
        } //while loop to keep code going
    }

}
