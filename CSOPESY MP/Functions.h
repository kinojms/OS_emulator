#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <string>
#include "CPUCore.h"
#include "Scheduler.h"
#include "Process.h"
#include "Clock.h"

class Functions {
private:
    std::vector<std::shared_ptr<Process>> allProcesses;
    std::shared_ptr<Scheduler> scheduler;
    std::thread schedulerThread;
    std::thread processGenThread; // Thread for random process generation
    std::atomic<bool> schedulerRunning = false;
    std::atomic<bool> processGenRunning = false; // Control flag for process generation
    std::atomic<bool> schedulerStopRequested = false; // New flag to request scheduler stop

    void writeScreenReport(std::ostream& out); // Helper for screen/reportUtil

public:
    // Schedulers
    void FCFS(int num_cpu, int quantum_Cycles, int min_ins, int max_ins, int batch_process_freq, float delay_Per_Exec);
    void RR(int num_cpu, int quantum_Cycles, int min_ins, int max_ins, int batch_process_freq, float delay_Per_Exec);
    void schedulerTest(int num_cpu, const std::string& schedulerType, int quantum_Cycles, int min_ins, int max_ins, int batch_process_freq, float delay_Per_Exec);

    // Control
    void schedulerStop();
    void startProcessGenerator(int min_ins, int max_ins, int batch_process_freq = 1); // Start background process generator
    void stopProcessGenerator(); // Stop background process generator

    // Monitoring
    void screen();
    void reportUtil();

    // Process management for screen -s and -r
    std::shared_ptr<Process> createProcess(const std::string& name, int min_ins, int max_ins, float delay_per_exec);
    std::shared_ptr<Process> getProcessByName(const std::string& name);
    void switchScreen(const std::string& name);
};

extern std::shared_ptr<Clock> globalClock;

#endif