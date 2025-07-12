# First-Fit Memory Allocator Implementation Summary

## Overview
This document summarizes the implementation of a first-fit, flat memory allocator for the CSOPESY MP project.

## Key Components Implemented

### 1. MemoryManager Class (`MemoryManager.h` and `MemoryManager.cpp`)
- **First-Fit Algorithm**: Allocates memory by finding the first available block that can fit the requested size
- **Memory Block Structure**: Tracks start address, size, process name, and allocation status
- **Memory Deallocation**: Automatically merges adjacent free blocks when deallocating
- **Snapshot Generation**: Creates memory layout files every quantum cycle

### 2. Configuration Updates (`config.txt`)
Updated with the required parameters:
- `num-cpu = 2`
- `scheduler = rr`
- `quantum-cycles = 4`
- `batch-process-freq = 1`
- `min-ins = 100`
- `max-ins = 100`
- `max-overall-mem = 16384` (16KB total memory)
- `mem-per-frame = 16`
- `mem-per-proc = 4096` (4KB per process)

### 3. Integration Points

#### Scheduler Integration (`Scheduler.cpp`)
- Modified `addProcess()` to attempt memory allocation before adding to queues
- If memory allocation fails, process is added to the tail of the ready queue (no backing store)
- Tracks memory allocation status in Process objects

#### Process Management (`Process.cpp`)
- Added `memoryAllocated` flag to track memory allocation status
- Added methods to set and check memory allocation status

#### CPU Core Integration (`CPUCore.cpp`)
- Automatically deallocates memory when processes finish execution
- Calls memory manager's deallocation method

#### Main Console (`MainConsole.cpp`)
- Reads memory parameters from config.txt
- Initializes memory manager during system initialization

## Memory Allocation Algorithm

### First-Fit Implementation
1. **Search**: Iterate through memory blocks from lowest to highest address
2. **Fit Check**: Find first block that is free and large enough for the process
3. **Allocation**: 
   - If perfect fit: allocate entire block
   - If larger: split block, allocate required portion, create new free block
4. **Update**: Mark block as allocated and assign process name

### Memory Deallocation
1. **Find Block**: Locate block allocated to the process
2. **Mark Free**: Set allocation status to false
3. **Merge**: Attempt to merge with adjacent free blocks to reduce fragmentation

## Memory Snapshot Format

Generated files: `memory_stamp_<quantum_cycle>.txt`

Format:
```
Timestamp: [current timestamp]
Number of processes in memory: [count]
Total external fragmentation in KB: [fragmentation]
----end---- = 16384 // This is the max
[highest_address]
[process_name_if_allocated]
[lowest_address]

[middle_addresses_and_processes]

----start---- = 0
```

## Key Features

### 1. Thread Safety
- All memory operations are protected by mutex locks
- Safe concurrent access from multiple CPU cores

### 2. Automatic Memory Management
- Memory is allocated when processes are added to scheduler
- Memory is automatically deallocated when processes finish
- No manual memory management required

### 3. Fragmentation Handling
- Tracks external fragmentation in KB
- Automatically merges adjacent free blocks
- Reports fragmentation in memory snapshots

### 4. Process Queue Management
- Processes without memory are still added to ready queue
- They will be retried for memory allocation when they reach the front
- No backing store implementation (as per requirements)

## Expected Behavior

With the configured parameters:
- **Total Memory**: 16,384 bytes (16KB)
- **Per Process**: 4,096 bytes (4KB)
- **Maximum Processes**: 4 processes can fit in memory simultaneously
- **External Fragmentation**: Will occur as processes finish and new ones start

## Testing Instructions

1. Run the application
2. Type `initialize` to load configuration
3. Type `scheduler-start` to begin simulation
4. Wait 5 seconds
5. Type `scheduler-stop` to stop scheduler
6. Use `screen -ls` every 2 seconds to monitor processes
7. Check for generated `memory_stamp_*.txt` files

## Files Modified/Created

### New Files:
- `MemoryManager.h` - Memory manager header
- `MemoryManager.cpp` - Memory manager implementation
- `MEMORY_IMPLEMENTATION_SUMMARY.md` - This summary

### Modified Files:
- `config.txt` - Added memory parameters
- `Functions.h/cpp` - Added memory manager integration
- `Scheduler.h/cpp` - Added memory-aware scheduling
- `Process.h/cpp` - Added memory allocation tracking
- `CPUCore.h/cpp` - Added memory deallocation
- `MainConsole.cpp` - Added memory parameter reading
- `CSOPESY MP.vcxproj` - Updated project files
- `CSOPESY MP.vcxproj.filters` - Updated filters

## Compliance with Requirements

✅ **First-fit allocation algorithm** - Implemented in `MemoryManager::allocateMemory()`
✅ **16,384 bytes total memory** - Configured in `config.txt`
✅ **4,096 bytes per process** - Configured in `config.txt`
✅ **Processes stay in memory during execution** - Implemented in scheduler
✅ **Memory released when process finishes** - Implemented in `CPUCore::assignProcess()`
✅ **Processes moved to tail of ready queue when memory full** - Implemented in `Scheduler::addProcess()`
✅ **Memory snapshots every quantum cycle** - Implemented in `Functions::RR()`
✅ **Correct snapshot format** - Implemented in `MemoryManager::generateSnapshotFile()`
✅ **Round-robin scheduler with memory allocation** - Integrated throughout the system 