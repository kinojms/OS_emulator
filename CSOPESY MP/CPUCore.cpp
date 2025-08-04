#include "CPUCore.h"
#include <iostream>
#include "MemoryManager.h"

CPUCore::CPUCore(int id, std::shared_ptr<Clock> clock)
    : id(id), isBusy(false), clock(clock), memoryManager(nullptr) {}

void CPUCore::setMemoryManager(std::shared_ptr<MemoryManager> memMgr) {
    memoryManager = memMgr;
}

void CPUCore::assignProcess(std::shared_ptr<Process> process) {
    isBusy = true;
    process->assignedCore = id; // Track assigned core

    workerThread = std::thread([this, process]() {
        int lastCycle = clock ? clock->cycle.load() : 0;
        while (!process->isFinished) {
            if (clock) {
                clock->waitForNextCycle(lastCycle);
                lastCycle = clock->cycle.load();
                // Log core execution at this clock cycle
                /*std::cout << "[Clock Cycle " << lastCycle << "] Core " << id << " executing process '"
                          << process->processName << "' (PID " << process->pid << ")" << std::endl;*/
            }
            process->runInstructions(1); // Execute one instruction per cycle
        }
        isBusy = false;
        process->assignedCore = -1; // Unassign core when done

        // Deallocate memory when process finishes
        if (memoryManager && process->isMemoryAllocated()) {
            memoryManager->deallocateMemory(process->processName);
            process->setMemoryAllocated(false);
            std::cout << "[CPUCore] Memory deallocated for finished process " << process->processName << std::endl;
        }
        });

    workerThread.detach(); // Let the thread run independently
}