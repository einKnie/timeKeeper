# timeKeeper

~~_This is a work in progress. The codebase as well as this README will be cleaned up once I'm done._~~

Version 1.0

## Functionality
**[LINUX ONLY]**  

Keep track of how much time you spend on a number of tasks.  
The program keeps track of the cumulative time any given task was active. Thus, task switches are easily possible. Only one task can be active at a time.  
The cumulative task times are saved automatically upon exit.


### Usage

| cmd|action |
|----|-------|
|-t \<no\>   | specify a task (numbers range from 1 to 5 \*)  
|-n \<str\>  | set a name for the specified task  \*\*
|-s          | start the specified task
|-e          | stop all tasks
|-v          | show a notification with information on tasks
|-x          | write current task data to file \*\*\*
|-q          | quit the daemon process (also writes data to file)
|-h          | print help

\* number can be increased in _timeKeeper.h_  
\*\* if a task without a set name is started, the user is automatically queried for a name.  
\*\*\* data is stored in $HOME/.timeKeeper.dat

### Examples

    timeKeeper -t1 -n "meetings" # set name of task1 to 'meetings'
    timeKeeper -t1 -s            # start task1
    ...
    timeKeeper -t4 -s            # stop running task and start task3
    ...
    timeKeeper -x                # write data of all tasks to file


#### Example logfile

    ...
    2020-09-27 18:44:44
    Task 1 00:23:41 "meetings"
    Task 2 00:15:17 "JIRA-3460"
    Task 3 00:00:00 ""
    Task 4 00:04:26 "jenkins pipeline"
    Task 5 00:00:00 ""
    ------------





### Implementation specifics

The first running timeKeeper process will daemonize itself and keep running in the background. Any new timeKeeper process will check if the daemon is running and simply send the command via IPC to the daemon process.  
Stopping the daemon process will also halt any time keeping. But current data is written to file upon exit, so nothing will be lost.  
Log output from the daemon can be found in _/tmp/timeKeeper.log_.

### TODO

This is a list of features that will be implemented in the future:  
*  more than one action possible per call  
*  possibility to resume last active task after record keeping has been disabled with _-e_

### Example Configuration and Usage

 
Configure keyboard commands to call the program with specific arguments, e.g.:  

| key     | cmd              |
|---------|------------------|
| mod+F1  | timeKeeper -t1 -s|
| mod+F2  | timeKeeper -t2 -s|
| ...     | ...              |
| mod+^   | timeKeeper -v    |
| mod+l   | timeKeeper -e; lockscreen \*|
| mod+esc | timeKeeper -q    |

\* useful to stop time tracking whenever the screen is locked
