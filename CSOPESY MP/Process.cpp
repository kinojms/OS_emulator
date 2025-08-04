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
#include <regex>

std::string Process::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);

    char buffer[26] = { 0 };  // Safe fixed-size buffer
    errno_t err = ctime_s(buffer, sizeof(buffer), &timeNow);
    if (err != 0) {
        return "[Timestamp Error]";
    }

    std::string timestampStr(buffer);
    if (!timestampStr.empty() && timestampStr.back() == '\n') {
        timestampStr.pop_back();  // Remove newline
    }

    return "[" + timestampStr + "]";
}

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
    logs.push_back(getCurrentTimestamp() + " WRITE " + std::to_string(value) + " to " + addressHex);
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
    logs.push_back(getCurrentTimestamp() + " WRITE " + std::to_string(value) + " to " + addressHex);
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
            if (hasCustomInstructions) return;
            uint16_t randX = 1 + (rand() % 100);
            DECLARE("x", randX);
            PRINT("Declared variable 'x' with random value: " + std::to_string(randX));
        }},
        {3, [this](int) {
            if (hasCustomInstructions) return;
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
            if (hasCustomInstructions) return;
            uint16_t resultZ = SUBTRACT("z", "y", "x");
            PRINT("z = y - x = " + std::to_string(resultZ));
        }},
        {5, [this](int) {
            if (hasCustomInstructions) return;
            PRINT("Sleeping for 2 ticks...");
            SLEEP(2);
        }},
        {6, [this](int) {
            if (hasCustomInstructions) return;
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
            if (hasCustomInstructions) return;
            std::string address = getRandomAddress();
            WRITE(address, 42);
        }},
        {8, [this](int) {
            if (hasCustomInstructions) return;
            std::string address = getRandomAddress();
            READ("my_var", address);
        }}
    };
}

void Process::runInstructions(int instructionLimit) {
    if (totalInstructions == 0) totalInstructions = static_cast<int>(instructionQueue.size());

    int executed = 0;
    int customArgIndex = 0;

    // If instructionLimit <= 0, run all instructions (non-preemptive)
    while (!instructionQueue.empty() && (instructionLimit <= 0 || executed < instructionLimit)) {
        int instructionID = instructionQueue.front();
        instructionQueue.pop();
        currentInstruction++;
        executed++;

        if (instructionMap.count(instructionID)) {
            instructionMap[instructionID](0);
        }
        else {
            if (customArgIndex >= customArgs.size()) {
                logs.push_back("[ERROR] Custom argument index out of bounds.");
                break;
            }
            const std::vector<std::string>& args = customArgs[customArgIndex];
            switch (instructionID) {
            case INST_DECLARE: {
                const std::string& var = args[0];
                uint16_t val = static_cast<uint16_t>(std::stoi(args[1]));
                DECLARE(var, val);
                logs.push_back(getCurrentTimestamp() + " DECLARE " + var + " = " + std::to_string(val));
                break;
            }
            case INST_ADD: {
                const std::string& dest = args[0];
                const std::string& src1 = args[1];
                const std::string& src2 = args[2];
                uint16_t result = ADD(dest, src1, src2);
                logs.push_back(getCurrentTimestamp() + " ADD " + dest + " = " + src1 + " + " + src2 + " = " + std::to_string(result));
                break;
            }
            case INST_SUBTRACT: {
                const std::string& dest = args[0];
                const std::string& src1 = args[1];
                const std::string& src2 = args[2];
                uint16_t result = SUBTRACT(dest, src1, src2);
                logs.push_back(getCurrentTimestamp() + " SUBTRACT " + dest + " = " + src1 + " - " + src2 + " = " + std::to_string(result));
                break;
            }
            case INST_WRITE: {
                const std::string& addr = args[0];
                const std::string& var = args[1];
                uint16_t val = memory.count(var) ? memory[var] : 0;
                WRITE(addr, val);
                break;
            }
            case INST_READ: {
                const std::string& var = args[0];
                const std::string& addr = args[1];
                READ(var, addr);
                break;
            }
            case INST_PRINT: {
                std::string finalMessage;
                for (const auto& part : args) {
                    if (memory.count(part)) {
                        finalMessage += std::to_string(memory[part]);
                    }
                    else {
                        finalMessage += part;
                    }
                }
                PRINT(finalMessage);
                break;
            }
            default:
                logs.push_back("[ERROR] Unknown instruction ID: " + std::to_string(instructionID));
                break;
            }
            customArgIndex++;
        }

        // Flush printCommands to logs with timestamps
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

    if (instructionQueue.empty()) {
        isFinished = true;
        writeLogsToFile();
    }
    else if (!logs.empty() && instructionLimit > 0) {
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
    // std::cout << "Wrote logs for " << processName << " to " << logPath << std::endl;
    file.close();
}

void Process::setMemoryAllocated(bool allocated) {
    memoryAllocated = allocated;
}

bool Process::isMemoryAllocated() const {
    return memoryAllocated;
}

void Process::loadCustomInstructions(const std::vector<std::string>& customInstructions) {
    hasCustomInstructions = true;
    instructionQueue = std::queue<int>();
    customArgs.clear();
    totalInstructions = static_cast<int>(customInstructions.size());
    currentInstruction = 0;

    for (const auto& instr : customInstructions) {
        std::istringstream iss(instr);
        std::string cmd;
        iss >> cmd;

        std::vector<std::string> args;

        if (cmd == "DECLARE") {
            std::string var;
            int value;
            iss >> var >> value;
            args = { var, std::to_string(value) };
            instructionQueue.push(INST_DECLARE);
        }
        else if (cmd == "ADD") {
            std::string dest, src1, src2;
            iss >> dest >> src1 >> src2;
            args = { dest, src1, src2 };
            instructionQueue.push(INST_ADD);
        }
        else if (cmd == "SUBTRACT") {
            std::string dest, src1, src2;
            iss >> dest >> src1 >> src2;
            args = { dest, src1, src2 };
            instructionQueue.push(INST_SUBTRACT);
        }
        else if (cmd == "WRITE") {
            std::string addr, var;
            iss >> addr >> var;
            args = { addr, var };
            instructionQueue.push(INST_WRITE);
        }
        else if (cmd == "READ") {
            std::string var, addr;
            iss >> var >> addr;
            args = { var, addr };
            instructionQueue.push(INST_READ);
        }
        else if (instr.find("PRINT(") == 0) {
            // Better PRINT parser that supports + concatenation
            size_t firstParen = instr.find("(");
            size_t lastParen = instr.rfind(")");

            std::string content = (firstParen != std::string::npos && lastParen != std::string::npos && lastParen > firstParen)
                ? instr.substr(firstParen + 1, lastParen - firstParen - 1)
                : "";

            std::vector<std::string> parts;
            std::istringstream partStream(content);
            std::string token;

            while (std::getline(partStream, token, '+')) {
                // Trim whitespace
                token.erase(0, token.find_first_not_of(" \t"));
                token.erase(token.find_last_not_of(" \t") + 1);

                // Remove quotes if it's a string literal
                if (!token.empty() && token.front() == '"' && token.back() == '"') {
                    token = token.substr(1, token.size() - 2);
                }

                parts.push_back(token);
            }

            customArgs.push_back(parts);
            instructionQueue.push(INST_PRINT);
        }
        else {
            logs.push_back("[ERROR] Unknown instruction: " + instr);
            continue;
        }

        if (cmd != "PRINT") {
            customArgs.push_back(args);
        }
    }
}
