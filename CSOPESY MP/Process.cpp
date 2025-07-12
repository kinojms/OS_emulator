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
    // Enforce max FOR nesting using member variable
    if (forNestingLevel >= MAX_FOR_NESTING) {
        logs.push_back("[ERROR] Maximum FOR nesting level (" + std::to_string(MAX_FOR_NESTING) + ") reached. Skipping FOR.");
        return;
    }
    forNestingLevel++;
    forInstructionCountStack.push(0);

    for (int i = 0; i < repeats; ++i) {
        auto it = instructions.find(instructionID);
        if (it != instructions.end()) {
            it->second(0);
            // Count this instruction inside the FOR
            if (!forInstructionCountStack.empty()) {
                forInstructionCountStack.top()++;
            }
        }
    }

    // After FOR loop ends
    int instructionsInThisFor = 0;
    if (!forInstructionCountStack.empty()) {
        instructionsInThisFor = forInstructionCountStack.top();
        forInstructionCountStack.pop();
    }
    logs.push_back("[INFO] FOR loop executed with " + std::to_string(instructionsInThisFor) + " instructions inside. Nesting level was " + std::to_string(forNestingLevel) + ".");
    forNestingLevel--;
}

void Process::InstructionCode(int pid) {
    this->pid = pid;
    filename = "process_" + std::to_string(pid) + ".txt";

    instructionMap = {
        {1, [this](int) {
            // Default message unless overridden in a test case
            std::string msg = "Hello world from process_" + std::to_string(this->pid) + "!";

            // Example of test-case-specified print with variable
            bool customTestCase = false; // Set to true in test scenario if needed

            if (customTestCase) {
                std::string base = "Value from: ";
                std::string var = "x";  // Variable to print
                uint16_t value = memory.count(var) ? memory[var] : 0;
                msg = base + std::to_string(value);
            }

            PRINT(msg);
        }},
        {2, [this](int) {
            uint16_t randX = 1 + (rand() % 100);
            DECLARE("x", randX);
            PRINT("Declared variable 'x' with random value: " + std::to_string(randX));
        }},
        {3, [this](int) {
            // Ensure 'x' is declared
            if (memory.find("x") == memory.end()) {
                uint16_t randX = 1 + (rand() % 100);
                DECLARE("x", randX);
                PRINT("x was undeclared. Assigned random x = " + std::to_string(randX));
            }

            // Always assign a random value to 'z'
            uint16_t randZ = 1 + (rand() % 100);
            DECLARE("z", randZ);


            // Perform y = x + z
            ADD("y", "x", "z");

            // Show result
            uint16_t resultY = memory["y"];
            PRINT("y = x + z = " + std::to_string(resultY));
        }},
        {4, [this](int) {
            // Check if y and x exist; fallback handled inside SUBTRACT already
            uint16_t resultZ = SUBTRACT("z", "y", "x");

            // Print result
            PRINT("z = y - x = " + std::to_string(resultZ));
        }},
        {5, [this](int) {
            PRINT("Sleeping for 2 ticks...");
            SLEEP(2);
        }},
        {6, [this](int) {
            static thread_local int for6_count = 0; // local to thread, persists across calls

            int randomID = 1 + (rand() % 6); // random number from 1 to 6

            // Limit recursive FOR instruction (ID 6) to max 3 times
                if (randomID == 6) {
                if (for6_count >= 3) {
                    PRINT("Max recursive FOR(6) use reached. Skipping.");
                    return;
                    }
                ++for6_count;
                }

                int repeatCount = 1 + (rand() % 3);
                PRINT("FOR loop repeating instruction " + std::to_string(randomID) + " " + std::to_string(repeatCount) + " times.");
                FOR(instructionMap, randomID, repeatCount);

                // Decrease counter after FOR completes
                    if (randomID == 6) {
                    --for6_count;
                    }
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

void Process::setMemoryAllocated(bool allocated) {
    memoryAllocated = allocated;
}

bool Process::isMemoryAllocated() const {
    return memoryAllocated;
}