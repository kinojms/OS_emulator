#include "Functions.h"
#include <iostream>
#include <random>

void Functions::FCFS(int num_cpu, int quantum_Cycles, int max_ins) {
    if (schedulerRunning) {
        std::cout << "Scheduler already running.\n";
        return;
    }

    if (!scheduler) {
        scheduler = std::make_shared<Scheduler>();
        for (int i = 0; i < num_cpu; ++i) {
            scheduler->cores.push_back(std::make_shared<CPUCore>(i));
        }
    }

    if (allProcesses.empty()) {
        for (int i = 0; i < 10; ++i) {
            auto p = std::make_shared<Process>(i);
            p->InstructionCode(i);
            for (int j = 0; j < 100; ++j) {
                int instructionID = rand() % 6 + 1;
                p->instructionQueue.push(instructionID);
            }
            allProcesses.push_back(p);
        }
    }

    for (auto& p : allProcesses) {
        if (!p->isFinished) {
            scheduler->addProcess(p);
        }
    }

    schedulerRunning = true;

    schedulerThread = std::thread([this]() {
        while (!scheduler->processQueue.empty()) {
            auto process = scheduler->processQueue.front();
            scheduler->processQueue.pop();

            if (!process->isFinished) {
                process->execute();
            }
        }
        schedulerRunning = false;
        });

    schedulerThread.detach();
}

void Functions::RR(int num_cpu, int quantum_Cycles, int max_ins) {
    if (schedulerRunning) {
        std::cout << "Scheduler already running.\n";
        return;
    }

    if (!scheduler) {
        scheduler = std::make_shared<Scheduler>();
        for (int i = 0; i < num_cpu; ++i) {
            scheduler->cores.push_back(std::make_shared<CPUCore>(i));
        }
    }

    if (allProcesses.empty()) {
        for (int i = 0; i < 100; ++i) {
            auto p = std::make_shared<Process>(i);
            p->InstructionCode(i);
            for (int j = 0; j < 10; ++j) {
                int instructionID = rand() % 6 + 1;
                p->instructionQueue.push(instructionID);
            }
            allProcesses.push_back(p);
        }
    }

    for (auto& p : allProcesses) {
        if (!p->isFinished) {
            scheduler->addProcess(p);
        }
    }

    schedulerRunning = true;

    schedulerThread = std::thread([this, quantum_Cycles]() {
        const int timePerCycleMs = 10;

        while (!scheduler->runningQueue.empty() && scheduler->runningFlag) {
            auto process = scheduler->runningQueue.front();
            scheduler->runningQueue.pop();

            if (!process->isFinished) {
                process->execute();
                if (!process->isFinished) {
                    scheduler->runningQueue.push(process);
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(quantum_Cycles * timePerCycleMs));
        }

        schedulerRunning = false;
        });

    schedulerThread.detach();
}

void Functions::schedulerTest(int num_cpu, const std::string& schedulerType, int quantum_Cycles, int max_ins) {
    if (schedulerType == "fcfs") {
        FCFS(num_cpu, quantum_Cycles, max_ins);
    }
    else if (schedulerType == "rr") {
        RR(num_cpu, quantum_Cycles, max_ins);
    }
    else {
        std::cout << "Unknown scheduler type: " << schedulerType << "\n";
    }

    std::cout << "Scheduler started.\n";
}

void Functions::schedulerStop() {
    if (scheduler) {
        scheduler->runningFlag = false;
    }
    schedulerRunning = false;
    std::cout << "Scheduler stopped.\n";
}

void Functions::screen() {
    for (const auto& p : allProcesses) {
        std::cout << "Process " << p->pid << " - Finished: " << (p->isFinished ? "Yes" : "No") << "\n";
    }
}

void Functions::reportUtil() {
    std::cout << "CPU Utilization report not implemented.\n";
}