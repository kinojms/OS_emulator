#pragma once

#include <cstdint>

// Simple types used by console APIs
typedef struct {
    short X;
    short Y;
} COORD;

typedef struct {
    short Left;
    short Top;
    short Right;
    short Bottom;
} SMALL_RECT;

typedef struct {
    COORD dwSize;
    COORD dwCursorPosition;
    uint16_t wAttributes;
    SMALL_RECT srWindow;
    COORD dwMaximumWindowSize;
} CONSOLE_SCREEN_BUFFER_INFO;

// Constants
#define STD_OUTPUT_HANDLE ((unsigned long)-11)

// Import only the required WinAPI functions
extern "C" {
    __declspec(dllimport) void* __stdcall GetStdHandle(unsigned long nStdHandle);
    __declspec(dllimport) int __stdcall SetConsoleCursorPosition(void* hConsoleOutput, COORD pos);
    __declspec(dllimport) int __stdcall GetConsoleScreenBufferInfo(void* hConsoleOutput, CONSOLE_SCREEN_BUFFER_INFO* info);
}