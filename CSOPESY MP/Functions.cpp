#include "Functions.h"
#include <iostream>

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

void Functions::RR(int num_cpu, int qunatum_Cycles) {
    //implement this
}

void Functions::schedulerTest(int num_cpu, std::string scheduler1, int quantum_Cycles) {
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
