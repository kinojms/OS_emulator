void intro() {
    std::cout << "\033[36m" << R"(
 ______     ______     ______     ______   ______     ______     __  __    
/\  ___\   /\  ___\   /\  __ \   /\  == \ /\  ___\   /\  ___\   /\ \_\ \   
\ \ \____  \ \___  \  \ \ \/\ \  \ \  _-/ \ \  __\   \ \___  \  \ \____ \  
 \ \_____\  \/\_____\  \ \_____\  \ \_\    \ \_____\  \/\_____\  \/\_____\ 
  \/_____/   \/_____/   \/_____/   \/_/     \/_____/   \/_____/   \/_____/ 
)" << "\033[0m\n"
<< "\033[32m  Hello, welcome to CSOPESY command line!\033[0m\n"
<< "\033[33m  Type 'exit' to quit, 'clear' to clear the screen, 'command' to show all commands\033[0m\n";
}

void showCommands() {
    std::cout << " (1) initialize\n (2) screen -s screen\n (3) screen -s scheduler-test\n"
        << " (4) screen -s scheduler-stop\n (5) screen -s report-util\n"
        << " (6) clear\n (7) exit\n";
}