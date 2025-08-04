#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "consoleLayout.h"
#include "Display.h"
#include "Process.h"
#include "Functions.h"
#include <fstream>
#include <regex>

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
    int max_overall_mem = 0;
    int mem_per_frame = 0;
    int mem_per_proc = 0;

    std::string tmp, processName, instructionBlock;
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
                        }
                        else if (key == "scheduler" && eq == "=") {
                            iss >> scheduler;
                        }
                        else if (key == "quantum-cycles" && eq == "=") {
                            iss >> quantum_Cycles;
                        }
                        else if (key == "batch-process-freq" && eq == "=") {
                            iss >> batch_Process_Freq;
                        }
                        else if (key == "min-ins" && eq == "=") {
                            iss >> min_ins;
                        }
                        else if (key == "max-ins" && eq == "=") {
                            iss >> max_ins;
                        }
                        else if (key == "delay-per-exec" && eq == "=") {
                            iss >> delay_Per_Exec;
                        }
                        else if (key == "max-overall-mem" && eq == "=") {
                            iss >> max_overall_mem;
                        }
                        else if (key == "mem-per-frame" && eq == "=") {
                            iss >> mem_per_frame;
                        }
                        else if (key == "mem-per-proc" && eq == "=") {
                            iss >> mem_per_proc;
                        }
                    }
                }
                // Initialize memory manager
                if (max_overall_mem > 0 && mem_per_proc > 0 && mem_per_frame > 0) {
                    fun.initializeMemoryManager(max_overall_mem, mem_per_proc, mem_per_frame);
                }
                firstRun = false;

				// Initialize scheduler
                if (num_cpu > 0 && quantum_Cycles > 0 && !scheduler.empty()) {
                    fun.runScheduler(num_cpu, quantum_Cycles, min_ins, max_ins, batch_Process_Freq, delay_Per_Exec, scheduler);
                }
                else {
                    std::cout << "Invalid scheduler configuration. Please check config.txt.\n";
                }
            }

            while (running) {
                std::cout << "\nEnter a command: ";
                std::getline(std::cin, line);

                std::istringstream iss(line);
                std::string token, flag, command, size_str, command_String;
                iss >> token >> flag >> command >> size_str >> command_String;


                // Basic commands
                if (token == "clear") {
                    system("cls");
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
                    //fun.schedulerTest(num_cpu, scheduler, quantum_Cycles, min_ins, max_ins, batch_Process_Freq, delay_Per_Exec);
					fun.startProcessGenerator(min_ins, max_ins, batch_Process_Freq);
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
                    if (flag != "-r" && flag != "-s" && flag != "-ls" && flag != "-c") {
                        std::cout << "You must use '-r' or '-s' or e-cf and input a command to continue.\n";
                        continue;
                    }

                    if (flag == "-r") {
                        if (command.empty()) {
                            std::cout << "You must specify a process to read using 'screen -r'.\n";
                        }
                        else {
                            fun.switchScreen(command);
                        }
                        continue;
                    }

                    if (flag == "-s") {
                        if (command.empty() || size_str.empty()) {
                            std::cout << "You must specify both a process name and memory size using 'screen -s <name> <size>'.\n";
                            continue;
                        }

                        int size = 0;
                        try {
                            size = std::stoi(size_str);
                        }
                        catch (...) {
                            std::cout << "Invalid memory size input. Must be a number.\n";
                            continue;
                        }

                        // Memory must be between 64 bytes and 262144 (2^6 to 2^18), and power of 2
                        if (size <= 64 && size >= 262144 && (size & (size - 1)) != 0) {
                            std::cout << "Invalid memory allocation: must be a power of 2 between 64 and 262144 bytes.\n";
                            continue;
                        }

                        else {
                            auto proc = fun.createProcess(command, min_ins, max_ins, delay_Per_Exec, size);
                            //fun.switchScreen(command);
                        }
                        continue;
                    }

                    if (flag == "-ls") {
                        std::cout << "Listing all processes...\n";
						fun.writeScreenReport(std::cout);
                        continue;
                    }

                    if (flag == "-c") {
                        // Check for valid memory size
                        int size = 0;
                        try {
                            size = std::stoi(size_str);
                        }
                        catch (...) {
                            std::cout << "Invalid memory size input. Must be a number.\n";
                            continue;
                        }

                        if (command.empty() || size_str.empty()) {
                            std::cout << "You must specify a process name and memory size using 'screen -c <name> <size> \"<commands>\"'.\n";
                            continue;
                        }

                        // Extract quoted instruction string from the entire input line
                        std::size_t firstQuote = line.find('"');
                        std::size_t lastQuote = line.rfind('"');
                        if (firstQuote == std::string::npos || lastQuote == std::string::npos || lastQuote <= firstQuote) {
                            std::cout << "Invalid command string. Instructions must be enclosed in double quotes.\n";
                            continue;
                        }

                        std::string instructionStr = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);

                        // Split by semicolon
                        std::vector<std::string> instructions;
                        std::stringstream ss(instructionStr);
                        std::string item;

                        while (std::getline(ss, item, ';')) {
                            std::string trimmed = std::regex_replace(item, std::regex("^\\s+|\\s+$"), "");
                            if (!trimmed.empty()) instructions.push_back(trimmed);
                        }

                        if (instructions.size() == 0 || instructions.size() > 50) {
                            std::cout << "Invalid command: instruction count must be between 1 and 50.\n";
                            continue;
                        }

                        // Create or reuse process
                        auto process = fun.getProcessByName(command);
                        if (!process) {
                            process = fun.createProcess(command, 0, 0, 0, size);
                            if (!process) {
                                std::cout << "Failed to create process.\n";
                                continue;
                            }
                        }

                        process->loadCustomInstructions(instructions);
                        std::cout << "Instructions loaded into " << command << ".\n";
                        continue;
                    }


                }

                else {
                    std::cout << "Unknown command: " << token << ". Type 'command' to see available commands.\n";
					continue;
                }
            }
        }
        else {
            std::cout << "You must initialize before using other commands.\n";
            std::cout << "\nEnter a command: ";
            std::getline(std::cin, initializer);
        }
    }
}