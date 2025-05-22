#include "ScreenSession.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

using namespace std;



ScreenSession::ScreenSession()
{
}

ScreenSession::ScreenSession(const string& name)
    : processName(name), currentLine(1), totalLines(10), timestamp(getCurrentTimestamp()) {
}

string ScreenSession::getCurrentTimestamp() const {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);

    tm localTime;
#ifdef _WIN32
    localtime_s(&localTime, &now_c);
#else
    localtime_r(&now_c, &localTime);
#endif

    char buffer[100];
    strftime(buffer, sizeof(buffer), "%m/%d/%Y, %I:%M:%S %p", &localTime);
    return string(buffer);
}

string ScreenSession::getProcessName() const {
    return processName;
}

int ScreenSession::getCurrentLine() const {
    return currentLine;
}

int ScreenSession::getTotalLines() const {
    return totalLines;
}

string ScreenSession::getTimestamp() const {
    return timestamp;
}

void ScreenSession::advanceInstruction() {
    if (currentLine < totalLines) {
        currentLine++;
    }
}

void ScreenSession::resetInstruction() {
    currentLine = 1;
}

void ScreenSession::display() const {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    cout << "==========================" << endl;
    cout << "  SCREEN SESSION: " << processName << endl;
    cout << "==========================" << endl;
    cout << "Instruction: " << currentLine << " / " << totalLines << endl;
    cout << "Started at: " << timestamp << endl;
    cout << "==========================" << endl;
    cout << "Type 'exit' to return to main menu." << endl;
}
