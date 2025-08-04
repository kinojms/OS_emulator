#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <vector>
#include <map>
#include <memory>
#include <string>
#include <mutex>
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "Process.h"

struct Frame {
    int frameNumber;
    std::string processName;
    int pageNumber;
    bool isOccupied;
    std::string data;
};

struct PageTableEntry {
    int frameNumber; // -1 if not in memory
    bool inBackingStore;
};

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
    std::vector<Frame> frames;
    std::map<std::string, std::map<int, PageTableEntry>> processPageTables;
    int pageFaultCount = 0;

    void initializeMemory();
    void generateMemorySnapshot();
    int calculateExternalFragmentation();
    int calculateExternalFragmentationInternal();
    std::string formatMemoryLayout();
    std::string formatMemoryLayoutInternal();
    int getProcessesInMemoryInternal();
    int findFreeFrame() const;
    int selectVictimFrame() const;

public:
    MemoryManager(int totalMem, int memPerProc, int memPerFrame);

    bool allocateMemory(std::shared_ptr<Process> process);
    void deallocateMemory(const std::string& processName);
    void setQuantumCycle(int cycle);
    int getProcessesInMemory();
    void generateSnapshotFile();
    
    void handlePageFault(std::shared_ptr<Process> process, int pageNumber);
    void swapOutPage(const std::string& processName, int pageNumber);
    void swapInPage(const std::string& processName, int pageNumber);

    void vmstat() const;
    void processSmi(const std::string& processName) const;
};

#endif // MEMORYMANAGER_H