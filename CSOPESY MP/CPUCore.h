#ifndef CPUCORE_H
#define CPUCORE_H

#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include "Process.h"

class CPUCore {
public:
    int id;
    std::atomic<bool> isBusy;
    std::thread workerThread;
    std::mutex coreMutex;

    CPUCore(int id);
    void assignProcess(std::shared_ptr<Process> process);
};

#endif
