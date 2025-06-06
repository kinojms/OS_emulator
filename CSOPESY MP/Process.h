#include <string>
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <conio.h>

class Process  
{  
public:  
	struct ProcessInfo {  
		std::string name;  
		int currentLine;  
		int totalLines;  
		std::chrono::system_clock::time_point timeCreated;  
	};  
	std::unordered_map<std::string, ProcessInfo> processes;

public:  
	void createProcess(const std::string& name);  
	void displayProcessInfo(const std::string& name);
	void displayAllProcessInfo();
	std::string formatTimestamp(const std::chrono::system_clock::time_point& time);  
};
