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

struct MemoryBlock {
    int startAddress;
    int size;
    std::string processName;
    bool isAllocated;

    MemoryBlock(int start, int sz, const std::string& proc = "", bool allocated = false)
        : startAddress(start), size(sz), processName(proc), isAllocated(allocated) {}
};

class MemoryManager {
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

    bool allocateMemory(std::shared_ptr<Process> process);
    void deallocateMemory(const std::string& processName);
    void setQuantumCycle(int cycle);
    int getProcessesInMemory();
    void generateSnapshotFile();
};

#endif // MEMORYMANAGER_H 