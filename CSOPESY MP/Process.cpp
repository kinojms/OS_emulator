#include "Process.h"
#include <iostream>
#include <thread>
#include <random>

void Process::InstructionCode(int pid) {  
    this->pid = pid;  
    filename = "process_" + std::to_string(pid) + ".txt";  
    instructionMap = {  
        {1, [this](int x) { this->generatePrintCommands(x); }},  
        {2, [this](int x) { this->declare(x); }},  
        {3, [this](int x) { this->Add(x, x, x); }},  
        {4, [this](int x) { this->Subtract(x, x, x); }},  
        {5, [this](int x) { this->Sleep(x); }},  
        {6, [this](int x) { this->FOR(instructionMap, 1, x); }}  
    };  
}


void Process::generatePrintCommands(int count) {
    for (int i = 1; i <= count; ++i) {
        printCommands.push("Print message " + std::to_string(i));
    }
}

uint16_t Process::declare(int var) {
    uint16_t var1 = var;
    return var1;
}

int Process::Add(int var1, int var2, int var3) {
	var1 += var2 + var3;
    return var1;
}

int Process::Subtract(int var, int var2, int var3) {
    var -= var2 + var3;
    return var;
}

void Process::Sleep(int x) {
    int sleep_global = x;
}

void Process::FOR(const std::unordered_map<int, std::function<void(int)>> &instructionMap,  
                  int instructionID, int repeats) {  
    auto it = instructionMap.find(instructionID);  
    if (it == instructionMap.end()) {  
        std::cerr << "Instruction " << instructionID << " not found!\n";  
        return;  
    }  

    for (int i = 0; i < repeats; ++i) {  
        it->second(i); // Call the function with index i  
    }  
}

// fix this by making an ordered map of functions and sending it to this 

Process::Process(int pid) : pid(pid) {
    filename = "process_" + std::to_string(pid) + ".txt";
}

void Process::execute() {
    std::ofstream file(filename, std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for Process " << pid << "\n";
        return;
    }

    while (!printCommands.empty()) {
        std::string command = printCommands.front();
        printCommands.pop();

        auto now = std::chrono::system_clock::now();
        std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
        char timestamp[26];
        ctime_s(timestamp, sizeof(timestamp), &timeNow);
        std::string timestampStr(timestamp);
        timestampStr.pop_back(); // Remove trailing newline

        file << "[" << timestampStr << "] " << command << "\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Simulate workload
    }

    file.close();
    isFinished = true;



    /* attempted code to use queue instructions 
std::uniform_int_distribution<int> dist(1, 6);
std::mt19937 gen(std::random_device{}());

for (int i = 0; i < 10; ++i) {
    int randomInstruction = dist(gen);
    instructionQueue.push(randomInstruction);
    std::cout << i << std::endl;
}

// Process the queue
while (!instructionQueue.empty()) {
    int instructionID = instructionQueue.front();
    instructionQueue.pop();
    // Use the instructionMap to call the function
    auto it = instructionMap.find(instructionID);
    if (it != instructionMap.end()) {
        it->second(5); // Pass an

        std::cout << "Executing instruction " << instructionID << " for Process " << pid << "\n";
        std::ofstream file(filename, std::ios::out);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for Process " << pid << "\n";
            return;
        }
        */
}