#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include "CPUCore.h"
#include "Scheduler.h"


class Functions {
private:
    std::vector<std::shared_ptr<Process>> allProcesses;
    std::shared_ptr<Scheduler> scheduler;
    std::thread schedulerThread;
    std::atomic<bool> schedulerRunning = false;

public:
    void FCFS(int num_cpu, int qunatum_Cycles);
    void RR(int num_cpu, int qunatum_Cycles);
    void schedulerTest(int num_cpu, std::string scheduler, int quantum_Cycles);
    void schedulerStop();
    void screen();         // show all processes status
    void reportUtil();     // optional: show CPU utilization
};

#endif
