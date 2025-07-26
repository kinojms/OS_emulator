#include "BackingStoreManager.h"
#include <sstream>
#include <iostream>
#include <algorithm>

BackingStoreManager::BackingStoreManager(const std::string& filename)
    : backingStoreFile(filename) {}

void BackingStoreManager::savePage(const BackingStorePage& page) {
    std::lock_guard<std::mutex> lock(fileMutex);
    std::ofstream file(backingStoreFile, std::ios::app);
    if (file.is_open()) {
        file << page.pid << " " << page.pageNumber << " " << page.data << "\n";
        file.close();
    }
}

bool BackingStoreManager::loadPage(int pid, int pageNumber, BackingStorePage& outPage) {
    std::lock_guard<std::mutex> lock(fileMutex);
    std::ifstream file(backingStoreFile);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int filePid, filePageNum;
        std::string data;
        if (iss >> filePid >> filePageNum >> data) {
            if (filePid == pid && filePageNum == pageNumber) {
                outPage = {filePid, filePageNum, data};
                return true;
            }
        }
    }
    return false;
}

void BackingStoreManager::removeProcessPages(int pid) {
    std::lock_guard<std::mutex> lock(fileMutex);
    std::ifstream file(backingStoreFile);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int filePid, filePageNum;
        std::string data;
        if (iss >> filePid >> filePageNum >> data) {
            if (filePid != pid) {
                lines.push_back(line);
            }
        }
    }
    file.close();
    std::ofstream outFile(backingStoreFile, std::ios::trunc);
    for (const auto& l : lines) {
        outFile << l << "\n";
    }
    outFile.close();
}

std::vector<BackingStorePage> BackingStoreManager::getAllPages() {
    std::lock_guard<std::mutex> lock(fileMutex);
    std::vector<BackingStorePage> pages;
    std::ifstream file(backingStoreFile);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int filePid, filePageNum;
        std::string data;
        if (iss >> filePid >> filePageNum >> data) {
            pages.push_back({filePid, filePageNum, data});
        }
    }
    return pages;
}
