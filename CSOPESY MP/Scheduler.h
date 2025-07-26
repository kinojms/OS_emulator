#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include "Process.h"
#include "CPUCore.h"
#include "IMemoryManager.h"
#include "IScheduler.h"

class Scheduler : public IScheduler {
public:
    std::queue<std::shared_ptr<Process>> processQueue;
    std::queue<std::shared_ptr<Process>> runningQueue;
    std::vector<std::shared_ptr<CPUCore>> cores;
    std::mutex queueMutex;
    std::atomic<bool> runningFlag{ true };
    std::shared_ptr<IMemoryManager> memoryManager;

    Scheduler();

    void addProcess(std::shared_ptr<Process> process) override;
    void start() override;
    void roundRobin(int quantum_Cycles, const std::vector<std::shared_ptr<Process>>& allProcesses, std::atomic<bool>& schedulerRunning) override;
    void setMemoryManager(std::shared_ptr<IMemoryManager> memMgr) override;
};

#endif
