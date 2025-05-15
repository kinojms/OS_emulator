#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include "Display.h"

using namespace std;

int main() {
    string line;
    bool isInitialized = false;

    intro();

    while (true) {
        cout << "\nEnter a command: ";
        getline(cin, line);
        transform(line.begin(), line.end(), line.begin(), ::tolower); // convert to lower case

        if (line == "exit") break;

        if (line == "clear") {
            system("cls");
            intro();
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
        iss >> token >> flag >> command;

        if (token != "screen" || flag != "-s") {
            cout << "You must start with 'screen -s' to run a command." << endl;
            continue;
        }

        if (command.empty()) {
            cout << "You must enter a command after 'screen -s'." << endl;
            continue;
        }

        // c++ version of a python dictionary
        static unordered_map<string, string> actions = {
            {"screen", "Screen command recognized. Doing something."},
            {"scheduler-test", "scheduler-test command recognized. Doing something."},
            {"scheduler-stop", "scheduler-stop command recognized. Doing something."},
            {"report-util", "report-util command recognized. Doing something."}
        };

        if (command == "command") {
            showCommands();
        }
        else if (actions.count(command)) {
            cout << actions[command] << endl;
        }
        else {
            cout << "Unknown command after 'screen -s': " << command << endl;
        }
    }

    cout << "Code End" << endl;
    return 0;
}
