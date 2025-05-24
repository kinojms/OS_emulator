#include "ConsoleLayout.h"
#include <algorithm>

ConsoleLayout::ConsoleLayout()
    : running(true)
{
    // initialize any other members if needed
}

void ConsoleLayout::displayIntro()
{
    std::cout << _CYAN << R"(
 ______     ______     ______     ______   ______     ______     __  __    
/\  ___\   /\  ___\   /\  __ \   /\  == \ /\  ___\   /\  ___\   /\ \_\ \   
\ \ \____  \ \___  \  \ \ \/\ \  \ \  _-/ \ \  __\   \ \___  \  \ \____ \  
 \ \_____\  \/\_____\  \ \_____\  \ \_\    \ \_____\  \/\_____\  \/\_____\ 
  \/_____/   \/_____/   \/_____/   \/_/     \/_____/   \/_____/   \/_____/
 
)" << _END_COLOR << '\n'
<< _GREEN << "Hello, welcome to CSOPESY command line!" << _END_COLOR << '\n'
<< _YELLOW << "Type 'exit' to quit, 'clear' to clear the screen, 'command' to show all commands" << _END_COLOR << '\n';
}

void ConsoleLayout::displayCommands()
{
    std::cout 
        << " (1) initialize\n"
        << " (2) screen -s screen\n"
        << " (3) screen -s scheduler-test\n"
        << " (4) screen -s scheduler-stop\n"
        << " (5) screen -s report-util\n"
        << " (6) clear\n"
        << " (7) exit\n";
}

void ConsoleLayout::run() {
    string line;
    bool isInitialized = false;

    displayIntro();

    while (true) {
        cout << "\nEnter a command: ";
        getline(cin, line);

        if (line == "exit") break;

        if (line == "clear") {
            system("cls");
            displayIntro();
            continue;
        }

        if (line == "initialize") {
            isInitialized = true;
            cout << "Initialized successfully." << endl;
            continue;
        }

        if (!isInitialized) {
            cout << "You must initialize before using other commands." << endl;
            continue;
        }

        // Parse the input
        istringstream iss(line);
        string token, flag, command;
        iss >> token >> flag >> command;

        // Handle "screen -r <process>"
        if (token == "screen" && flag == "-r") {
            if (command.empty()) {
                cout << "You must specify a process to resume using 'screen -r'." << endl;
            }
			else if (processes.find(command) == processes.end()) {
				cout << "Process not found: " << command << endl;
                continue;
			}
            else {
                system("cls");
                /*displayIntro();*/
                while (true) {
                    system("cls");
                    displayProcessInfo(command);
					cout << "Enter command: ";
					getline(cin, line);
                    if (line == "exit") {
                        system("cls");
                        displayIntro();
                        break;
                    }
                    else {
                        cout << "Please exit first before entering a new command" << endl;
                        cout << "Press any key to continue..." << endl;
                        _getch();
                        continue;
                    }
                }
            }
            continue;
        }

        // Validate for "screen -s" before proceeding
        if (token != "screen" || flag != "-s") {
            cout << "You must start with 'screen -s' to run a command." << endl;
            continue;
        }

        if (command.empty()) {
            cout << "You must enter a command after 'screen -s'." << endl;
            continue;
        }

        // Define valid actions
        static const unordered_map<string, string> actions = {
            {"screen", "Screen command recognized. Doing something."},
            {"scheduler-test", "scheduler-test command recognized. Doing something."},
            {"scheduler-stop", "scheduler-stop command recognized. Doing something."},
            {"report-util", "report-util command recognized. Doing something."}
        };

        // Handle recognized screen -s commands
        if (command == "command") {
            displayCommands();
        }
        else if (actions.count(command)) {
            cout << actions.at(command) << endl;
        }
        else {
            // Prevent creating duplicate processes
            if (processes.find(command) != processes.end()) {
                cout << "Process '" << command << "' already exists. Duplicate processes are not allowed." << endl;
            } else {
                createProcess(command);
            }
        }
    }
}


void ConsoleLayout::createProcess(const string& name)
{
	ProcessInfo info;
	info.name = name;
	info.currentLine = 0;
	info.totalLines = 100;
	info.timeCreated = system_clock::now();
	processes[name] = info;
	cout << "Process " << name << " created." << endl;
}

void ConsoleLayout::displayProcessInfo(const string& name) const
{
	auto it = processes.find(name);
	if (it != processes.end()) {
		const ProcessInfo& info = it->second;
		cout << "Process Name: " << info.name << endl;
		cout << "Current Line: " << info.currentLine << endl;
		cout << "Total Lines: " << info.totalLines << endl;
		cout << "Time Created: " << formatTimestamp(info.timeCreated) << endl;
	}
	else {
		cout << "Process not found: " << name << endl;
	}
}

string ConsoleLayout::formatTimestamp(const system_clock::time_point& time) const
{
    auto time_t = system_clock::to_time_t(time);
    struct tm localTime;
    localtime_s(&localTime, &time_t);
    stringstream ss;
    ss << put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}


// maybe add a new method to handle the creation of new screen sesh...

// add deconstructors to clean memory?
