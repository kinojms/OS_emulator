#pragma once
#include <memory>
#include <vector>
#include <atomic>
#include "Process.h"

class IScheduler {
public:
    virtual ~IScheduler() = default;
    virtual void addProcess(std::shared_ptr<Process> process) = 0;
    virtual void start() = 0;
    virtual void roundRobin(int quantum_Cycles, const std::vector<std::shared_ptr<Process>>& allProcesses, std::atomic<bool>& schedulerRunning) = 0;
    virtual void setMemoryManager(std::shared_ptr<IMemoryManager> memMgr) = 0;
};
