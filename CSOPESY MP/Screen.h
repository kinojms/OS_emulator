#pragma once
#include <string>
using namespace std;

// Abstract base class for all screens
class Screen {
public:
    virtual ~Screen() = default;

    // Display screen contents
    virtual void display() = 0;

    // Handle input and decide if screen should pop
    virtual bool handleInput(string& input) = 0;
};
