#include "CommandManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

CommandManager::CommandManager(std::shared_ptr<Functions> functions, std::shared_ptr<Display> display)
    : fun(functions), dp(display), initialized(false) {}

void CommandManager::initializeFromConfig(const std::string& configFile) {
    std::ifstream file(configFile);
    std::string line1;
    while (std::getline(file, line1)) {
        std::istringstream iss(line1);
        std::string key, eq;
        if (iss >> key >> eq) {
            if (key == "num-cpu" && eq == "=") iss >> num_cpu;
            else if (key == "scheduler" && eq == "=") iss >> scheduler;
            else if (key == "quantum-cycles" && eq == "=") iss >> quantum_Cycles;
            else if (key == "batch-process-freq" && eq == "=") iss >> batch_Process_Freq;
            else if (key == "min-ins" && eq == "=") iss >> min_ins;
            else if (key == "max-ins" && eq == "=") iss >> max_ins;
            else if (key == "delay-per-exec" && eq == "=") iss >> delay_Per_Exec;
            else if (key == "max-overall-mem" && eq == "=") iss >> max_overall_mem;
            else if (key == "mem-per-frame" && eq == "=") iss >> mem_per_frame;
            else if (key == "mem-per-proc" && eq == "=") iss >> mem_per_proc;
        }
    }
    if (max_overall_mem > 0 && mem_per_proc > 0 && mem_per_frame > 0) {
        fun->initializeMemoryManager(max_overall_mem, mem_per_proc, mem_per_frame);
    }
    initialized = true;
}

bool CommandManager::isInitialized() const {
    return initialized;
}

void CommandManager::setInitialized(bool value) {
    initialized = value;
}

void CommandManager::execute(const std::string& input) {
    std::istringstream iss(input);
    std::string token, flag, command;
    iss >> token >> flag >> command;

    if (token == "clear") {
        system("clear");
        dp->displayIntro();
        return;
    }
    if (token == "exit") {
        exit(0);
    }
    if (token == "command") {
        dp->displayCommands();
        return;
    }
    if (token == "scheduler-start") {
        fun->schedulerTest(num_cpu, scheduler, quantum_Cycles, min_ins, max_ins, batch_Process_Freq, delay_Per_Exec);
        return;
    }
    if (token == "scheduler-stop") {
        fun->schedulerStop();
        return;
    }
    if (token == "report-util") {
        handleReportUtil();
        return;
    }
    if (token == "screen") {
        handleScreenCommand(flag, command);
        return;
    }
    if (token == "process-smi") {
        handleProcessSmi(command);
        return;
    }
    if (token == "vmstat") {
        handleVmstat();
        return;
    }
    handleUnknownCommand();
}

void CommandManager::handleScreenCommand(const std::string& flag, const std::string& command) {
    if (flag != "-r" && flag != "-s" && flag != "-ls") {
        std::cout << "You must use '-r' or '-s' and input a command to continue.\n";
        return;
    }
    if (flag == "-r") {
        if (command.empty()) {
            std::cout << "You must specify a process to resume using 'screen -r'.\n";
        } else {
            fun->switchScreen(command);
        }
        return;
    }
    if (flag == "-s") {
        if (command.empty()) {
            std::cout << "You must specify a command using 'screen -s'.\n";
            return;
        } else {
            auto proc = fun->createProcess(command, min_ins, max_ins, delay_Per_Exec);
            fun->switchScreen(command);
        }
        return;
    }
    if (flag == "-ls") {
        std::cout << "Listing all processes...\n";
        fun->screen();
        return;
    }
}

void CommandManager::handleSchedulerCommand(const std::string& token) {
    // Extend for more scheduler commands if needed
}

void CommandManager::handleReportUtil() {
    fun->reportUtil();
}

void CommandManager::handleProcessSmi(const std::string& processName) {
    // Placeholder for process-smi implementation
    std::cout << "process-smi for " << processName << " (to be implemented)\n";
}

void CommandManager::handleVmstat() {
    // Placeholder for vmstat implementation
    std::cout << "vmstat (to be implemented)\n";
}

void CommandManager::handleUnknownCommand() {
    std::cout << "Unknown command.\n";
}
