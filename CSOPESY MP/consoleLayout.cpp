#include "consoleLayout.h"
#include <algorithm>

consoleLayout::consoleLayout()
    : running(true)
{
    // initialize any other members if needed
}

void consoleLayout::displayIntro()
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

void consoleLayout::displayCommands()
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

void consoleLayout::run() // replicates the main function in main.cpp; this is where the functions are used
{
    string line;
    bool isInitialized = false;

    displayIntro();

    while (true) {
        cout << "\nEnter a command: ";
        getline(cin, line);
        transform(line.begin(), line.end(), line.begin(), ::tolower); // convert to lower case

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

        if (line.find("screen -s") == string::npos) { // if substring "screen -s" is not found
            cout << "You must start with 'screen -s' to run a command." << endl;
            continue;
        }

        istringstream iss(line);
        string token, flag, command;
        iss >> token >> flag >> command; // separates user input into three variables 1) token 2) flag 3) command

        if (token != "screen" || flag != "-s") {
            cout << "You must start with 'screen -s' to run a command." << endl;
            continue;
        }

        if (command.empty()) {
            cout << "You must enter a command after 'screen -s'." << endl;
            continue;
        }

        // these will be keywords 
        static unordered_map<string, string> actions = {
            {"screen", "Screen command recognized. Doing something."},
            {"scheduler-test", "scheduler-test command recognized. Doing something."},
            {"scheduler-stop", "scheduler-stop command recognized. Doing something."},
            {"report-util", "report-util command recognized. Doing something."}
        };

        if (command == "command") {
            displayCommands();
        }
        else if (actions.count(command)) {
            cout << actions[command] << endl;
        }
        else {
            cout << "Unknown command after 'screen -s': " << command << endl;
        }
    }
}

void consoleLayout::createProcess(const string& name)
{
	// TODO: implement process creation logic
}

void consoleLayout::displayProcessInfo(const string& name) const
{
	// TODO: implement creation of new screen session (maybe in a different method?)
    // and the process info display logic
}

// maybe add a new method to handle the creation of new screen sesh...

// add deconstructors to clean memory?
