#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <vector>
#include <memory>
#include <string>
#include <mutex>
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "Process.h"
#include "IMemoryManager.h"

struct MemoryBlock {
    int startAddress;
    int size;
    std::string processName;
    bool isAllocated;

    MemoryBlock(int start, int sz, const std::string& proc = "", bool allocated = false)
        : startAddress(start), size(sz), processName(proc), isAllocated(allocated) {}
};

class MemoryManager : public IMemoryManager {
private:
    std::vector<MemoryBlock> memoryBlocks;
    int totalMemory;
    int memoryPerProcess;
    int memoryPerFrame;
    std::mutex memoryMutex;
    int currentQuantumCycle;

    void initializeMemory();
    void generateMemorySnapshot();
    int calculateExternalFragmentation();
    int calculateExternalFragmentationInternal();
    std::string formatMemoryLayout();
    std::string formatMemoryLayoutInternal();
    int getProcessesInMemoryInternal();

public:
    MemoryManager(int totalMem, int memPerProc, int memPerFrame);

    bool allocateMemory(std::shared_ptr<Process> process) override;
    void deallocateMemory(const std::string& processName) override;
    void setQuantumCycle(int cycle) override;
    int getProcessesInMemory() override;
    void generateSnapshotFile() override;
};

#endif // MEMORYMANAGER_H 