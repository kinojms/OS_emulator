#include "CPUCore.h"
#include <iostream>

CPUCore::CPUCore(int id) : id(id), isBusy(false) {}

void CPUCore::assignProcess(std::shared_ptr<Process> process) {
    isBusy = true;
    process->assignedCore = id; // Track assigned core

    workerThread = std::thread([this, process]() {
        process->execute();
        isBusy = false;
        process->assignedCore = -1; // Unassign core when done
    });

    workerThread.detach(); // Let the thread run independently
}