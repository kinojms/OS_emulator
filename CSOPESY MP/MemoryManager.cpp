#include "MemoryManager.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <map>
#include <iomanip>

MemoryManager::MemoryManager(int totalMem, int memPerProc, int memPerFrame)
    : totalMemory(totalMem), memoryPerProcess(memPerProc), memoryPerFrame(memPerFrame), currentQuantumCycle(0) {
    initializeMemory();
}

void MemoryManager::initializeMemory() {
    // Start with one free block covering the entire memory
    memoryBlocks.clear();
    memoryBlocks.emplace_back(0, totalMemory, "", false);
}

// Add these to your MemoryManager class (private section)
std::vector<Frame> frames; // Physical memory frames
std::map<std::string, std::map<int, PageTableEntry>> processPageTables; // processName -> (pageNumber -> PageTableEntry)
int pageFaultCount = 0;

// Helper: Find a free frame index, or -1 if none
int MemoryManager::findFreeFrame() const {
    for (size_t i = 0; i < frames.size(); ++i) {
        if (!frames[i].isOccupied) return static_cast<int>(i);
    }
    return -1;
}

// Helper: Select a victim frame for replacement (FIFO here, can use LRU)
int MemoryManager::selectVictimFrame() const {
    for (size_t i = 0; i < frames.size(); ++i) {
        if (frames[i].isOccupied) return static_cast<int>(i);
    }
    return -1;
}

bool MemoryManager::allocateMemory(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(memoryMutex);

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
        file << "ProcessName PageNumber Data\n";
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

void MemoryManager::swapOutPage(const std::string& processName, int pageNumber) {
    std::string pageData;
    // Find the frame containing this page
    for (const auto& frame : frames) {
        if (frame.isOccupied && frame.processName == processName && frame.pageNumber == pageNumber) {
            pageData = frame.data;
            break;
        }
    }
    std::ofstream backingStore("csopesy-backing-store.txt", std::ios::app);
    backingStore << processName << " " << pageNumber << " " << pageData << "\n";
    backingStore.close();
}

void MemoryManager::swapInPage(const std::string& processName, int pageNumber) {
    std::ifstream backingStore("csopesy-backing-store.txt");
    std::string line;
    std::string pageData;
    while (std::getline(backingStore, line)) {
        std::istringstream iss(line);
        std::string pname;
        int pnum;
        iss >> pname >> pnum;
        if (pname == processName && pnum == pageNumber) {
            std::getline(iss, pageData); // Get the rest as data
            break;
        }
    }
    backingStore.close();


    int frameIdx = findFreeFrame();
    if (frameIdx == -1) return; // No free frame, should not happen here

    frames[frameIdx] = { frameIdx, processName, pageNumber, true, pageData };
    processPageTables[processName][pageNumber] = { frameIdx, false };
}

void MemoryManager::handlePageFault(std::shared_ptr<Process> process, int pageNumber) {
    ++pageFaultCount;
    int frameIdx = findFreeFrame();
    if (frameIdx != -1) {
        swapInPage(process->processName, pageNumber);
    } else {
        int victimIdx = selectVictimFrame();
        if (victimIdx == -1) return; // No victim found, should not happen

        // Swap out victim
        const Frame& victim = frames[victimIdx];
        swapOutPage(victim.processName, victim.pageNumber);

        // Update victim's page table
        processPageTables[victim.processName][victim.pageNumber] = { -1, true };

        // Swap in requested page
        frames[victimIdx] = { victimIdx, process->processName, pageNumber, true, "" };
        swapInPage(process->processName, pageNumber);
        processPageTables[process->processName][pageNumber] = { victimIdx, false };
    }
}

void MemoryManager::vmstat() const {
    std::cout << "---- VMSTAT ----\n";
    std::cout << "Total frames: " << frames.size() << "\n";
    int used = 0;
    for (const auto& f : frames) if (f.isOccupied) ++used;
    std::cout << "Used frames: " << used << "\n";
    std::cout << "Free frames: " << (frames.size() - used) << "\n";
    std::cout << "Page faults: " << pageFaultCount << "\n";
    std::cout << "----------------\n";
}

void MemoryManager::processSmi(const std::string& processName) const {
    auto it = processPageTables.find(processName);
    if (it == processPageTables.end()) {
        std::cout << "Process not found.\n";
        return;
    }
    std::cout << "---- process-smi for " << processName << " ----\n";
    std::cout << std::setw(10) << "Page" << std::setw(10) << "Frame" << std::setw(15) << "Location\n";
    for (const auto& [pageNum, entry] : it->second) {
        std::cout << std::setw(10) << pageNum
                  << std::setw(10) << (entry.frameNumber == -1 ? "-" : std::to_string(entry.frameNumber))
                  << std::setw(15) << (entry.inBackingStore ? "BackingStore" : "PhysicalMem") << "\n";
    }
    std::cout << "-----------------------------\n";
}