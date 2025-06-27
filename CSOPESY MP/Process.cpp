#include "Process.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <ctime>
#include <sstream>
#include <random>
#include <algorithm>
#include <filesystem>

Process::Process(int pid, const std::string& name) : pid(pid), processName(name) {
    if (processName.empty())
        processName = "process_" + std::to_string(pid);
    filename = processName + ".txt";
}

void Process::generatePrintCommands(int count) {
    for (int i = 1; i <= count; ++i) {
        printCommands.push("Print message " + std::to_string(i));
    }
}

void Process::PRINT(const std::string& msg) {
    printCommands.push(msg);
}

uint16_t Process::DECLARE(const std::string& var, uint16_t value) {
    memory[var] = value;
    return value;
}

uint16_t Process::ADD(const std::string& dest, const std::string& src1, const std::string& src2) {
    uint16_t val1 = memory.count(src1) ? memory[src1] : 0;
    uint16_t val2 = memory.count(src2) ? memory[src2] : 0;
    uint16_t result = std::clamp<uint32_t>(val1 + val2, 0, UINT16_MAX);
    memory[dest] = result;
    return result;
}

uint16_t Process::SUBTRACT(const std::string& dest, const std::string& src1, const std::string& src2) {
    uint16_t val1 = memory.count(src1) ? memory[src1] : 0;
    uint16_t val2 = memory.count(src2) ? memory[src2] : 0;
    int32_t result = static_cast<int32_t>(val1) - static_cast<int32_t>(val2);
    result = std::clamp(result, 0, static_cast<int32_t>(UINT16_MAX));
    memory[dest] = static_cast<uint16_t>(result);
    return static_cast<uint16_t>(result);
}

void Process::SLEEP(int ticks) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ticks * 10));
}

void Process::FOR(const std::unordered_map<int, std::function<void(int)>>& instructions, int instructionID, int repeats) {
    static thread_local int depth = 0;
    if (depth >= 3) {
        printCommands.push("Max FOR nesting reached.");
        return;
    }
    ++depth;
    for (int i = 0; i < repeats; ++i) {
        auto it = instructions.find(instructionID);
        if (it != instructions.end()) {
            it->second(0);
        }
    }
    --depth;
}

void Process::InstructionCode(int pid) {
    this->pid = pid;
    filename = "process_" + std::to_string(pid) + ".txt";

    instructionMap = {
        {1, [this](int) {
            PRINT("Hello world from process_" + std::to_string(this ->pid) + "!");
        }},
        {2, [this](int) {
            DECLARE("x", 42);
            PRINT("Declared x = 42");
        }},
        {3, [this](int) {
            ADD("y", "x", "x");
            PRINT("y = x + x");
        }},
        {4, [this](int) {
            SUBTRACT("z", "y", "x");
            PRINT("z = y - x");
        }},
        {5, [this](int) {
            PRINT("Sleeping for 2 ticks...");
            SLEEP(2);
        }},
        {6, [this](int) {
            FOR(instructionMap, 1, 3);
        }}
    };
}

void Process::execute() {
    currentInstruction = 0;
    totalInstructions = static_cast<int>(instructionQueue.size());
    while (!instructionQueue.empty()) {
        int instructionID = instructionQueue.front();
        instructionQueue.pop();
        currentInstruction++;
        auto it = instructionMap.find(instructionID);
        if (it != instructionMap.end()) {
            it->second(0);
        }
        while (!printCommands.empty()) {
            std::string command = printCommands.front();
            printCommands.pop();
            auto now = std::chrono::system_clock::now();
            std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
            char timestamp[26];
            ctime_s(timestamp, sizeof(timestamp), &timeNow);
            std::string timestampStr(timestamp);
            timestampStr.pop_back();
            logs.push_back("[" + timestampStr + "] " + command);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    isFinished = true;
}

void Process::executeTimeSlice(int instructionLimit) {
    if (totalInstructions == 0) totalInstructions = static_cast<int>(instructionQueue.size());
    int executed = 0;
    while (!instructionQueue.empty() && executed < instructionLimit) {
        int instructionID = instructionQueue.front();
        instructionQueue.pop();
        currentInstruction++;
        auto it = instructionMap.find(instructionID);
        if (it != instructionMap.end()) {
            it->second(0);
        }
        while (!printCommands.empty()) {
            std::string command = printCommands.front();
            printCommands.pop();
            auto now = std::chrono::system_clock::now();
            std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
            char timestamp[26];
            ctime_s(timestamp, sizeof(timestamp), &timeNow);
            std::string timestampStr(timestamp);
            timestampStr.pop_back();
            logs.push_back("[" + timestampStr + "] " + command);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        executed++;
    }
    if (instructionQueue.empty()) {
        isFinished = true;
    }
}

void Process::writeLogsToFile() {
    // Ensure the process_logs directory exists
    std::filesystem::create_directories("process_logs");
    std::string logPath = "process_logs/" + processName + ".txt";
    std::ofstream file(logPath, std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for Process " << pid << "\n";
        return;
    }
    for (const auto& log : logs) {
        file << log << "\n";
    }
    file.close();
}