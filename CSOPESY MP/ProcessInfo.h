#pragma once
#include <string>
#include <chrono>

struct ProcessInfo {
    std::string name;
    int currentLine;
    int totalLines;
    std::chrono::system_clock::time_point timeCreated;
};
