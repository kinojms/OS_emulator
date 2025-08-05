#include "Functions.h"
#include "Display.h"
#include "Clock.h"
#include <iostream>
#include <random>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <Windows.h>
#include <sstream>

std::shared_ptr<Clock> globalClock = nullptr;

void Functions::runScheduler(
    int num_cpu,
    int quantum_Cycles,
    int min_ins,
    int max_ins,
    int batch_process_freq,
    float delay_Per_Exec,
    const std::string& schedulerType
) {
    if (schedulerRunning) {
        std::cout << "Scheduler already running.\n";
        return;
    }

    if (!globalClock) globalClock = std::make_shared<Clock>();

    if (!scheduler) {
        scheduler = std::make_shared<Scheduler>();
        for (int i = 0; i < num_cpu; ++i) {
            auto core = std::make_shared<CPUCore>(i, globalClock);
            if (memoryManager) core->setMemoryManager(memoryManager);
            scheduler->cores.push_back(core);
        }
        if (memoryManager) scheduler->setMemoryManager(memoryManager);
    }
    else {
        while (!scheduler->processQueue.empty()) scheduler->processQueue.pop();
        while (!scheduler->runningQueue.empty()) scheduler->runningQueue.pop();
    }

    for (auto& p : allProcesses) {
        if (!p->isFinished) scheduler->addProcess(p);
    }

    schedulerRunning = true;
    schedulerStopRequested = false;
    if (schedulerType == "rr") scheduler->runningFlag = true;

    // Clock thread
    std::thread([this, schedulerType]() {
        while (schedulerRunning || (schedulerType == "rr" && schedulerStopRequested)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (globalClock) globalClock->advance();
        }
        if (globalClock) globalClock->stop();
        }).detach();

    // Scheduler thread
    schedulerThread = std::thread([this, quantum_Cycles, delay_Per_Exec, schedulerType]() {
        int lastSnapshotCycle = -1;
        const int timePerCycleMs = static_cast<int>(delay_Per_Exec);

        while ((schedulerType == "fcfs" && schedulerRunning) ||
            (schedulerType == "rr" && (scheduler->runningFlag || schedulerStopRequested))) {

            std::shared_ptr<Process> process = nullptr;

            {
                std::lock_guard<std::mutex> lock(scheduler->queueMutex);
                if (schedulerType == "fcfs") {
                    if (!scheduler->processQueue.empty()) {
                        process = scheduler->processQueue.front();
                        scheduler->processQueue.pop();
                    }
                }
                else if (schedulerType == "rr") {
                    while (!scheduler->runningQueue.empty()) {
                        auto next = scheduler->runningQueue.front();
                        scheduler->runningQueue.pop();
                        if (!next->isFinished) {
                            process = next;
                            break;
                        }
                    }
                }
            }

            if (process) {
                if (schedulerType == "fcfs") {
                    // FCFS logic
                    bool assigned = false;
                    while (!assigned) {
                        for (auto& core : scheduler->cores) {
                            if (!core->isBusy) {
                                core->assignProcess(process);
                                assigned = true;
                                break;
                            }
                        }
                        if (!assigned) std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                }
                else if (schedulerType == "rr") {
                    // RR logic
                    if (!process->isMemoryAllocated()) {
                        bool allocated = false;
                        if (memoryManager) {
                            allocated = memoryManager->allocateMemory(process);
                            process->setMemoryAllocated(allocated);
                        }
                        if (!allocated) {
                            std::lock_guard<std::mutex> lock(scheduler->queueMutex);
                            scheduler->runningQueue.push(process);
                            std::this_thread::sleep_for(std::chrono::milliseconds(10));
                            continue;
                        }
                    }
                    bool assigned = false;
                    while (!assigned) {
                        for (auto& core : scheduler->cores) {
                            if (!core->isBusy) {
                                core->isBusy = true;
                                process->assignedCore = core->id;
                                std::thread([core, process, quantum_Cycles, this, timePerCycleMs]() {
                                    std::this_thread::sleep_for(std::chrono::milliseconds(quantum_Cycles * timePerCycleMs));
                                    process->runInstructions(quantum_Cycles);
                                    core->isBusy = false;
                                    process->assignedCore = -1;
                                    if (!process->isFinished) {
                                        std::lock_guard<std::mutex> lock(scheduler->queueMutex);
                                        scheduler->runningQueue.push(process);
                                    }
                                    else {
                                        if (memoryManager && process->isMemoryAllocated()) {
                                            memoryManager->deallocateMemory(process->processName);
                                            process->setMemoryAllocated(false);
                                        }
                                        process->writeLogsToFile();
                                    }
                                    }).detach();
                                assigned = true;
                                break;
                            }
                        }
                        if (!assigned) std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                }

                // Memory snapshot
                if (globalClock && memoryManager) {
                    int currentCycle = globalClock->cycle.load();
                    if (currentCycle != lastSnapshotCycle) {
                        memoryManager->setQuantumCycle(currentCycle);
                        memoryManager->generateSnapshotFile();
                        lastSnapshotCycle = currentCycle;
                    }
                }
            }
            else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            // Stop condition
            if (schedulerStopRequested) {
                bool allDone = std::all_of(allProcesses.begin(), allProcesses.end(),
                    [](const std::shared_ptr<Process>& p) { return p->isFinished; });

                std::lock_guard<std::mutex> lock(scheduler->queueMutex);
                if (schedulerType == "fcfs" && allDone && scheduler->processQueue.empty()) {
                    schedulerRunning = false;
                    break;
                }
                else if (schedulerType == "rr" && allDone && scheduler->runningQueue.empty()) {
                    scheduler->runningFlag = false;
                    schedulerRunning = false;
                    std::cout << "Round Robin scheduler finished.\n";
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

//void Functions::schedulerTest(int num_cpu, const std::string& schedulerType, int quantum_Cycles, int min_ins, int max_ins, int batch_process_freq, float delay_Per_Exec) {
//    if (schedulerType == "fcfs") {
//        FCFS(num_cpu, quantum_Cycles, min_ins, max_ins, batch_process_freq, delay_Per_Exec);
//    }
//    else if (schedulerType == "rr") {
//        RR(num_cpu, quantum_Cycles, min_ins, max_ins, batch_process_freq, delay_Per_Exec);
//    }
//    else {
//        std::cout << "Unknown scheduler type: " << schedulerType << "\n";
//    }
//
//    std::cout << "Scheduler started.\n";
//}

void Functions::schedulerStop() {
    schedulerStopRequested = true; // Request scheduler to stop after finishing all current processes
    stopProcessGenerator(); // Stop generating new processes
    std::cout << "Scheduler stop requested. Waiting for all processes to finish...\n";
}

void Functions::writeScreenReport(std::ostream& out) {
    if (!scheduler) {
        out << "No scheduler initialized.\n";
        return;
    }

    int totalCores = static_cast<int>(scheduler->cores.size());
    int availableCores = 0;
    for (const auto& core : scheduler->cores) {
        if (!core->isBusy) availableCores++;
    }

    int usedCores = totalCores - availableCores;
    double cpuUtilization = totalCores > 0 ? (static_cast<double>(usedCores) / totalCores) : 0.0;

    out << "CPU Utilization: " << std::fixed << std::setprecision(2) << (cpuUtilization * 100) << "%\n";
    out << "Cores used: " << usedCores << "\n";
    out << "Cores available: " << availableCores << "\n";
    out << "\n--------------------------------------------------------\n";

    out << "Running processes (in memory & assigned to a core):\n";
    for (const auto& p : allProcesses) {
        if (!p->isFinished && p->isMemoryAllocated() && p->assignedCore != -1) {
            out << p->processName << " Core: " << p->assignedCore
                << " " << p->currentInstruction << "/"
                << (p->totalInstructions == 0 ? "-" : std::to_string(p->totalInstructions)) << "\n";
        }
    }

    out << "\nWaiting for memory (not in memory):\n";
    for (const auto& p : allProcesses) {
        if (!p->isFinished && !p->isMemoryAllocated()) {
            out << p->processName << " Core: - "
                << p->currentInstruction << "/"
                << (p->totalInstructions == 0 ? "-" : std::to_string(p->totalInstructions)) << "\n";
        }
    }

    out << "\nIn memory but not running (waiting for core):\n";
    for (const auto& p : allProcesses) {
        if (!p->isFinished && p->isMemoryAllocated() && p->assignedCore == -1) {
            out << p->processName << " Core: - "
                << p->currentInstruction << "/"
                << (p->totalInstructions == 0 ? "-" : std::to_string(p->totalInstructions)) << "\n";
        }
    }

    out << "\nFinished processes:\n";
    for (const auto& p : allProcesses) {
        if (p->isFinished) {
            int coreID = (p->assignedCore == -1 ? -1 : p->assignedCore);
            out << p->processName << " Core: " << (coreID == -1 ? "-" : std::to_string(coreID))
                << " " << p->currentInstruction << "/"
                << (p->totalInstructions == 0 ? "-" : std::to_string(p->totalInstructions)) << "\n";
        }
    }
}

//void Functions::screen() {
//    writeScreenReport(std::cout);
//}

void Functions::reportUtil() {
    // Write logs to file for all processes
    for (const auto& p : allProcesses) {
        p->writeLogsToFile();
    }
    // Write the screen summary to cpu_report.txt
    std::ofstream cpuReportFile("cpu_report.txt", std::ios::out);
    if (cpuReportFile.is_open()) {
        writeScreenReport(cpuReportFile);
        cpuReportFile.close();
        std::cout << "CPU utilization and process summary written to cpu_report.txt\n";
    }
    else {
        std::cerr << "Failed to write cpu_report.txt\n";
    }
    std::cout << "Process logs have been written to their respective .txt files.\n";
}

std::shared_ptr<Process> Functions::createProcess(const std::string& name, int min_ins, int max_ins, float delay_per_exec, int size) {
    // Validate memory size
    //if (size < 64 || size > 262144 || (size & (size - 1)) != 0) {
    //    std::cerr << "Invalid memory allocation: must be a power of 2 between 64 and 262144 bytes.\n";
    //    return nullptr;
    //}
    std::cout << "Process " << name << " created with PID " << name << " and size " << size << "\n";

    int pid = static_cast<int>(allProcesses.size());

    // Make sure Process constructor supports memorySize if you want to pass it in
    auto p = std::make_shared<Process>(pid, name, size); // or (pid, name, size)

    // Set memory manager for demand paging
    if (memoryManager) {
        p->setMemoryManager(memoryManager.get());
    }

    p->InstructionCode(pid);
    int num_instructions = min_ins + (rand() % (max_ins - min_ins + 1));

    for (int j = 0; j < num_instructions; ++j) {
        int instructionID = rand() % 8 + 1;
        p->instructionQueue.push(instructionID);
    }

    allProcesses.push_back(p);

    if (scheduler && (schedulerRunning || scheduler->runningFlag)) {
        scheduler->addProcess(p);
    }

    return p;
}

// helper function to iterate over all processes and find one by name
std::shared_ptr<Process> Functions::getProcessByName(const std::string& name) {
    for (auto& p : allProcesses) {
        if (p->processName == name) {
            return p;
        }
    }
    return nullptr;
}

void Functions::switchScreen(const std::string& name) {

    auto proc = getProcessByName(name);
    if (!proc) {
        std::cout << "Process '" << name << "' not found.\n";
        return;
    }

    system("cls");
    std::cout << "\n--- Process Screen ---\n";
    std::cout << "Process name: " << proc->processName << "\n";
    std::cout << "ID: " << proc->pid << "\n";
    std::cout << "Logs:\n";
    // Display logs from memory
    if (!proc->logs.empty()) {
        for (const auto& log : proc->logs) {
            std::cout << log << "\n";
        }
    }
    else {
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
            }
            else {
                std::cout << "No logs found for this process.\n";
            }
            if (proc->isFinished) {
                std::cout << "Finished!\n";
            }
        }
        else {
            std::cout << "Unknown command. Type 'process-smi' or 'exit'.\n";
        }
    }
}

void Functions::startProcessGenerator(int min_ins, int max_ins, int batch_process_freq) {

    std::cout << "Process generation started. Use screen-ls to check the progress.\n";

    processGenRunning = true;
    processGenThread = std::thread([this, min_ins, max_ins, batch_process_freq]() {
        int lastCycle = globalClock ? globalClock->cycle.load() : 0;
        while (processGenRunning) {
            if (!globalClock) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            // Wait for batch_process_freq cycles
            for (int i = 0; i < batch_process_freq && processGenRunning; ++i) {
                globalClock->waitForNextCycle(lastCycle);
                lastCycle = globalClock->cycle.load();
            }
            if (!processGenRunning) break;
            int pid = static_cast<int>(allProcesses.size());
            auto p = std::make_shared<Process>(pid);
            p->InstructionCode(pid);
            int num_instructions = min_ins + (rand() % (max_ins - min_ins + 1));
            for (int j = 0; j < num_instructions; ++j) {
                int instructionID = rand() % 8 + 1;
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

void Functions::initializeMemoryManager(int maxOverallMem, int memPerProc, int memPerFrame) {
    if (!memoryManager) {
        memoryManager = std::make_shared<MemoryManager>(maxOverallMem, memPerProc, memPerFrame);
        // std::cout << "[Functions] Memory manager initialized with " << maxOverallMem 
        //          << " bytes total, " << memPerProc << " bytes per process" << std::endl;
    }
}

void Functions::generateMemorySnapshot() {
    if (memoryManager && globalClock) {
        memoryManager->setQuantumCycle(globalClock->cycle.load());
        memoryManager->generateSnapshotFile();
    }
}