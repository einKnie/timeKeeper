# timeKeeper

_This is a work in progress. The codebase as well as this README will be cleaned up once I'm done._

## Functionality

Keep track of how much time you spend on a number of tasks. 


### Usage

| cmd|action |
|----|-------|
|-t \<no\>   | specify a task (numbers range from 1 to 5 \*)  
|-n \<str\>  | set a name for the specified task  
|-s          | start the specified task
|-v          | show a notification with information on tasks (wip)
|-x          | write current task data to file \*\*
|-h          | print help

\* number can be increased in _timeKeeper.h_  
\*\* data is stored in $HOME/.timeKeeper.dat

### Examples

    timeKeeper -t1 -n "meetings" # set name of task1 to 'meetings'
    timeKeeper -t1 -s            # start task1
    ...
    timeKeeper -t3 -s            # stop running task and start task3
    ...
    timeKeeper -x                # write data of all tasks to file
    
   
#### Example logfile

    ...
    2020-09-27 17:00:07
    Task 1;"meetings";00:22:15 
    Task 2;"";00:00:00 
    Task 3;"";00:05:18 [x]   <-- x denotes the currently active task
    Task 4;"";00:00:00 
    Task 5;"";00:00:00 
    ------------


The program keeps track of the cumulative time any given task was active. Thus, task switches are easily possible. Only one task can be active at a time. 

### Implementation specifics

The first running timeKeeper process will daemonize itself (wip) and keep running in the background. Any new timeKeeper process will check if the daemon is running and simply send the command via IPC to the daemon process.  
Stopping the daemon process will also halt any time keeping. But current data is written to file upon exit, so nothing will be lost.

### TODO

This is a list of features that will be implemented in the future:  
*  actual daemonisation
*  halt all tasks (e.g. when on a break)
*  tbd

