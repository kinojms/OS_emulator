#include "Functions.h"
#include "Display.h"
#include <iostream>
#include <random>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <Windows.h>

void Functions::FCFS(int num_cpu, int quantum_Cycles, int max_ins, int min_ins) {
    if (schedulerRunning) {
        std::cout << "Scheduler already running.\n";
        return;
    }

    if (!scheduler) {
        scheduler = std::make_shared<Scheduler>();
        for (int i = 0; i < num_cpu; ++i) {
            scheduler->cores.push_back(std::make_shared<CPUCore>(i));
        }
    } else {
        // Clear queues if scheduler already exists (for restart)
        while (!scheduler->processQueue.empty()) scheduler->processQueue.pop();
        while (!scheduler->runningQueue.empty()) scheduler->runningQueue.pop();
    }

    // Always ensure there are at least 10 processes
    int existing = static_cast<int>(allProcesses.size());
    for (int i = existing; i < 10; ++i) {
        auto p = std::make_shared<Process>(i);
        p->InstructionCode(i);
        int num_instructions = min_ins + (rand() % (max_ins - min_ins + 1));

        for (int j = 0; j < num_instructions; ++j) {
            int instructionID = rand() % 6 + 1;
            p->instructionQueue.push(instructionID);
        }
        allProcesses.push_back(p);
    }

    for (auto& p : allProcesses) {
        if (!p->isFinished) {
            scheduler->addProcess(p);
        }
    }

    schedulerRunning = true;
    schedulerStopRequested = false;
    startProcessGenerator(max_ins);

    schedulerThread = std::thread([this]() {
        while (schedulerRunning || schedulerStopRequested) {
            std::shared_ptr<Process> process = nullptr;
            {
                std::lock_guard<std::mutex> lock(scheduler->queueMutex);
                if (!scheduler->processQueue.empty()) {
                    process = scheduler->processQueue.front();
                    scheduler->processQueue.pop();
                }
            }
            if (process) {
                // assign to core as before
                bool assigned = false;
                while (!assigned) {
                    for (auto& core : scheduler->cores) {
                        if (!core->isBusy) {
                            core->assignProcess(process);
                            assigned = true;
                            break;
                        }
                    }
                    if (!assigned) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            // Only exit if stop requested and all processes are finished and the queue is empty
            if (schedulerStopRequested) {
                bool allDone = true;
                for (const auto& p : allProcesses) {
                    if (!p->isFinished) {
                        allDone = false;
                        break;
                    }
                }
                std::lock_guard<std::mutex> lock(scheduler->queueMutex);
                if (allDone && scheduler->processQueue.empty()) {
                    schedulerRunning = false;
                    break;
                }
            }
        }
        // Wait for all cores to finish
        bool anyBusy = true;
        while (anyBusy) {
            anyBusy = false;
            for (auto& core : scheduler->cores) {
                if (core->isBusy) {
                    anyBusy = true;
                    break;
                }
            }
            if (anyBusy) std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    schedulerThread.detach();
}

void Functions::RR(int num_cpu, int quantum_Cycles, int max_ins,int min_ins) {
    if (schedulerRunning) {
        std::cout << "Scheduler already running.\n";
        return;
    }

    if (!scheduler) {
        scheduler = std::make_shared<Scheduler>();
        for (int i = 0; i < num_cpu; ++i) {
            scheduler->cores.push_back(std::make_shared<CPUCore>(i));
        }
    } else {
        // Clear queues if scheduler already exists (for restart)
        while (!scheduler->processQueue.empty()) scheduler->processQueue.pop();
        while (!scheduler->runningQueue.empty()) scheduler->runningQueue.pop();
    }

    // Always ensure there are at least 10 processes
    int existing = static_cast<int>(allProcesses.size());
    for (int i = existing; i < 10; ++i) {
        auto p = std::make_shared<Process>(i);
        p->InstructionCode(i);
        int num_instructions = min_ins + (rand() % (max_ins - min_ins + 1));
        for (int j = 0; j < num_instructions; ++j) {
            int instructionID = rand() % 6 + 1;
            p->instructionQueue.push(instructionID);
        }
        allProcesses.push_back(p);
    }

    for (auto& p : allProcesses) {
        if (!p->isFinished) {
            scheduler->addProcess(p);
        }
    }

    scheduler->runningFlag = true;
    schedulerRunning = true;
    schedulerStopRequested = false;
    startProcessGenerator(max_ins);

    schedulerThread = std::thread([this, quantum_Cycles]() {
        const int timePerCycleMs = 10;
        while (scheduler->runningFlag || schedulerStopRequested) {
            std::shared_ptr<Process> process = nullptr;
            {
                std::lock_guard<std::mutex> lock(scheduler->queueMutex);
                if (!scheduler->runningQueue.empty()) {
                    process = scheduler->runningQueue.front();
                    scheduler->runningQueue.pop();
                }
            }
            if (process) {
                // Find an available core
                bool assigned = false;
                while (!assigned) {
                    for (auto& core : scheduler->cores) {
                        if (!core->isBusy) {
                            // Use a lambda to wrap executeTimeSlice
                            core->isBusy = true;
                            process->assignedCore = core->id;
                            std::thread([core, process, quantum_Cycles]() {
                                process->executeTimeSlice(quantum_Cycles);
                                core->isBusy = false;
                                process->assignedCore = -1;
                            }).detach();
                            assigned = true;
                            break;
                        }
                    }
                    if (!assigned) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(quantum_Cycles * timePerCycleMs));
                if (!process->isFinished) {
                    std::lock_guard<std::mutex> lock(scheduler->queueMutex);
                    scheduler->runningQueue.push(process);
                }
            } else {
                // Only exit if stop requested and all processes are finished and the queue is empty
                if (schedulerStopRequested) {
                    bool allDone = true;
                    for (const auto& p : allProcesses) {
                        if (!p->isFinished) {
                            allDone = false;
                            break;
                        }
                    }
                    std::lock_guard<std::mutex> lock(scheduler->queueMutex);
                    if (allDone && scheduler->runningQueue.empty()) {
                        scheduler->runningFlag = false;
                        schedulerRunning = false;
                        std::cout << "Round Robin scheduler finished.\n";
                        break;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        // Wait for all cores to finish
        bool anyBusy = true;
        while (anyBusy) {
            anyBusy = false;
            for (auto& core : scheduler->cores) {
                if (core->isBusy) {
                    anyBusy = true;
                    break;
                }
            }
            if (anyBusy) std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    schedulerThread.detach();
}

void Functions::schedulerTest(int num_cpu, const std::string& schedulerType, int quantum_Cycles, int max_ins,int min_ins) {
    if (schedulerType == "fcfs") {
        FCFS(num_cpu, quantum_Cycles, max_ins, min_ins);
    }
    else if (schedulerType == "rr") {
        RR(num_cpu, quantum_Cycles, max_ins, min_ins);
    }
    else {
        std::cout << "Unknown scheduler type: " << schedulerType << "\n";
    }

    std::cout << "Scheduler started.\n";
}

void Functions::schedulerStop() {
    schedulerStopRequested = true; // Request scheduler to stop after finishing all current processes
    stopProcessGenerator(); // Stop generating new processes
    std::cout << "Scheduler stop requested. Waiting for all processes to finish...\n";
}

void Functions::screen() {
    if (!scheduler) {
        std::cout << "No scheduler initialized.\n";
        return;
    }
    int totalCores = static_cast<int>(scheduler->cores.size());
    int availableCores = 0;
    for (const auto& core : scheduler->cores) {
        if (!core->isBusy) availableCores++;
    }
    std::cout << "Cores used: " << (totalCores - availableCores) << "\n";
    std::cout << "Cores available: " << availableCores << "\n";
    std::cout << "\n--------------------------------------------------------\n";
    std::cout << "Running processes:\n";
    for (const auto& p : allProcesses) {
        if (!p->isFinished) {
            std::string timestamp = "-";
            if (!p->logs.empty()) {
                size_t l = p->logs.back().find("]");
                if (l != std::string::npos) timestamp = p->logs.back().substr(0, l+1);
            }
            std::cout << p->processName << " " << timestamp << " Core: " << (p->assignedCore == -1 ? "-" : std::to_string(p->assignedCore))
                << " " << p->currentInstruction << "/" << (p->totalInstructions == 0 ? "-" : std::to_string(p->totalInstructions)) << "\n";
        }
    }
    std::cout << "\nFinished processes:\n";
    for (const auto& p : allProcesses) {
        if (p->isFinished) {
            std::string timestamp = "-";
            if (!p->logs.empty()) {
                size_t l = p->logs.back().find("]");
                if (l != std::string::npos) timestamp = p->logs.back().substr(0, l+1);
            }
            std::cout << p->processName << " " << timestamp << " Core: " << (p->assignedCore == -1 ? "-" : std::to_string(p->assignedCore))
                << " " << p->currentInstruction << "/" << (p->totalInstructions == 0 ? "-" : std::to_string(p->totalInstructions)) << "\n";
        }
    }
}

void Functions::reportUtil() {
    // Write logs to file for all processes
    for (const auto& p : allProcesses) {
        p->writeLogsToFile();
    }
    std::cout << "Process logs have been written to their respective .txt files.\n";
}

std::shared_ptr<Process> Functions::createProcess(const std::string& name, int min_ins, int max_ins, float delay_per_exec) {
    int pid = static_cast<int>(allProcesses.size());
    auto p = std::make_shared<Process>(pid, name);
    p->InstructionCode(pid);
    int num_instructions = min_ins + (rand() % (max_ins - min_ins + 1));
    for (int j = 0; j < num_instructions; ++j) {
        int instructionID = rand() % 6 + 1;
        p->instructionQueue.push(instructionID);
    }
    allProcesses.push_back(p);
    // Add to scheduler if running
    if (scheduler && (schedulerRunning || scheduler->runningFlag)) {
        scheduler->addProcess(p);
    }
    return p;
}

std::shared_ptr<Process> Functions::getProcessByName(const std::string& name) {
    for (auto& p : allProcesses) {
        if (p->processName == name) {
            return p;
        }
    }
    return nullptr;
}

void Functions::switchScreen(const std::string& name) {
    system("cls");
    auto proc = getProcessByName(name);
    if (!proc) {
        std::cout << "Process '" << name << "' not found.\n";
        return;
    }
    std::cout << "\n--- Process Screen ---\n";
    std::cout << "Process name: " << proc->processName << "\n";
    std::cout << "ID: " << proc->pid << "\n";
    std::cout << "Logs:\n";
    // Display logs from memory
    if (!proc->logs.empty()) {
        for (const auto& log : proc->logs) {
            std::cout << log << "\n";
        }
    } else {
        std::cout << "No logs found for this process.\n";
    }
    std::string cmd;

    while (true) {
        std::cout << "\n(process) > ";
        std::getline(std::cin, cmd);
        if (cmd == "exit") {
            system("cls");
            extern Display dp;
            dp.displayIntro();
            break;
        }
        else if (cmd == "process-smi") {
            std::cout << "Process name: " << proc->processName << "\n";
            std::cout << "ID: " << proc->pid << "\n";
            std::cout << "Logs:\n";
            if (!proc->logs.empty()) {
                for (const auto& log : proc->logs) {
                    std::cout << log << "\n";
                }
            } else {
                std::cout << "No logs found for this process.\n";
            }
            if (proc->isFinished) {
                std::cout << "Finished!\n";
            }
        } else {
            std::cout << "Unknown command. Type 'process-smi' or 'exit'.\n";
        }
    }
}

// check if we need this 
void Functions::startProcessGenerator(int max_ins) {
    processGenRunning = true;
    processGenThread = std::thread([this, max_ins]() {
        while (processGenRunning) {
            // Generate a new random process every 2 seconds
            std::this_thread::sleep_for(std::chrono::seconds(2));
            if (!processGenRunning) break;
            int pid = static_cast<int>(allProcesses.size());
            auto p = std::make_shared<Process>(pid);
            p->InstructionCode(pid);
            int num_instructions = 1 + (rand() % max_ins);
            for (int j = 0; j < num_instructions; ++j) {
                int instructionID = rand() % 6 + 1;
                p->instructionQueue.push(instructionID);
            }
            allProcesses.push_back(p);
            if (scheduler) {
                scheduler->addProcess(p);
            }
            // std::cout << "[Process Generator] New random process created: " << p->processName << std::endl;
        }
    });
}

void Functions::stopProcessGenerator() {
    processGenRunning = false;
    if (processGenThread.joinable()) {
        processGenThread.join();
    }
}
