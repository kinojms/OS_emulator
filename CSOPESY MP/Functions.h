#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <string>
#include "CPUCore.h"
#include "Scheduler.h"
#include "Process.h"

class Functions {
private:
    std::vector<std::shared_ptr<Process>> allProcesses;
    std::shared_ptr<Scheduler> scheduler;
    std::thread schedulerThread;
    std::atomic<bool> schedulerRunning = false;

public:
    void FCFS(int num_cpu, int quantum_Cycles, int max_ins);
    void RR(int num_cpu, int quantum_Cycles, int max_ins);
    void schedulerTest(int num_cpu, const std::string& schedulerType, int quantum_Cycles, int max_ins);
    void schedulerStop();
    void screen();
    void reportUtil();
};

#endif