#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "consoleLayout.h"
#include "Display.h"
#include "Process.h"
#include "Functions.h"
#include <fstream>

Display dp;
Functions fun;

bool firstRun = true;
bool running = true;


void consoleLayout::controller(std::string initializer) {
    std::string line;

    //Getfile and its variables
    std::ifstream file("config.txt");
    std::string line1;
    //config
    int num_cpu = 0;
    std::string scheduler;
    int quantum_Cycles = 0;
    int batch_Process_Freq = 0;
    int min_ins = 0;
    int max_ins = 0;
    float delay_Per_Exec = 0.0;


    dp.displayIntro();
    std::cout << "\nEnter a command: ";
    std::getline(std::cin, initializer);

    while (running) {
        if (initializer == "initialize") {
            if (firstRun) {
                std::cout << "Initialized successfully." << std::endl;
                while (std::getline(file, line1)) {
                    std::istringstream iss(line1);
                    std::string key, eq;
                    if (iss >> key >> eq) {
                        if (key == "num-cpu" && eq == "=") {
                            iss >> num_cpu;
                        } else if (key == "scheduler" && eq == "=") {
                            iss >> scheduler;
                        } else if (key == "quantum-cycles" && eq == "=") {
                            iss >> quantum_Cycles;
                        } else if (key == "batch-process-freq" && eq == "=") {
                            iss >> batch_Process_Freq;
                        } else if (key == "min-ins" && eq == "=") {
                            iss >> min_ins;
                        } else if (key == "max-ins" && eq == "=") {
                            iss >> max_ins;
                        } else if (key == "delay-per-exec" && eq == "=") {
                            iss >> delay_Per_Exec;
						}
                    }
                }
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
                
                // Scheduler commands
                if (token == "scheduler-start") {
                    fun.schedulerTest(num_cpu, scheduler, quantum_Cycles, max_ins, batch_Process_Freq);
                    continue;
                }

                if (token == "scheduler-stop") {
                    fun.schedulerStop();
                    continue;
                }

                if (token == "report-util") {
					fun.reportUtil();
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
                            fun.switchScreen(command);
                        }
                        continue;
                    }

                    if (flag == "-s") {
                        std::string command;
                        int memoryBytes = 0;
                        iss >> command >> memoryBytes; //added memory bytes
                        if (command.empty()) {
                            std::cout << "You must specify a command using 'screen -s'.\n";
                            continue;
                        }
						if (memoryBytes < 64 || memoryBytes > 65536) { //memory byte validation
                            std::cout << "Memory size must be between 64 and 65536 bytes (2^6 to 2^16).\n";
                            continue;
                        }
                        else {
                            auto proc = fun.createProcess(command, memoryBytes, min_ins, max_ins, delay_Per_Exec);
                            fun.switchScreen(command);
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
