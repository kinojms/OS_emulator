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
#include "MemoryManager.h"

class Functions {
private:
    std::vector<std::shared_ptr<Process>> allProcesses;
    std::shared_ptr<Scheduler> scheduler;
    std::shared_ptr<MemoryManager> memoryManager;
    std::thread schedulerThread;
    std::thread processGenThread; // Thread for random process generation
    std::atomic<bool> schedulerRunning = false;
    std::atomic<bool> processGenRunning = false; // Control flag for process generation
    std::atomic<bool> schedulerStopRequested = false; // New flag to request scheduler 

    // CPU tick counters for vmstat
    int idleCpuTicks = 0;
    int activeCpuTicks = 0;
    int totalCpuTicks = 0;

public:
    // Schedulers
    void runScheduler(int num_cpu, int quantum_Cycles, int min_ins, int max_ins,int batch_process_freq, float delay_Per_Exec, const std::string& schedulerType
	);
    // void schedulerTest(int num_cpu, const std::string& schedulerType, int quantum_Cycles, int min_ins, int max_ins, int batch_process_freq, float delay_Per_Exec);

    // Control
    void schedulerStop();
    void startProcessGenerator(int min_ins, int max_ins, int batch_process_freq = 1, int min_mem_per_proc = 0, int max_mem_per_proc = 0, int mem_per_frame = 0); // Start background process generator
    void stopProcessGenerator(); // Stop background process generator

    // Monitoring
    // void screen();
    void writeScreenReport(std::ostream& out); // Helper for screen/reportUtil
    void reportUtil();
    void processSMI(); // Summarized memory/process view
    void vmstat();     // Detailed memory/process/page view

    // Process management for screen -s and -r
    std::shared_ptr<Process> createProcess(const std::string& name, int min_ins, int max_ins, float delay_per_exec, int size);
    std::shared_ptr<Process> getProcessByName(const std::string& name);
    void switchScreen(const std::string& name);

    // Memory management
    void initializeMemoryManager(int maxOverallMem, int maxMemPerProc, int memPerFrame);
    void generateMemorySnapshot();
};

extern std::shared_ptr<Clock> globalClock;

#endif