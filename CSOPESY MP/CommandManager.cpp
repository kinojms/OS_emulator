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
    // Split input into tokens
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string tok;
    while (iss >> tok) {
        tokens.push_back(tok);
    }
    if (tokens.empty()) {
        handleUnknownCommand();
        return;
    }
    std::string token = tokens[0];
    std::string flag = tokens.size() > 1 ? tokens[1] : "";
    // For screen commands, pass all remaining tokens as arguments
    if (token == "screen") {
        std::vector<std::string> args(tokens.begin() + 2, tokens.end());
        handleScreenCommand(flag, args);
        return;
    }
    // Legacy handling for other commands
    std::string command = tokens.size() > 2 ? tokens[2] : "";
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

void CommandManager::handleScreenCommand(const std::string& flag, const std::vector<std::string>& args) {
    if (flag != "-r" && flag != "-s" && flag != "-ls" && flag != "-c") {
        std::cout << "You must use '-r', '-s', '-c', or '-ls' and input a command to continue.\n";
        return;
    }
    if (flag == "-r") {
        if (args.empty()) {
            std::cout << "You must specify a process to resume using 'screen -r'.\n";
        } else {
            fun->switchScreen(args[0]);
        }
        return;
    }
    if (flag == "-s") {
        if (args.size() != 2) {
            std::cout << "You must specify a process name and memory size using 'screen -s <name> <size>'.\n";
            return;
        }
        std::string name = args[0];
        std::string memSizeStr = args[1];
        int memSize = 0;
        try {
            memSize = std::stoi(memSizeStr);
        } catch (...) {
            std::cout << "invalid memory allocation\n";
            return;
        }
        if (memSize < 64 || memSize > 65536 || (memSize & (memSize - 1)) != 0) {
            std::cout << "invalid memory allocation\n";
            return;
        }
        auto proc = fun->createProcess(name, memSize);
        if (proc) {
            fun->switchScreen(name);
        }
        return;
    }
    if (flag == "-c") {
        if (args.size() < 3) {
            std::cout << "invalid command\n";
            return;
        }
        std::string name = args[0];
        std::string memSizeStr = args[1];
        std::string instructions;
        for (size_t i = 2; i < args.size(); ++i) {
            if (i > 2) instructions += " ";
            instructions += args[i];
        }
        int memSize = 0;
        try {
            memSize = std::stoi(memSizeStr);
        } catch (...) {
            std::cout << "invalid command\n";
            return;
        }
        size_t count = std::count(instructions.begin(), instructions.end(), ';');
        if (count < 1 || count > 50) {
            std::cout << "invalid command\n";
            return;
        }
        auto proc = fun->createProcess(name, memSize, instructions);
        if (proc) {
            fun->switchScreen(name);
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
