#pragma once
#include "Screen.h"
#include "ProcessInfo.h"
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;

class ProcessScreen : public Screen {
public:

    ProcessScreen(const string& name, unordered_map<string, ProcessInfo>& allProcs)
        : processName(name), processes(allProcs) {
    }

    void display() override {
        system("cls");
        const auto& info = processes.at(processName);

        cout << "====================\n";
        cout << "  Process View: " << info.name << '\n';
        cout << "====================\n";
        cout << "Current Line: " << info.currentLine << '\n';
        cout << "Total Lines: " << info.totalLines << '\n';
        cout << "Time Created: " << formatTimestamp(info.timeCreated) << '\n';
        cout << "\n[Type 'exit' to go back, 'screen -r <name>' to open another]\n";
    }

    bool handleInput(string& input) override {
        cout << "Enter command: ";
        getline(cin, input);

        if (input == "exit") return true; // Pop screen

        if (input.rfind("screen -r ", 0) == 0) {
            string targetName = input.substr(10);
            if (processes.find(targetName) == processes.end()) {
                cout << "Process not found: " << targetName << endl;
            }
            else {
                // Push another screen
                nextProcessToPush = targetName;
            }
        }
        else {
            cout << "Unrecognized command." << endl;
        }

        return false; // Stay on screen
    }

    string getNextScreenName() const {
        return nextProcessToPush;
    }

private:
    string processName;
    unordered_map<string, ProcessInfo>& processes;
    string nextProcessToPush;

    string formatTimestamp(const system_clock::time_point& time) const {
        auto time_t = system_clock::to_time_t(time);
        struct tm localTime;
        localtime_s(&localTime, &time_t);
        stringstream ss;
        ss << put_time(&localTime, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};
