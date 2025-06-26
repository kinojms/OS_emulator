#include "Functions.h"
#include <iostream>
#include <algorithm>

void Functions::FCFS(int num_cpu, int qunatum_Cycles) {
    if (schedulerRunning) {
        std::cout << "Scheduler already running.\n";
        return;
    }

    if (!scheduler) {
        scheduler = std::make_shared<Scheduler>();
        // Create 4 CPU cores
        for (int i = 0; i < 4; ++i) {
            scheduler->cores.push_back(std::make_shared<CPUCore>(i));
        }
    }

    // Only create processes if allProcesses is empty
    if (allProcesses.empty()) {
        for (int i = 0; i < 10; ++i) {
            auto p = std::make_shared<Process>(i);
            p->generatePrintCommands(100);
            allProcesses.push_back(p);
        }
    }

    // Add only unfinished processes to the scheduler
    for (auto& p : allProcesses) {
        if (!p->isFinished) {
            scheduler->addProcess(p);
        }
    }

    scheduler->runningFlag = true;
    schedulerRunning = true;

    schedulerThread = std::thread([this]() {
        scheduler->start();
        schedulerRunning = false;
        });

    schedulerThread.detach();

    std::cout << "Scheduler started.\n";
}

void Functions::RR(int num_cpu, int quantum_Cycles) {
    if (schedulerRunning) {
        return;
    }

    // Initialize the scheduler and CPU cores if not already done
    if (!scheduler) {
        scheduler = std::make_shared<Scheduler>();
        for (int i = 0; i < num_cpu; ++i) {
            scheduler->cores.push_back(std::make_shared<CPUCore>(i));
        }
    }

    // Only create processes if allProcesses is empty
    if (allProcesses.empty()) {
        for (int i = 0; i < 10; ++i) {
            auto p = std::make_shared<Process>(i);
            p->generatePrintCommands(100); // give 100 tasks per process
            allProcesses.push_back(p);
        }
    }

    // Add only unfinished processes to the scheduler queue
    for (auto& p : allProcesses) {
        if (!p->isFinished) {
            scheduler->runningQueue.push(p);
        }
    }

    scheduler->runningFlag = true;
    schedulerRunning = true;

    // Launch scheduler loop in a separate thread
    schedulerThread = std::thread([this, quantum_Cycles]() {
        const int timePerCycleMs = 10;

        while (!scheduler->runningQueue.empty() && scheduler->runningFlag) {
            auto process = scheduler->runningQueue.front();
            scheduler->runningQueue.pop();

            if (!process->isFinished) {
                //process->executeTimeSlice(quantum_Cycles);

                if (!process->isFinished) {
                    scheduler->runningQueue.push(process);
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(quantum_Cycles * timePerCycleMs));
        }

        scheduler->runningFlag = false;
        schedulerRunning = false;
    });

    schedulerThread.detach();
}


void Functions::schedulerTest(int num_cpu, std::string scheduler1, int quantum_Cycles) {
    // Convert input to lowercase
    std::transform(scheduler1.begin(), scheduler1.end(), scheduler1.begin(), ::tolower);

    if (scheduler1 == "fcfs") {
        FCFS(num_cpu, quantum_Cycles);
        std::cout << "Scheduler started (FCFS).\n";
    }
    else if (scheduler1 == "rr") {
        RR(num_cpu, quantum_Cycles);
        std::cout << "Scheduler started (Round Robin).\n";
    }
    else {
        std::cout << "Unknown scheduler type: " << scheduler1 << "\n";
    }
}


void Functions::schedulerStop() {
    if (!schedulerRunning) {
        std::cout << "Scheduler is not running.\n";
        return;
    }

    std::cout << "Stopping scheduler...\n";
    scheduler->runningFlag = false;
    schedulerRunning = false;
}

void Functions::screen() {
    std::cout << "Running/Waiting processes:\n";
    for (auto& proc : allProcesses) {
        if (!proc->isFinished) {
            std::cout << "  Process " << proc->pid << "\n";
        }
    }

    std::cout << "Finished processes:\n";
    for (auto& proc : allProcesses) {
        if (proc->isFinished) {
            std::cout << "  Process " << proc->pid << "\n";
        }
    }

}


void Functions::reportUtil() {
    std::cout << "CPU Utilization Report (simulated)...\n";
    for (int i = 0; i < scheduler->cores.size(); ++i) {
        std::cout << "Core " << i << " - "
            << (scheduler->cores[i]->isBusy ? "Busy" : "Idle") << "\n";
    }
}
