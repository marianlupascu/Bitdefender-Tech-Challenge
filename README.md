
  
# Bitdefender Tech Challenge :rocket:
This repository contains some solutions and issues from Bitdefender TechChallenge 2018. This course took 14 weeks, and I will continue to present the details of each week.

## Course 1  :performing_arts:
Course 1 consisted of solving admission problems

## Course 2 :necktie:
[Course 2](https://github.com/marianlupascu/Bitdefender-Tech-Challenge/tree/master/Curs%202) consists of __working with files__. Some subtitles are: _NTFS Filesystem_, _MFT FILE_, _Alternate Data Streams_ and the presentation of several functions and structures for manipulating files:
- NTFS Structures
- CreateFile
- ReadFile
- WriteFile
- CloseHandle
- FindFirstStreamW
- SetFilePointer
- GetFileAttributes
- FindFirstFile
- FindNextFile
- MoveFile
- GetFileSizeEx
- GetFileTime
- SetFileTime

## Course 3 :telescope:
[Course 3](https://github.com/marianlupascu/Bitdefender-Tech-Challenge/tree/master/Curs%203) consists of __Threads and Synchronization__. Some subtitles are: _Threads Scheduling/Dispatching_, _Context Switching_, _Thread Scheduling Strategy_, _Mutex_, _Setthreadpriority_ and the presentation of several functions and structures for manipulating threads:
- CreateThread
- CreateMutex
- CreateEvent
- **CriticalSection**

> And the subject statement is:
> 
### Simulation of an intersection. :car:

Write a program that generates N (say 200) threads, each thread having a randomly chosen direction from one of the values:
0 - NS
1 - SN
2 - EW
3 - WE

On each NS or EW direction there is a simulator simulated by an event.
There must still be a master thread that changes the traffic lights, giving every 90 seconds the road of a traffic light and blocking the other with a period of 3 seconds in which both are red to release the intersection.
Every thread arriving at the intersection checks if the cars are in front of him in the same direction and calculates the time needed to pass as the number of cars in front of him + 3 seconds from the moment when the traffic light is green, if not green can pass barely after the cars in front of him passed + another 3 seconds.

## Course 4 and 5 :straight_ruler:
[Course 4](https://github.com/marianlupascu/Bitdefender-Tech-Challenge/tree/master/Curs%204) and [Course 5 ](https://github.com/marianlupascu/Bitdefender-Tech-Challenge/tree/master/Curs%205) is all about threads, which brings in addition:
- Find out the number of processors (GetSystemInfo)
- **Semaphores** (CreateSemaphore, WaitForSingleObject, WaitForMultipleObjects, RelaseSemaphore)
- SignalObjectAndWait
- **ConditionVariable** (SleepConditionVariableCS, WakeConditionVariable)
- **Barrier** (InitializeSynchronizationBarrier, EnterSynchronizationBarrier, DeleteSynchronizationBarrier)
- **DLL**

## Course 6 and 7 :pencil2:
[Courses 6 and 7](https://github.com/marianlupascu/Bitdefender-Tech-Challenge/tree/master/Curs%206-7/Pipeline) are about **processes**, **communication between processes**, synchronizations, **environment variables, named pipes and shared memory**.

## Course 8 and 9 :black_nib:
[Courses 8 and 9](https://github.com/marianlupascu/Bitdefender-Tech-Challenge/tree/master/Curs%208-9) are about **Windows services** and **networking**.

## Course 10 and 11 :battery:
[Courses 10 and 11](https://github.com/marianlupascu/Bitdefender-Tech-Challenge/tree/master/Curs%2010-11) is all about **Advanced I/O**. ome subtitles are: _Overlapped Structure_, _I/O completion ports_, _Intercepting exceptions_, _HARDLINKS_, _WINHTTP, _Prefetch files_ and _The NTFS log_.

## Course 12 and 13 :mag:
[Course 12](https://github.com/marianlupascu/Bitdefender-Tech-Challenge/tree/master/Curs%2012/Curs12-Tools) and [Course 13](https://github.com/marianlupascu/Bitdefender-Tech-Challenge/tree/master/Curs%2013) is all about **debugging** and contains informations about:
- Deadlocks
- Memories crash
- LeakMemory
- DLLs
- Registers
- various debugging tools on Windows such as:
	- ProcessExplorer
	- ProcessMonitor
	- Autoruns
	- Wireshark
	- ProcDump
	- and more from [Windows Sysinternals Suite](https://docs.microsoft.com/en-us/sysinternals/downloads/sysinternals-suite).

## Course 14 :pizza:
During the 14th we held the final test. :books: :computer:
