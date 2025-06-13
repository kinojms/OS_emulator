#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <queue>
#include <mutex>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>

class Process {
public:
    int pid;
    std::string filename;
    std::queue<std::string> printCommands;
    bool isFinished = false;
    std::mutex fileMutex;

    Process(int pid);

    void generatePrintCommands(int count);
    void execute();
};

#endif
