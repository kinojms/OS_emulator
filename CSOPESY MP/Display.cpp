#include <iostream>
#include <thread>
#include "Display.h"
#include "windows_console_minimal.h"
#define _CYAN "\033[36m"
#define _GREEN "\033[32m"
#define _YELLOW "\033[33m"
#define _END_COLOR "\033[0m"

void Display::displayIntro()
{
    std::cout << _CYAN << R"(
  ______     ______     ______     ______   ______     ______     __  __    
 /\  ___\   /\  ___\   /\  __ \   /\  == \ /\  ___\   /\  ___\   /\ \_\ \   
 \ \ \____  \ \___  \  \ \ \/\ \  \ \  _-/ \ \  __\   \ \___  \  \ \____ \  
  \ \_____\  \/\_____\  \ \_____\  \ \_\    \ \_____\  \/\_____\  \/\_____\ 
   \/_____/   \/_____/   \/_____/   \/_/     \/_____/   \/_____/   \/_____/

)" << _END_COLOR << '\n'
<< _GREEN <<    " Hello, welcome to CSOPESY command line!" << _END_COLOR << '\n'
<< _YELLOW <<   " Type 'exit' to quit, 'clear' to clear the screen, 'command' to show all commands" << _END_COLOR << '\n';
}

void Display::displayCommands()
{
    std::cout
        << "Available commands:\n"
        << " Screen commands:\n"
        << " screen -r <name of existing process>\n"
        << " screen -s <name of new process> <memory size>\n"
        << " screen -c <name of new process> <memory size> \"<commands>\"\n"
        << " \n"
        << " Monitoring/Logging commands:\n"
        << " screen -ls\n"
        << " report-util\n"
        << " \n"
        << " Scheduler commands:\n"
        << " scheduler-start\n"
        << " scheduler-stop\n"
        << " \n"
        << " Misc commands:\n"
        << " clear\n"
        << " exit\n";
}