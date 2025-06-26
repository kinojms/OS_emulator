#include "CPUCore.h"
#include <iostream>

CPUCore::CPUCore(int id) : id(id), isBusy(false) {}

void CPUCore::assignProcess(std::shared_ptr<Process> process) {
    isBusy = true;

    workerThread = std::thread([this, process]() {
        // std::cout << "Core " << id << " started executing Process " << process->pid << "\n";
        process->execute();
        // std::cout << "Core " << id << " finished executing Process " << process->pid << "\n";
        isBusy = false;
        });

    workerThread.detach(); // Let the thread run independently
}
