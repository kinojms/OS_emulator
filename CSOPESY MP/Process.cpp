#include "Process.h"
#include <iostream>
#include <thread>

Process::Process(int pid) : pid(pid) {
    filename = "process_" + std::to_string(pid) + ".txt";
}

void Process::generatePrintCommands(int count) {
    for (int i = 1; i <= count; ++i) {
        printCommands.push("Print message " + std::to_string(i));
    }
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
}
