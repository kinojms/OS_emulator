# Code Explanation
To make it simple, here's an analogy:
- Process = Jobs
- CPUCores = Workers
- Scheduler = Manager
- Functions = Supervisor

## Process.cpp (Job)
- is a "job" with a unique id (`pid`) and a list of instructions (100 print statements)
- when a "worker" (CPU Core) gets a "job" (Process), it runs the `execute()` method, which writes messages to a `.txt` file and marks itself as finished when done.

## CPUCore.cpp (Worker)
- each CPU Core is a "worker" with a unique id
- when the "manager" (Scheduler) assigns a job (Process), the worker (CPU Core) starts a new thread to run the Process' `execute()` method. (`void Process:execute()`)

## Scheduler.cpp (Manager)
- this keeps a queue of jobs (Processes) and a list of workers (CPU cores)
- it loops to check for any free workers (CPU cores)
- if a worker (CPU Core) is free and there's a job (Process) waiting to be done, it gives the oldest job (the first one in the queue) to that worker
- this reflects the FCFS—processes are executed based on their arrival time
- the manager keeps checking and assigning until all jobs (Process) are done and workers (CPU Core) are idle 

## Functions.cpp (Supervisor)
- sets up/initializes the manager and workers, creates the jobs, and starts the scheduling process using `screen -s scheduler-start`
- lets you check the current status of all jobs (Processes) using the command `screen -ls` 

### Note: 
The `.txt` files are stored in the solution's folder.

If you cloned my branch, you navigate through it like this: `OS_emulator/CSOPESY MP/<processes are saved here>`

I'll try and make a folder for this tomorrow and save the text files there instead.

### Working Commands:
```
initialize
screen -s scheduler-start // starts the scheduler
screen -s scheduler-stop // stops the scheduler
screen -ls // views the queue (current and finished processes)
```

### Big Issue: 
1. Lost `screen -r` functionality.  
	- I plan on making multiple Console classes for this (Main Console and Process Console) also use a ConsoleManager to switch between these two screens
2. Lost `screen -s <process name>` functionality.
	- will reimplement this after the deadline lmao
