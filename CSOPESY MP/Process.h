#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <queue>
#include <unordered_map>
#include <functional>
#include <cstdint>
#include <vector>
#include <stack> // For FOR loop tracking

class Process {
private:
    std::string filename;
    std::unordered_map<std::string, uint16_t> memory;
    std::unordered_map<uint32_t, uint16_t> emulatedMemory; // New: Simulated memory
    static constexpr int SYMBOL_TABLE_LIMIT = 64; // New: Symbol table size in bytes
    int memorySize; // New: total memory allocated to process

    // FOR loop tracking
    std::stack<int> forInstructionCountStack; // Tracks instruction count per FOR
    int forNestingLevel = 0; // Current FOR nesting level
    static constexpr int MAX_FOR_NESTING = 3;
    std::queue<int> instructionsQueue;

public:
    int pid;
    std::string processName;
    std::queue<std::string> printCommands;
    std::unordered_map<int, std::function<void(int)>> instructionMap;
    std::queue<int> instructionQueue;
    bool isFinished = false;
    std::vector<std::string> logs;
    int assignedCore = -1;
    int currentInstruction = 0;
    int totalInstructions = 0;
    bool memoryAllocated = false;

    // Updated constructor to require memory size
    Process(int pid, const std::string& name = "", int memorySize = 65536);

    void generatePrintCommands(int count);
    void InstructionCode(int pid);
    void execute();
    void executeTimeSlice(int instructionLimit);

    // Instruction implementations
    void PRINT(const std::string& msg);
    uint16_t DECLARE(const std::string& var, uint16_t value);
    uint16_t ADD(const std::string& dest, const std::string& src1, const std::string& src2);
    uint16_t SUBTRACT(const std::string& dest, const std::string& src1, const std::string& src2);
    void SLEEP(int ticks);
    void FOR(const std::unordered_map<int, std::function<void(int)>>& instructions, int instructionID, int repeats);

    // New: memory access
    uint16_t READ(const std::string& var, const std::string& addressHex);
    void WRITE(const std::string& addressHex, uint16_t value);

    std::string getRandomAddress() const;

    void writeLogsToFile();

    void setMemoryAllocated(bool allocated);
    bool isMemoryAllocated() const;
    void loadCustomInstructions(const std::vector<std::string>& customInstructions);
};

#endif // PROCESS_H
