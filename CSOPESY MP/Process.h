#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <queue>
#include <unordered_map>
#include <functional>
#include <cstdint>
#include <vector>

class Process {
private:
    std::string filename;
    std::unordered_map<std::string, uint16_t> memory;

public:
    int pid;
    std::string processName; // Add process name
    std::queue<std::string> printCommands;
    std::unordered_map<int, std::function<void(int)>> instructionMap;
    std::queue<int> instructionQueue;
    bool isFinished = false;
    std::vector<std::string> logs; // In-memory logs
    int assignedCore = -1; // -1 if not assigned
    int currentInstruction = 0; // 0-based index
    int totalInstructions = 0;

    Process(int pid, const std::string& name = ""); // Add name to constructor

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

    // Write logs to file (for report-util)
    void writeLogsToFile();
};

#endif // PROCESS_H