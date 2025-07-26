#pragma once
#include <memory>
#include <string>
#include "Process.h"

class IMemoryManager {
public:
    virtual ~IMemoryManager() = default;
    virtual bool allocateMemory(std::shared_ptr<Process> process) = 0;
    virtual void deallocateMemory(const std::string& processName) = 0;
    virtual void setQuantumCycle(int cycle) = 0;
    virtual int getProcessesInMemory() = 0;
    virtual void generateSnapshotFile() = 0;
};
