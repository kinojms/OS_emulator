#pragma once
#include <string>
#include <memory>
#include "Functions.h"
#include "Display.h"

class CommandManager {
public:
    CommandManager(std::shared_ptr<Functions> functions, std::shared_ptr<Display> display);
    void execute(const std::string& input);
    void initializeFromConfig(const std::string& configFile);
    bool isInitialized() const;
    void setInitialized(bool value);
private:
    std::shared_ptr<Functions> fun;
    std::shared_ptr<Display> dp;
    bool initialized = false;
    // Config values
    int num_cpu = 0;
    std::string scheduler;
    int quantum_Cycles = 0;
    int batch_Process_Freq = 0;
    int min_ins = 0;
    int max_ins = 0;
    float delay_Per_Exec = 0.0f;
    int max_overall_mem = 0;
    int mem_per_frame = 0;
    int mem_per_proc = 0;
    void handleScreenCommand(const std::string& flag, const std::string& command);
    void handleSchedulerCommand(const std::string& token);
    void handleReportUtil();
    void handleProcessSmi(const std::string& processName);
    void handleVmstat();
    void handleUnknownCommand();
};
