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

Process::Process(int pid, const std::string& name, int memorySize)
    : pid(pid), processName(name), memorySize(memorySize) {
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
    if (memory.size() >= SYMBOL_TABLE_LIMIT / 2) {
        logs.push_back("[ERROR] Symbol table limit reached. DECLARE ignored.");
        return 0;
    }
    memory[var] = std::clamp<uint16_t>(value, 0, UINT16_MAX);
    return memory[var];
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
            if (!forInstructionCountStack.empty()) {
                forInstructionCountStack.top()++;
            }
        }
    }

    int instructionsInThisFor = 0;
    if (!forInstructionCountStack.empty()) {
        instructionsInThisFor = forInstructionCountStack.top();
        forInstructionCountStack.pop();
    }
    logs.push_back("[INFO] FOR loop executed with " + std::to_string(instructionsInThisFor) + " instructions inside. Nesting level was " + std::to_string(forNestingLevel) + ".");
    forNestingLevel--;
}

uint16_t Process::READ(const std::string& var, const std::string& addressHex) {
    uint32_t addr;
    try {
        addr = std::stoul(addressHex, nullptr, 16);
    }
    catch (...) {
        logs.push_back("[ERROR] Invalid memory address format: " + addressHex);
        isFinished = true;
        return 0;
    }

    // Ensure address is within process memory size (each address maps to 1 byte)
    if (addr + 1 >= memorySize) { // +1 since uint16 is 2 bytes
        logs.push_back("[ACCESS VIOLATION] READ from address " + addressHex +
                       " exceeds memory size " + std::to_string(memorySize));
        isFinished = true;
        writeLogsToFile();
        return 0;
    }

    uint16_t value = emulatedMemory.count(addr) ? emulatedMemory[addr] : 0;
    DECLARE(var, value);
    logs.push_back("[INFO] READ from " + addressHex + ": " + std::to_string(value));
    return value;
}


void Process::WRITE(const std::string& addressHex, uint16_t value) {
    uint32_t addr;
    try {
        addr = std::stoul(addressHex, nullptr, 16);
    }
    catch (...) {
        logs.push_back("[ERROR] Invalid memory address format: " + addressHex);
        isFinished = true;
        writeLogsToFile();
        return;
    }

    // Ensure write does not exceed memory bounds (2 bytes for uint16_t)
    if (addr + 1 >= memorySize) {
        logs.push_back("[ACCESS VIOLATION] WRITE to address " + addressHex +
            " exceeds memory size " + std::to_string(memorySize));
        isFinished = true;
        return;
    }

    emulatedMemory[addr] = std::clamp<uint16_t>(value, 0, UINT16_MAX);
    logs.push_back("[INFO] WRITE " + std::to_string(value) + " to " + addressHex);
}

std::string Process::getRandomAddress() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dist(0, memorySize - 2); // -2 to allow room for uint16
    std::stringstream ss;
    ss << "0x" << std::hex << dist(gen);
    return ss.str();
}

void Process::InstructionCode(int pid) {
    this->pid = pid;
    filename = "process_" + std::to_string(pid) + ".txt";

    instructionMap = {
        {1, [this](int) {
            std::string msg = "Hello world from process_" + std::to_string(this->pid) + "!";
            PRINT(msg);
        }},
        {2, [this](int) {
            uint16_t randX = 1 + (rand() % 100);
            DECLARE("x", randX);
            PRINT("Declared variable 'x' with random value: " + std::to_string(randX));
        }},
        {3, [this](int) {
            if (memory.find("x") == memory.end()) {
                uint16_t randX = 1 + (rand() % 100);
                DECLARE("x", randX);
                PRINT("x was undeclared. Assigned random x = " + std::to_string(randX));
            }
            uint16_t randZ = 1 + (rand() % 100);
            DECLARE("z", randZ);
            ADD("y", "x", "z");
            uint16_t resultY = memory["y"];
            PRINT("y = x + z = " + std::to_string(resultY));
        }},
        {4, [this](int) {
            uint16_t resultZ = SUBTRACT("z", "y", "x");
            PRINT("z = y - x = " + std::to_string(resultZ));
        }},
        {5, [this](int) {
            PRINT("Sleeping for 2 ticks...");
            SLEEP(2);
        }},
        {6, [this](int) {
            static thread_local int for6_count = 0;
            int randomID = 1 + (rand() % 6);
            if (randomID == 6 && for6_count >= 3) {
                PRINT("Max recursive FOR(6) use reached. Skipping.");
                return;
            }
            if (randomID == 6) ++for6_count;
            int repeatCount = 1 + (rand() % 3);
            PRINT("FOR loop repeating instruction " + std::to_string(randomID) + " " + std::to_string(repeatCount) + " times.");
            FOR(instructionMap, randomID, repeatCount);
            if (randomID == 6) --for6_count;
        }},
        {7, [this](int) {
            std::string address = getRandomAddress();
            WRITE(address, 42);
        }},
        {8, [this](int) {
            std::string address = getRandomAddress();
            READ("my_var", address);
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
    if (isFinished) {
        writeLogsToFile();
    }
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
        writeLogsToFile();
    }
}

void Process::writeLogsToFile() {
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

void Process::loadCustomInstructions(const std::vector <std::string>& customInstructions) {
    instructionsQueue = std::queue<int>();
    totalInstructions = static_cast<int>(customInstructions.size());
    currentInstruction = 0;
    for (const auto& instr : customInstructions) {
        // Parse each instruction and bind to a lambda that wraps actual behavior
        if (instr.starts_with("DECLARE ")) {
            std::istringstream iss(instr);
            std::string cmd, var;
            int value;
            iss >> cmd >> var >> value;
            instructionMap[9000 + currentInstruction] = [this, var, value](int) {
                DECLARE(var, static_cast<uint16_t>(value));
                };
        }
        else if (instr.starts_with("ADD ")) {
            std::istringstream iss(instr);
            std::string cmd, dest, src1, src2;
            iss >> cmd >> dest >> src1 >> src2;
            instructionMap[9000 + currentInstruction] = [this, dest, src1, src2](int) {
                ADD(dest, src1, src2);
                };
        }
        else if (instr.starts_with("WRITE ")) {
            std::istringstream iss(instr);
            std::string cmd, addr, var;
            iss >> cmd >> addr >> var;
            instructionMap[9000 + currentInstruction] = [this, addr, var](int) {
                WRITE(addr, memory.count(var) ? memory[var] : 0);
                };
        }
        else if (instr.starts_with("READ ")) {
            std::istringstream iss(instr);
            std::string cmd, var, addr;
            iss >> cmd >> var >> addr;
            instructionMap[9000 + currentInstruction] = [this, var, addr](int) {
                READ(var, addr);
                };
        }
        else if (instr.starts_with("PRINT(")) {
            size_t start = instr.find("\"") + 1;
            size_t end = instr.rfind("\"");
            std::string content = (start < end) ? instr.substr(start, end - start) : "Invalid";

            instructionMap[9000 + currentInstruction] = [this, content](int) {
                std::string expanded = content;
                for (const auto& [var, val] : memory) {
                    std::string placeholder = var;
                    size_t pos = expanded.find(placeholder);
                    if (pos != std::string::npos) {
                        expanded.replace(pos, placeholder.length(), std::to_string(val));
                    }
                }
                PRINT(expanded);
                };
        }
        else {
            logs.push_back("[ERROR] Unknown instruction: " + instr);
        }

        instructionQueue.push(9000 + currentInstruction);
        currentInstruction++;
    }
}
