#ifndef CPUCORE_H
#define CPUCORE_H

#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include "Process.h"
#include "Clock.h"

class CPUCore {
public:
    int id;
    std::atomic<bool> isBusy;
    std::thread workerThread;
    std::mutex coreMutex;
    std::shared_ptr<Clock> clock; // Add this

    CPUCore(int id, std::shared_ptr<Clock> clock);
    void assignProcess(std::shared_ptr<Process> process);
};

#endif
