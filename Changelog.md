Full code Refactor changelog as of 6/6/2025

Main.cpp
has a function that receives string initializer set to notinitialized 

consoleLayout.cpp
  No longer has any display elements
  Connects to various functions via the Functions.cpp and gFunctions.cpp
  Creates processes via Process.cpp
 **NEW** properly identifies -r and -s inputs
 **ISSUE** -s defaults to creating a new process if command doesn't exist 

Process.cpp
  **ISSUE**: not sure how to properly make ProcessInfo a class without breaking the entire thing
  Explanation:
    _createProcess_ - makes process via taking an input of string from consoleLayout(if there is no command that matches from -s)
    _displayProcessInfo_ - looks for and shows processes made via screen -r <process name>
  **ISSUE**: displayAllProcessInfo - used to work but now cannot find any created processes even though displayProcessInfo works
    _formatTimestamp_- formats timestamps

gFunctions.cpp
  Houses general functions that are useful across the board
  Explanation
    _clearScreen_ - clears screen
    _gotoxy_ - uses windows console functions to go to a specific x and y coordinate on the CMD
    _clearLine_ - clears specific x coordinate lines

Functions.cpp
  Houses the specific function calls for screen scheduler-test etc.

Display.cpp
  Has all the output couts we have including my individual dummy Nivada CLI XD

Windows_console_minimal.h
  Has some of the functions in windows.h had to make it since windows.h and one of our .h files hate each other leading to a byte not defined scenario (best not to touch i also dont fully get it)
