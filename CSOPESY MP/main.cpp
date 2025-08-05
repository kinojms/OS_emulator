#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include "MemoryManager.h"
#include "Process.h"
#include "consoleLayout.h"
#include "Display.h"
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>

Display disp;
using namespace std;

// Test function to observe demand paging, page faults, and backing store
void testDemandPaging() {
    std::cout << "\n--- Demand Paging Test Start ---\n";
    // Small memory to force page faults and replacement
    int totalMem = 4096; // 4096 bytes
    int memPerProc = 2048; // 2048 bytes per process
    int memPerFrame = 1024; // 1024 bytes per frame (4 frames)
    MemoryManager mm(totalMem, memPerProc, memPerFrame);

    auto procA = std::make_shared<Process>(0, "A", memPerProc);
    procA->setMemoryManager(&mm);
    mm.allocateMemory(procA);

    // Access 3 different pages (should fit in memory)
    for (int i = 0; i < 3; ++i) {
        int addr = i * memPerFrame;
        std::cout << "[TEST] Process A reading address " << addr << " (page " << i << ")" << std::endl;
        mm.accessMemory("A", addr, false);
    }
    // Access a 4th page (should cause a page fault and possibly replacement)
    int addr4 = 3 * memPerFrame;
    std::cout << "[TEST] Process A writing address " << addr4 << " (page 3)" << std::endl;
    mm.accessMemory("A", addr4, true);

    // Access the first page again (should cause a page fault if FIFO replacement occurred)
    std::cout << "[TEST] Process A reading address 0 (page 0) again (should cause page fault if replaced)" << std::endl;
    mm.accessMemory("A", 0, false);

    // Check backing store file
    std::ifstream backingFile("csopesy-backing-store.txt");
    std::cout << "\n[TEST] Backing store contents after test:\n";
    std::string line;
    while (std::getline(backingFile, line)) {
        std::cout << line << std::endl;
    }
    backingFile.close();
    std::cout << "--- Demand Paging Test End ---\n\n";
}

// Test function to observe demand paging, page faults, and backing store with eviction
void testDemandPagingEviction() {
    std::cout << "\n--- Demand Paging Eviction Test Start ---\n";
    // Clear the backing store file for clarity
    std::ofstream clearFile("csopesy-backing-store.txt", std::ios::trunc);
    clearFile.close();

    int totalMem = 4096; // 4096 bytes
    int memPerProc = 4096; // 4096 bytes per process (to allow 5+ pages)
    int memPerFrame = 1024; // 1024 bytes per frame (4 frames)
    MemoryManager mm(totalMem, memPerProc, memPerFrame);

    auto procA = std::make_shared<Process>(0, "A", memPerProc);
    procA->setMemoryManager(&mm);
    mm.allocateMemory(procA);

    // Access 6 unique pages (0-5), which is more than the number of frames (4)
    for (int i = 0; i < 6; ++i) {
        int addr = i * memPerFrame;
        std::cout << "[TEST] Process A writing address " << addr << " (page " << i << ")" << std::endl;
        mm.accessMemory("A", addr, true);
    }

    // Re-access the first two pages to trigger more evictions
    for (int i = 0; i < 2; ++i) {
        int addr = i * memPerFrame;
        std::cout << "[TEST] Process A reading address " << addr << " (page " << i << ") again (should cause eviction if not resident)" << std::endl;
        mm.accessMemory("A", addr, false);
    }

    // Check backing store file
    std::ifstream backingFile("csopesy-backing-store.txt");
    std::cout << "\n[TEST] Backing store contents after eviction test:\n";
    std::string line;
    while (std::getline(backingFile, line)) {
        std::cout << line << std::endl;
    }
    backingFile.close();
    std::cout << "--- Demand Paging Eviction Test End ---\n\n";
}

int main() {
    // Run demand paging eviction test
    // testDemandPagingEviction();

    // Start the main console as usual
    consoleLayout layout;
    std::string initializer = "notinitialized";
    layout.controller(initializer);
}
