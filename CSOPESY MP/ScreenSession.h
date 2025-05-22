#ifndef SCREENSESSION_H
#define SCREENSESSION_H

#include <iostream>
#include <string>

class ScreenSession {
private:
    std::string processName;
    int currentLine;
    int totalLines;
    std::string timestamp;

    std::string getCurrentTimestamp() const;

public:
    // Constructor
    ScreenSession();
    explicit ScreenSession(const std::string& name);

    // Getters
    std::string getProcessName() const;
    int getCurrentLine() const;
    int getTotalLines() const;
    std::string getTimestamp() const;

    // Mutators
    void advanceInstruction();
    void resetInstruction();

    // Display the screen layout
    void display() const;
};

#endif // SCREENSESSION_H
