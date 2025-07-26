#include "PagingMemoryManager.h"
#include <fstream>
#include <iostream>
#include <algorithm>

PagingMemoryManager::PagingMemoryManager(int totalMem, int frameSz)
    : totalFrames(totalMem / frameSz), frameSize(frameSz), backingStore("csopesy-backing-store.txt") {
    frames.resize(totalFrames);
    for (int i = 0; i < totalFrames; ++i) {
        frames[i] = {i, false, -1, -1};
    }
}

bool PagingMemoryManager::allocateMemory(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    int numPages = (process->totalInstructions + frameSize - 1) / frameSize;
    std::vector<PageTableEntry> pageTable;
    for (int i = 0; i < numPages; ++i) {
        pageTable.push_back({i, -1, false});
    }
    processPageTables[process->pid] = pageTable;
    return true;
}

void PagingMemoryManager::deallocateMemory(const std::string& processName) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    // Find PID by processName (assume mapping available)
    // Remove page table and free frames
    // Remove pages from backing store
    // ...
}

void PagingMemoryManager::setQuantumCycle(int cycle) {
    currentQuantumCycle = cycle;
}

int PagingMemoryManager::getProcessesInMemory() {
    std::lock_guard<std::mutex> lock(memoryMutex);
    return static_cast<int>(processPageTables.size());
}

void PagingMemoryManager::generateSnapshotFile() {
    std::lock_guard<std::mutex> lock(memoryMutex);
    std::ofstream file("memory_stamps/memory_stamp_" + std::to_string(currentQuantumCycle) + ".txt");
    if (file.is_open()) {
        file << "Paging Memory Snapshot\n";
        file << "Quantum Cycle: " << currentQuantumCycle << "\n";
        file << "Total Frames: " << totalFrames << "\n";
        file << "Frame Size: " << frameSize << "\n";
        for (const auto& frame : frames) {
            file << "Frame " << frame.frameNumber << ": ";
            if (frame.isAllocated) {
                file << "PID " << frame.processPID << " Page " << frame.pageNumber << "\n";
            } else {
                file << "Free\n";
            }
        }
        file.close();
    }
}

bool PagingMemoryManager::accessPage(int pid, int pageNumber) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    auto& pageTable = processPageTables[pid];
    if (pageNumber < 0 || pageNumber >= static_cast<int>(pageTable.size())) return false;
    if (!pageTable[pageNumber].valid) {
        handlePageFault(pid, pageNumber);
        return false;
    }
    // Update replacement algorithm state (FIFO)
    frameFIFO.push_back(pageTable[pageNumber].frameNumber);
    return true;
}

void PagingMemoryManager::handlePageFault(int pid, int pageNumber) {
    // Find a free frame
    int freeFrame = -1;
    for (auto& frame : frames) {
        if (!frame.isAllocated) {
            freeFrame = frame.frameNumber;
            break;
        }
    }
    if (freeFrame == -1) {
        evictPage();
        // Try again
        for (auto& frame : frames) {
            if (!frame.isAllocated) {
                freeFrame = frame.frameNumber;
                break;
            }
        }
    }
    if (freeFrame != -1) {
        frames[freeFrame].isAllocated = true;
        frames[freeFrame].processPID = pid;
        frames[freeFrame].pageNumber = pageNumber;
        processPageTables[pid][pageNumber].frameNumber = freeFrame;
        processPageTables[pid][pageNumber].valid = true;
        // Simulate loading from backing store
        BackingStorePage page{pid, pageNumber, "page_data"};
        backingStore.savePage(page);
    }
}

void PagingMemoryManager::evictPage() {
    // FIFO: evict the oldest frame
    if (!frameFIFO.empty()) {
        int frameToEvict = frameFIFO.front();
        frameFIFO.erase(frameFIFO.begin());
        Frame& frame = frames[frameToEvict];
        BackingStorePage page{frame.processPID, frame.pageNumber, "page_data"};
        backingStore.savePage(page);
        // Invalidate page table entry
        auto& pageTable = processPageTables[frame.processPID];
        for (auto& entry : pageTable) {
            if (entry.frameNumber == frame.frameNumber) {
                entry.valid = false;
                entry.frameNumber = -1;
            }
        }
        frame.isAllocated = false;
        frame.processPID = -1;
        frame.pageNumber = -1;
    }
}
