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
    std::vector<std::shared_ptr<CPUCore>> cores;
    std::mutex queueMutex;
    std::atomic<bool> runningFlag{ true };

    Scheduler();

    void addProcess(std::shared_ptr<Process> process);
    void start();
};

#endif
