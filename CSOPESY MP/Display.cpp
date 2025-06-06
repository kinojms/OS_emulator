#include <iostream>
#include <thread>
#include "Display.h"
#include "gFunctions.h"
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
<< _GREEN << "Hello, welcome to CSOPESY command line!" << _END_COLOR << '\n'
<< _YELLOW << "Type 'exit' to quit, 'clear' to clear the screen, 'command' to show all commands" << _END_COLOR << '\n';
}

void Display::displayCommands()
{
    std::cout
        << " initialize\n"
        << " screen -s screen\n"
        << " screen -s scheduler-test\n"
        << " screen -s scheduler-stop\n"
        << " screen -s report-util\n"
        << " screen -s NivadaDummy\n"
		<< " screen -r <name of process>\n"
        << " clear\n"
        << " exit\n";
}

void Display::nivadaDummy() {
    gFunctions gF;
    std::string text1 = "GPU Name: NVIDIA GeForce GTX 1080";
    std::string text2 = "Driver Version: 551.86";
    std::string text3 = "CUDA Version: 12.4";
    int trigger = 1;

    while (trigger == true) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

        int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        std::string line(width, '-');

        for (int y = 1; y <= height; ++y) {
            gF.clearLine(y, width);
        }

        gF.gotoxy(0, 1); std::cout << text1;
        gF.gotoxy((width - static_cast<int>(text2.length())) / 2, 1); std::cout << text2;
        gF.gotoxy(width - static_cast<int>(text3.length()), 1); std::cout << text3;
        gF.gotoxy(0, 2); std::cout << line;

        // Divide line into 3 columns for line 3
        int cols = 3;
        int colWidth = width / cols;

        gF.gotoxy(0, 3);
        std::cout << "GPU  Name              TCC/WDDM |" << std::string(colWidth - 20, ' ');
        gF.gotoxy(colWidth, 3);
        std::cout << "Bus-Id        Disp.A |" << std::string(colWidth - 18, ' ');
        gF.gotoxy(colWidth * 2, 3);
        std::cout << "Volatile  Uncorr.  ECC" << std::string(colWidth - 15, ' ');

        gF.gotoxy(0, 4);
        std::cout << "Fan  Temp  Pref   Pwr:Usage/Cap |" << std::string(colWidth - 20, ' ');
        gF.gotoxy(colWidth, 4);
        std::cout << "        Memory-Usage |" << std::string(colWidth - 18, ' ');
        gF.gotoxy(colWidth * 2, 4);
        std::cout << "GPU-Util    Compute M." << std::string(colWidth - 15, ' ');

        gF.gotoxy(colWidth * 2, 5);
        std::cout << "                Mig M." << std::string(colWidth - 15, ' ');

        gF.gotoxy(0, 6); std::cout << line;
        gF.gotoxy(0, 7); std::cout << std::endl;
        gF.gotoxy(0, 8); std::cout << "Process";

        int cols2 = 4;
        int colWidth2 = width / cols2;

        gF.gotoxy(0, 9);
        std::cout << "GPU    GI    CI" << std::string(colWidth - 20, ' ');
        gF.gotoxy(colWidth2, 9);
        std::cout << "PID    Type" << std::string(colWidth - 15, ' ');
        gF.gotoxy(colWidth2 * 2, 9);
        std::cout << "Process Name                                  " << std::string(colWidth - 18, ' ');
        gF.gotoxy(colWidth2 * 3, 9);
        std::cout << "GPU Memory" << std::string(colWidth - 12, ' ');

        gF.gotoxy(0, 10);
        std::cout << "       ID    ID" << std::string(colWidth - 20, ' ');
        gF.gotoxy(colWidth2 * 3, 10);
        std::cout << "Usage" << std::string(colWidth - 12, ' ');

        gF.gotoxy(0, 11); std::cout << line;

        gF.gotoxy(0, 12);
        std::cout << " 0    N/A    N/A" << std::string(colWidth - 25, ' ');
        gF.gotoxy(colWidth2, 12);
        std::cout << "1234    C+G" << std::string(colWidth - 20, ' ');
        gF.gotoxy(colWidth2 * 2, 12);
        std::cout << "D:\School\CSNETWK\SRC\com.exe         " << std::string(colWidth - 18, ' ');
        gF.gotoxy(colWidth2 * 3, 12);
        std::cout << "N/A" << std::string(colWidth - 12, ' ');

        gF.gotoxy(0, 13);
        std::cout << " 0    N/A    N/A" << std::string(colWidth - 15, ' ');
        gF.gotoxy(colWidth2, 13);
        std::cout << "4321    C+G" << std::string(colWidth - 20, ' ');
        gF.gotoxy(colWidth2 * 2, 13);
        std::cout << "C:\Program Files\BlueStacks_nxt\B...         " << std::string(colWidth - 18, ' ');
        gF.gotoxy(colWidth2 * 3, 13);
        std::cout << "N/A" << std::string(colWidth - 12, ' ');

        gF.gotoxy(0, 14);
        std::cout << " 0    N/A    N/A" << std::string(colWidth - 15, ' ');
        gF.gotoxy(colWidth2, 14);
        std::cout << "2143    C+G" << std::string(colWidth - 20, ' ');
        gF.gotoxy(colWidth2 * 2, 14);
        std::cout << "D:\Games\GF2 Game\GF2.exe        " << std::string(colWidth - 18, ' ');
        gF.gotoxy(colWidth2 * 3, 14);
        std::cout << "N/A" << std::string(colWidth - 12, ' ');

        gF.gotoxy(0, 15);
        std::cout << " 0    N/A    N/A" << std::string(colWidth - 15, ' ');
        gF.gotoxy(colWidth2, 15);
        std::cout << "4213    C+G" << std::string(colWidth - 20, ' ');
        gF.gotoxy(colWidth2 * 2, 15);
        std::cout << "D:\STINTSY (AI advanced)\Linear-Regression\...        " << std::string(colWidth - 18, ' ');
        gF.gotoxy(colWidth2 * 3, 15);
        std::cout << "N/A" << std::string(colWidth - 12, ' ');

        gF.gotoxy(0, 16);
        std::cout << " 0    N/A    N/A" << std::string(colWidth - 15, ' ');
        gF.gotoxy(colWidth2, 16);
        std::cout << "4312    C+G" << std::string(colWidth - 20, ' ');
        gF.gotoxy(colWidth2 * 2, 16);
        std::cout << "D:\Steam\bin        " << std::string(colWidth - 18, ' ');
        gF.gotoxy(colWidth2 * 3, 16);
        std::cout << "N/A" << std::string(colWidth - 12, ' ');

        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Correct usage of std::this_thread
    }
}