#include <string>
#include <unordered_map>
#include <chrono>
#include <iostream>

#define _CYAN "\033[36m"
#define _GREEN "\033[32m"
#define _YELLOW "\033[33m"
#define _END_COLOR "\033[0m"

using namespace std;
using namespace std::chrono;

class consoleLayout
{
public:
	// TODO: define the console layout here
	consoleLayout();
	void displayIntro();
	void displayCommands();
	void run();

private:
	struct ProcessInfo {
		string name;
		int currentLine;
		int totalLines;
		system_clock::time_point timeCreated;
	};

	void createProcess(const string& name);
	void displayProcessInfo(const string& name) const;
	string formatTimestamp(const system_clock::time_point& time) const;


	unordered_map<string, ProcessInfo> processes;
	bool running;
};

