#include "Scheduler.h"  
#include <chrono>
#include <thread>
#include <iostream>

Scheduler::Scheduler() {}

void Scheduler::addProcess(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(queueMutex);
    processQueue.push(process);
    runningQueue.push(process);  // Also add to RR queue for compatibility
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

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    // Optional: std::cout << "All processes completed. Scheduler exiting...\n";
}

void Scheduler::roundRobin(int quantum_Cycles, const std::vector<std::shared_ptr<Process>>& allProcesses, std::atomic<bool>& schedulerRunning) {
    const int timePerCycleMs = 10;
    runningFlag = true;
    schedulerRunning = true;

    while (runningFlag) {
        if (!runningQueue.empty()) {
            auto process = runningQueue.front();
            runningQueue.pop();

            if (!process->isFinished) {
                process->executeTimeSlice(quantum_Cycles);
                if (!process->isFinished) {
                    runningQueue.push(process);
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(quantum_Cycles * timePerCycleMs));
        }
        else {
            bool allDone = true;
            for (const auto& p : allProcesses) {
                if (!p->isFinished) {
                    allDone = false;
                    break;
                }
            }

            if (allDone) {
                runningFlag = false;
                schedulerRunning = false;
                std::cout << "Round Robin scheduler finished.\n";
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}
