#include <iostream>  
#include <cstdlib>  
#include <chrono>  
#include "Process.h"  
#include <map>
#include <iomanip>
#include <sstream> 
#include <thread>

struct ProcessInfo {
	std::string name;
	int currentLine;
	int totalLines;
	std::chrono::system_clock::time_point timeCreated;
};

void Process::createProcess(const std::string& name)  
{  
	ProcessInfo info;  
	info.name = name;  
	info.currentLine = 0;  
	info.totalLines = 100;  
	info.timeCreated = std::chrono::system_clock::now();  
	Process::processes[name] = info;  
	std::cout << "Process " << name << " created." << std::endl;  
}

void Process::displayProcessInfo(const std::string& name)
{
	auto it = processes.find(name);
	if (it != processes.end()) {
		const ProcessInfo& info = it->second;
		std::cout << "Process Name: " << info.name << std::endl;
		std::cout << "Current Line: " << info.currentLine << std::endl;
	    std::cout << "Total Lines: " << info.totalLines << std::endl;
		std::cout << "Time Created: " << formatTimestamp(info.timeCreated) << std::endl;
	}
	else {
		std::cout << "Process not found: " << name << std::endl;
	}
}

void Process::displayAllProcessInfo()
{
    int CPUUtilization = 100;
    int CoresUsed = 2;
    int CoresAvailable = 0;

    if (processes.empty()) {
        std::cout << "No processes are currently running." << std::endl;
        return;
    }
    std::cout << "CPU Utilization: " << CPUUtilization << "%" << std::endl;
    std::cout << "Cores Used: " << CoresUsed << std::endl;
    std::cout << "Cores Available: " << CoresAvailable << std::endl;
    std::cout << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "Running Process:" << std::endl;

    for (const auto& pair : processes) {
        const ProcessInfo& info = pair.second;
        std::cout << info.name << "      " << formatTimestamp(info.timeCreated);
        std::string fixedPart = info.name + "      " + formatTimestamp(info.timeCreated);

        for (int count = 1; count <= 100; ++count) {
            std::cout << "\r" << fixedPart << "      " << count << std::flush << "|100";
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        std::cout << std::endl;
    }
}

std::string Process::formatTimestamp(const std::chrono::system_clock::time_point& time)
{  
    auto time_t = std::chrono::system_clock::to_time_t(time);  
    struct tm localTime;  
    localtime_s(&localTime, &time_t);  
    std::stringstream ss;  
    ss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");  
    return ss.str();  
}
