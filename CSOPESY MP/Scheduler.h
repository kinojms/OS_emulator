#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include "Process.h"
#include "CPUCore.h"

class Scheduler {
public:
    std::queue<std::shared_ptr<Process>> processQueue;
    std::queue<std::shared_ptr<Process>> runningQueue; // Add this member to fix the error  
    std::vector<std::shared_ptr<CPUCore>> cores;
    std::mutex queueMutex;
    std::atomic<bool> runningFlag{ true };

    Scheduler();

    void addProcess(std::shared_ptr<Process> process);
    void start();
    void roundRobin(int quantum_Cycles, const std::vector<std::shared_ptr<Process>>& allProcesses, std::atomic<bool>& schedulerRunning);
};

#endif
