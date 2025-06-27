#include "Functions.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <fstream>
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
            for (int j = 0; j < max_ins; ++j) {
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
        for (int i = 0; i < 10; ++i) {
            auto p = std::make_shared<Process>(i);
            p->InstructionCode(i);
            for (int j = 0; j < max_ins; ++j) {
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

    scheduler->runningFlag = true;
    schedulerRunning = true;

    schedulerThread = std::thread([this, quantum_Cycles]() {
        const int timePerCycleMs = 10;

        while (scheduler->runningFlag) {
            if (!scheduler->runningQueue.empty()) {
                auto process = scheduler->runningQueue.front();
                scheduler->runningQueue.pop();

                if (!process->isFinished) {
                    process->executeTimeSlice(quantum_Cycles);
                    if (!process->isFinished) {
                        scheduler->runningQueue.push(process);
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(quantum_Cycles * timePerCycleMs));
            }
            else {
                // check if all processes are finished
                bool allDone = true;
                for (const auto& p : allProcesses) {
                    if (!p->isFinished) {
                        allDone = false;
                        break;
                    }
                }

                if (allDone) {
                    scheduler->runningFlag = false;
                    schedulerRunning = false;
                    std::cout << "Round Robin scheduler finished.\n";
                    break;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
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

std::string getCurrentDateTime() {
    std::time_t now = std::time(nullptr);
    std::tm local_tm;
#ifdef _WIN32
    localtime_s(&local_tm, &now);
#else
    local_tm = *std::localtime(&now);
#endif
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &local_tm);
    return buf;
}
int getCurrentInstruction(const std::shared_ptr<Process>& proc) {
    //Add get current instruction logic here
    return 1000;
}
int getTotalInstructions(const std::shared_ptr<Process>& proc) {
    return 1000; // Add get total instruction logic here
}
int getAssignedCore(const std::shared_ptr<Process>& proc, const std::vector<std::shared_ptr<CPUCore>>& cores) {
    // For now, return -1 (unknown)
    return -1;
}

void Functions::reportUtil() {
    if (!scheduler || scheduler->cores.empty()) {
        std::cout << "Scheduler not running or no cores available.\n";
        return;
    }

    int totalCores = static_cast<int>(scheduler->cores.size());
    int usedCores = 0;
    for (const auto& core : scheduler->cores) {
        if (core->isBusy) usedCores++;
    }
    int availableCores = totalCores - usedCores;
    double utilization = (totalCores > 0) ? (static_cast<double>(usedCores) / totalCores) * 100.0 : 0.0;

    std::ofstream logFile("csopesy-log.txt");
    if (!logFile) {
        std::cout << "Failed to write report to C:/csopesy-log.txt!\n";
        return;
    }

    logFile << "CPU utilization: " << static_cast<int>(utilization) << "%\n";
    logFile << "Cores used: " << usedCores << "\n";
    logFile << "Cores available: " << availableCores << "\n";
    logFile << "-------------------------\n";
    logFile << "Running processes:\n";
    for (const auto& proc : allProcesses) {
        if (!proc->isFinished) {
            std::string name = std::string("process") + (proc->pid < 10 ? "0" : "") + std::to_string(proc->pid);
            std::string datetime = getCurrentDateTime();
            int coreId = getAssignedCore(proc, scheduler->cores);
            int currentInst = getCurrentInstruction(proc);
            int totalInst = getTotalInstructions(proc);
            logFile << "  " << name << "   (" << datetime << ")   Core: ";
            if (coreId >= 0) logFile << coreId;
            else logFile << "N/A";
            logFile << "      " << currentInst << "/" << totalInst << "\n";
        }
    }
    logFile << "Finished processes:\n";
    for (const auto& proc : allProcesses) {
        if (proc->isFinished) {
            std::string name = std::string("process") + (proc->pid < 10 ? "0" : "") + std::to_string(proc->pid);
            std::string datetime = getCurrentDateTime();
            int totalInst = getTotalInstructions(proc);
            logFile << "  " << name << "   (" << datetime << ")   Finished      " << totalInst << "/" << totalInst << "\n";
        }
    }
    logFile << "---------------------\n";
    logFile.close();

    std::cout << "Report generated at C:/csopesy-log.txt!\n";
}

