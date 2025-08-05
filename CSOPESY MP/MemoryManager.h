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
#include <unordered_map>
#include <deque>
#include "Process.h"

struct MemoryBlock {
    int startAddress;
    int size;
    std::string processName;
    bool isAllocated;

    MemoryBlock(int start, int sz, const std::string& proc = "", bool allocated = false)
        : startAddress(start), size(sz), processName(proc), isAllocated(allocated) {}
};

// Frame structure for demand paging
struct Frame {
    int frameNumber;
    std::string processName;
    int pageNumber; // Virtual page number
    bool isOccupied;
    bool dirty;
    Frame(int num) : frameNumber(num), processName(""), pageNumber(-1), isOccupied(false), dirty(false) {}
};

// Page table entry for a process
struct PageTableEntry {
    int frameNumber; // -1 if not in memory
    bool inMemory;
    bool dirty;
    PageTableEntry() : frameNumber(-1), inMemory(false), dirty(false) {}
    PageTableEntry(int frame, bool inMem, bool isDirty) : frameNumber(frame), inMemory(inMem), dirty(isDirty) {}
};

class MemoryManager {
private:
    std::vector<MemoryBlock> memoryBlocks;
    int totalMemory;
    int memoryPerProcess;
    int memoryPerFrame;
    std::mutex memoryMutex;
    int currentQuantumCycle;

    // Demand paging members
    int numFrames;
    int pageSize;
    std::vector<Frame> frames; // Physical memory frames
    std::unordered_map<std::string, std::unordered_map<int, PageTableEntry>> pageTables; // processName -> (pageNumber -> entry)
    std::deque<int> frameQueue; // FIFO for page replacement
    std::string backingStoreFile = "csopesy-backing-store.txt";



    void initializeMemory();
    void generateMemorySnapshot();
    int calculateExternalFragmentation();
    int calculateExternalFragmentationInternal();
    std::string formatMemoryLayout();
    std::string formatMemoryLayoutInternal();
    int getProcessesInMemoryInternal();

    // Demand paging helpers (to be implemented in next phases)
    void loadPageFromBackingStore(const std::string& processName, int pageNumber, int frameNumber);
    void writePageToBackingStore(const std::string& processName, int pageNumber, int frameNumber);
    int findFreeFrame();
    int selectVictimFrame();
    void handlePageFault(const std::string& processName, int pageNumber);

public:
    // Counters for vmstat reporting
    int pageInCount = 0; // Number of pages paged in
    int pageOutCount = 0; // Number of pages paged out

    MemoryManager(int totalMem, int memPerProc, int memPerFrame);

    bool allocateMemory(std::shared_ptr<Process> process);
    void deallocateMemory(const std::string& processName);
    void setQuantumCycle(int cycle);
    int getProcessesInMemory();
    void generateSnapshotFile();

    // Demand paging API
    void accessMemory(const std::string& processName, int virtualAddress, bool isWrite);
    void contextSwitchOut(const std::string& processName);
    void contextSwitchIn(const std::string& processName);

    // Expose read-only accessors for summary/statistics
    int getTotalMemory() const { return totalMemory; }
    int getNumFrames() const { return numFrames; }
    const std::vector<MemoryBlock>& getMemoryBlocks() const { return memoryBlocks; }
    const std::vector<Frame>& getFrames() const { return frames; }
    const std::unordered_map<std::string, std::unordered_map<int, PageTableEntry>>& getPageTables() const { return pageTables; }
    // int getPageInCount() const { return pageInCount; }
    // int getPageOutCount() const { return pageOutCount; }
};

#endif // MEMORYMANAGER_H