#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <queue>
#include <unordered_map>
#include <functional>
#include <cstdint>
#include <vector>
#include <stack>
#include "PagingTypes.h"

class Process {
private:
    std::string filename;
    std::unordered_map<std::string, uint16_t> symbolTable; // Symbol table for variables
    std::unordered_map<uint32_t, uint16_t> memorySpace; // Simulated memory
    std::vector<PageTableEntry> pageTable; // Page table for paging
    int memorySize = 0; // Allocated memory size in bytes
    // FOR loop tracking
    std::stack<int> forInstructionCountStack;
    int forNestingLevel = 0;
    static constexpr int MAX_FOR_NESTING = 3;
    static constexpr int SYMBOL_TABLE_BYTES = 64;
    static constexpr int MAX_VARIABLES = 32;

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

    Process(int pid, const std::string& name = "", int memSize = 0);

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
    uint16_t READ(const std::string& var, uint32_t address);
    void WRITE(uint32_t address, uint16_t value);

    // Write logs to file (for report-util)
    void writeLogsToFile();

    // Memory management
    void setMemoryAllocated(bool allocated);
    bool isMemoryAllocated() const;
    void setMemorySize(int size);
    int getMemorySize() const;
    void setPageTable(const std::vector<PageTableEntry>& pt);
    bool isValidAddress(uint32_t address) const;
    void setUserInstructions(const std::string& instructions);
};

#endif // PROCESS_H