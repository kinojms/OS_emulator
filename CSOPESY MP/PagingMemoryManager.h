#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <mutex>
#include "IMemoryManager.h"
#include "Process.h"
#include "BackingStoreManager.h"
#include "PagingTypes.h"

class PagingMemoryManager : public IMemoryManager {
private:
    int totalFrames;
    int frameSize;
    std::vector<Frame> frames;
    std::unordered_map<int, std::vector<PageTableEntry>> processPageTables; // pid -> page table
    std::mutex memoryMutex;
    int currentQuantumCycle = 0;
    BackingStoreManager backingStore;
    // Backing store file name
    std::string backingStoreFile = "csopesy-backing-store.txt";
    // Page replacement algorithm state (FIFO queue)
    std::vector<int> frameFIFO;
public:
    PagingMemoryManager(int totalMem, int frameSz);
    bool allocateMemory(std::shared_ptr<Process> process) override;
    void deallocateMemory(const std::string& processName) override;
    void setQuantumCycle(int cycle) override;
    int getProcessesInMemory() override;
    void generateSnapshotFile() override;
    // Demand paging API
    bool accessPage(int pid, int pageNumber);
    void handlePageFault(int pid, int pageNumber);
    void evictPage();
};
