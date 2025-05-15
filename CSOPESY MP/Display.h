#define _CYAN "\033[36m"
#define _GREEN "\033[32m"
#define _YELLOW "\033[33m"
#define _END_COLOR "\033[0m"

void intro() {
    std::cout << _CYAN << R"(

       (       )  (       (       )  
   (   )\ ) ( /(  )\ )    )\ ) ( /(  
   )\ (()/( )\())(()/((  (()/( )\()) 
 (((_) /(_)|(_)\  /(_))\  /(_)|(_)\  
 )\___(_))   ((_)(_))((_)(_))__ ((_) 
((/ __/ __| / _ \| _ \ __/ __\ \ / / 
 | (__\__ \| (_) |  _/ _|\__ \\ V /  
  \___|___/ \___/|_| |___|___/ |_|   

)" << _END_COLOR << '\n'
<< _GREEN << "Hello, welcome to CSOPESY command line!" << _END_COLOR << '\n'
<< _YELLOW << "Type 'exit' to quit, 'clear' to clear the screen, 'command' to show all commands" << _END_COLOR << '\n';
}

void showCommands() {
    std::cout << " (1) initialize\n"
        << " (2) screen -s screen\n"
        << " (3) screen -s scheduler-test\n"
        << " (4) screen -s scheduler-stop\n"
        << " (5) screen -s report-util\n"
        << " (6) clear\n"
        << " (7) exit\n";
}
