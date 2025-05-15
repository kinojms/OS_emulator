#include <iostream>
#include <cstdio>
#include <cctype>
#include <cstring>
#include "Display/Display.h"
#include "Functions/Functions.h"
using namespace std;

int main() {
    char line[200];
    bool isInitialized = false;

    intro();

    while (true) {
        printf("\nEnter a command \n");
        fgets(line, sizeof(line), stdin);

        // Remove newline character from fgets
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // Convert entire input to lowercase
        for (int i = 0; line[i] != '\0'; ++i) {
            line[i] = tolower(line[i]);
        }

        // Check for "exit"
        if (strcmp(line, "exit") == 0) {
            break;
        }

        // Check for "clear" (always available)
        if (strcmp(line, "clear") == 0) {
            system("cls");
            intro();
            continue;
        }

        // Tokenize input
        char* token = strtok(line, " ");
        if (token == nullptr) continue;

        // Initialize-only command
        if (strcmp(token, "initialize") == 0) {
            isInitialized = true;
            continue;
        }

        // After initialize, require screen -s before any command
        if (strcmp(token, "screen") == 0) {
            char* next = strtok(nullptr, " ");
            if (next != nullptr && strcmp(next, "-s") == 0) {
                // Now fetch the actual command
                char* nextCmd = strtok(nullptr, " ");
                if (nextCmd == nullptr) {
                    printf("You must enter a command after 'screen -s'.\n");
                    continue;
                }

                if (strcmp(nextCmd, "screen") == 0) {
                    printf("Screen command recognized. Doing something.\n");
                } else if (strcmp(nextCmd, "scheduler-test") == 0) {
                    printf("scheduler-test command recognized. Doing something.\n");
                } else if (strcmp(nextCmd, "scheduler-stop") == 0) {
                    printf("scheduler-stop command recognized. Doing something.\n");
                } else if (strcmp(nextCmd, "report-util") == 0) {
                    printf("report-util command recognized. Doing something.\n");
                } else if (strcmp(nextCmd, "command") == 0) {
                    printf(" (1)initialize \n (2)screen -s screen \n (3)screen -s scheduler-test \n (4)screen -s scheduler-stop \n (5)screen -s report-util \n (6)clear \n (7)exit\n");
                } else {
                    printf("Unknown command after 'screen -s': %s\n", nextCmd);
                }
            } else {
                printf("You must start with 'screen -s' to run a command.\n");
            }
        }
    }

    printf("Code End\n");
    return 0;
}