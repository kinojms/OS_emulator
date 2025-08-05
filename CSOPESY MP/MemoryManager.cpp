#include "MemoryManager.h"
#include <iostream>
#include <algorithm>
#include <filesystem>

MemoryManager::MemoryManager(int totalMem, int memPerProc, int memPerFrame)
    : totalMemory(totalMem), memoryPerProcess(memPerProc), memoryPerFrame(memPerFrame), currentQuantumCycle(0) {
    initializeMemory();
    // Demand paging initialization
    numFrames = totalMemory / memoryPerFrame;
    pageSize = memoryPerFrame; // 1 page per frame for simplicity
    frames.reserve(numFrames);
    for (int i = 0; i < numFrames; ++i) {
        frames.emplace_back(i);
    }
}

void MemoryManager::initializeMemory() {
    // Start with one free block covering the entire memory
    memoryBlocks.clear();
    memoryBlocks.emplace_back(0, totalMemory, "", false);
}

bool MemoryManager::allocateMemory(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(memoryMutex);

    // Calculate number of pages needed for this process
    int numPages = (memoryPerProcess + pageSize - 1) / pageSize;
    auto& pt = pageTables[process->processName];
    for (int i = 0; i < numPages; ++i) {
        pt[i] = PageTableEntry(); // All pages start as not in memory
    }

    // Find the first block that can fit the process (first-fit algorithm)
    for (size_t i = 0; i < memoryBlocks.size(); ++i) {
        if (!memoryBlocks[i].isAllocated && memoryBlocks[i].size >= memoryPerProcess) {
            // Found a suitable block
            if (memoryBlocks[i].size == memoryPerProcess) {
                // Perfect fit - allocate the entire block
                memoryBlocks[i].isAllocated = true;
                memoryBlocks[i].processName = process->processName;
            }
            else {
                // Split the block
                int remainingSize = memoryBlocks[i].size - memoryPerProcess;
                int newStartAddress = memoryBlocks[i].startAddress + memoryPerProcess;

                // Update current block to allocated
                memoryBlocks[i].size = memoryPerProcess;
                memoryBlocks[i].isAllocated = true;
                memoryBlocks[i].processName = process->processName;

                // Insert new free block after current block
                memoryBlocks.insert(memoryBlocks.begin() + i + 1,
                    MemoryBlock(newStartAddress, remainingSize, "", false));
            }

            /*std::cout << "[MemoryManager] Allocated " << memoryPerProcess << " bytes for process "
                << process->processName << " at address " << memoryBlocks[i].startAddress << std::endl;*/
            return true;
        }
    }

    // No suitable block found
    /*std::cout << "[MemoryManager] Failed to allocate memory for process " << process->processName
        << ". Memory is full or fragmented." << std::endl;*/
    return false;
}

void MemoryManager::deallocateMemory(const std::string& processName) {
    std::lock_guard<std::mutex> lock(memoryMutex);

    for (size_t i = 0; i < memoryBlocks.size(); ++i) {
        if (memoryBlocks[i].isAllocated && memoryBlocks[i].processName == processName) {
            // Mark as free
            memoryBlocks[i].isAllocated = false;
            memoryBlocks[i].processName = "";

            // Try to merge with adjacent free blocks
            bool merged = false;

            // Merge with previous block if it's free
            if (i > 0 && !memoryBlocks[i - 1].isAllocated) {
                memoryBlocks[i - 1].size += memoryBlocks[i].size;
                memoryBlocks.erase(memoryBlocks.begin() + i);
                merged = true;
            }

            // Merge with next block if it's free (and we haven't already merged)
            if (!merged && i < memoryBlocks.size() - 1 && !memoryBlocks[i + 1].isAllocated) {
                memoryBlocks[i].size += memoryBlocks[i + 1].size;
                memoryBlocks.erase(memoryBlocks.begin() + i + 1);
            }

            //std::cout << "[MemoryManager] Deallocated memory for process " << processName << std::endl;
            return;
        }
    }
}

void MemoryManager::setQuantumCycle(int cycle) {
    currentQuantumCycle = cycle;
}

int MemoryManager::getProcessesInMemory() {
    std::lock_guard<std::mutex> lock(memoryMutex);
    return getProcessesInMemoryInternal();
}

int MemoryManager::calculateExternalFragmentation() {
    std::lock_guard<std::mutex> lock(memoryMutex);
    return calculateExternalFragmentationInternal();
}

int MemoryManager::getProcessesInMemoryInternal() {
    int count = 0;
    for (const auto& block : memoryBlocks) {
        if (block.isAllocated) {
            count++;
        }
    }
    return count;
}

int MemoryManager::calculateExternalFragmentationInternal() {
    int totalFree = 0;
    for (const auto& block : memoryBlocks) {
        if (!block.isAllocated) {
            totalFree += block.size;
        }
    }
    return totalFree;
}

std::string MemoryManager::formatMemoryLayout() {
    std::lock_guard<std::mutex> lock(memoryMutex);
    return formatMemoryLayoutInternal();
}

std::string MemoryManager::formatMemoryLayoutInternal() {
    std::stringstream ss;
    // Sort blocks by start address for proper display
    std::vector<MemoryBlock> sortedBlocks = memoryBlocks;
    std::sort(sortedBlocks.begin(), sortedBlocks.end(),
        [](const MemoryBlock& a, const MemoryBlock& b) {
            return a.startAddress < b.startAddress;
        });
    // Display from highest to lowest address (as per requirements)
    for (int i = static_cast<int>(sortedBlocks.size()) - 1; i >= 0; --i) {
        const auto& block = sortedBlocks[i];
        int endAddress = block.startAddress + block.size - 1;
        ss << endAddress << "\n";
        if (block.isAllocated) {
            ss << block.processName << "\n";
        }
        ss << block.startAddress << "\n\n";
    }
    return ss.str();
}

void MemoryManager::generateSnapshotFile() {
    std::lock_guard<std::mutex> lock(memoryMutex);
    // Get current timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
    char timestamp[26];
    ctime_s(timestamp, sizeof(timestamp), &timeNow);
    std::string timestampStr(timestamp);
    timestampStr.pop_back(); // Remove newline
    // Calculate metrics using internal versions to avoid deadlock
    int processesInMemory = getProcessesInMemoryInternal();
    int externalFragmentation = calculateExternalFragmentationInternal();
    // Create filename
    std::string filename = "memory_stamps/memory_stamp_" + std::to_string(currentQuantumCycle) + ".txt";
    // Write to file
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "Timestamp: " << timestampStr << "\n";
        file << "Number of processes in memory: " << processesInMemory << "\n";
        file << "Total external fragmentation in KB: " << (externalFragmentation / 1024) << "\n";
        file << "----end---- = " << totalMemory << " // This is the max\n";
        file << formatMemoryLayoutInternal();
        file << "----start---- = 0\n";
        file.close();
        //std::cout << "[MemoryManager] Generated memory snapshot: " << filename << std::endl;
    }
    else {
        //std::cerr << "[MemoryManager] Failed to create memory snapshot file: " << filename << std::endl;
    }
}

void MemoryManager::generateMemorySnapshot() {
    generateSnapshotFile();
}

// Find a free frame in physical memory, return frame number or -1 if none
int MemoryManager::findFreeFrame() {
    for (auto& frame : frames) {
        if (!frame.isOccupied) return frame.frameNumber;
    }
    return -1;
}

// FIFO page replacement: evict the oldest frame in the queue
int MemoryManager::selectVictimFrame() {
    if (frameQueue.empty()) return -1;
    int victim = frameQueue.front();
    frameQueue.pop_front();
    return victim;
}

// Stub for page fault handler (to be implemented in next phase)
void MemoryManager::handlePageFault(const std::string& processName, int pageNumber) {
    // To be implemented in Phase 4
}

// Stub for memory access (to be implemented in next phase)
void MemoryManager::accessMemory(const std::string& processName, int virtualAddress, bool isWrite) {
    // To be implemented in Phase 4
}

// Stub for context switch out (to be implemented in Phase 5)
void MemoryManager::contextSwitchOut(const std::string& processName) {
    // To be implemented in Phase 5
}

// Stub for context switch in (to be implemented in Phase 5)
void MemoryManager::contextSwitchIn(const std::string& processName) {
    // To be implemented in Phase 5
}