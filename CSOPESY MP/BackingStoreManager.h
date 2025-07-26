#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <mutex>

struct BackingStorePage {
    int pid;
    int pageNumber;
    std::string data; // Simulated page data
};

class BackingStoreManager {
public:
    BackingStoreManager(const std::string& filename);
    void savePage(const BackingStorePage& page);
    bool loadPage(int pid, int pageNumber, BackingStorePage& outPage);
    void removeProcessPages(int pid);
    std::vector<BackingStorePage> getAllPages();
private:
    std::string backingStoreFile;
    std::mutex fileMutex;
};
