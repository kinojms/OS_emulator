
#include "Scheduler.h"  
#include <chrono>
#include <thread>
#include <iostream>

Scheduler::Scheduler() {}

void Scheduler::addProcess(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(queueMutex);
    processQueue.push(process);
}

void Scheduler::start() {
    while (runningFlag) {
        bool allIdle = true;

        for (auto& core : cores) {
            if (!core->isBusy) {
                std::lock_guard<std::mutex> lock(queueMutex);
                if (!processQueue.empty()) {
                    auto process = processQueue.front();
                    processQueue.pop();
                    core->assignProcess(process);
                }
            }
            else {
                allIdle = false;
            }
        }

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (processQueue.empty() && allIdle) break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }

    // std::cout << "All processes completed. Scheduler exiting...\n";
}