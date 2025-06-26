#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <queue>
#include <mutex>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>

#include <queue> // Add this include for std::queue
#include <unordered_map>
#include <functional>

class Process {
public:
    int pid;
    std::string filename;
    std::queue<std::string> printCommands;
    std::queue<int> instructionQueue; // Add this field to define instructionQueue
    bool isFinished = false;
    std::mutex fileMutex;
    std::unordered_map<int, std::function<void(int)>> instructionMap;

    Process(int pid);

    void generatePrintCommands(int count);
    uint16_t declare(int var);
    int Add(int var1, int var2, int var3);
    int Subtract(int var, int var2, int var3);
    void Sleep(int x);
    void FOR(const std::unordered_map<int, std::function<void(int)>>& instructionMap, int instructionID, int repeats);
    void execute(int count);
    void InstructionCode(int pid);
};
