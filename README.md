## Synchronization

Project part of Principles of Operating Systems (IC6600) course. That has the aim to learn about synchronization, semaphores, paging and segmentation of memory.

## Developers

Adrián Álvarez Calderón [@adalvarez](https://github.com/adalvarez)

Michael Choque Núñez [@Feymus](https://github.com/Feymus)

---

## What is it?

Its about a project of the course Principles of Operating Systems (IC6600) in Technological Institute of Costa Rica that wants to develop a simulation of memory managment by a Operative System.

#### Objectives of the project

* Implement a synchronization problem.
* Deepen the knowledge of Semaphores.
* Know some Linux functions for synchronization.
* Review the theoretical knowledge of Pagination / Segmentation.

The purpose of this project is to implement a problem of synchronization of processes in a simulation of allocation of memory to processes through the paging and segmentation schemes. This problem includes a shared file among all the sub-programs (later mentioned) to keep an event log. Shared memory is used to simulate the memory to be assigned to processes.

#### Sub-programs

* **Initializer**: This program is responsible for creating the environment. Ask for the resources and initialize them, so the other sub-programs find everything ready when they start to work. This program asks for the number of pages or memory spaces that there will be and requests the memory shared to the operating system.
* **Producer**: This program is responsible for producing the process simulation, depending on the paging or segmentation scheme. These are the default values for the simulation. The distribution with which the processes are generated is determined by a random time (30s-60s). Each process must search within the memory the pages or space for its segments. If there is not enough space the process dies. Only one process at a time can be running the space search algorithm, so that there are no collisions that 2 processes select the same space / page. Once it has found a place, a sleep with the amount of time defined for it and then exclusively (Critical Region) returns the memory spaces assigned to it.

| Schema | Values | 
| ------ | ------ |
| Paging | Amount of pages (1 to 10) <br> Time for each process (20s to 60s) |
| Segmentación | Amount of segments (1 to 5) <br> Amount of memory spaces for each segment (1 to 3) <br> Time for each process (20s to 60s) |

* **Spy**: It shows the state of the memory at a certain moment and shows how the processes are accommodated in the memory. And show the following information:
    * The PID of the processes that are in memory at that moment (time-sleep)
    * The PID of the only process that is looking for space in memory
    * The PID of the processes that are blocked (waiting for the critical region)
    * The PID of the processes that have died because there is not enough space.
    * The PID of the processes that have already finished their execution
* **Finisher**: It is responsible for killing all the processes that are on the scene. Return the resources it had requested.

All sub-programs must register their actions. This includes saying for each PID, the action, what type it is (assignment, de-assignment), time and space allocated for each of its pages or segments. In the event that a process has not entered the memory must register this fact in the log.

#### Requirements

* [Make](https://www.gnu.org/software/make/)
* [GCC](https://gcc.gnu.org/)

---

## Usage

In case that you do not have the requirements, you can run the install file. (**Debian/Ubuntu**)
`$ chmod +x install.sh && ./install.sh`

Make the binary files by each folder (initializer, producer, spy and finisher).
`$ make`

#### Initializer

Run

`$ ./initializer`

It ask you for the number of memory spaces to be created.

#### Producer 

Run

`$ ./producer [options]`

Options:
* -s or -S to Segmentation
* -p or -P to Paging

#### Spy 

Run

`$ ./spy`

It will show something like this:

```Main PID: 1317
[Memory status]:
        idLine:0,       owner:0,        nSegment:0,     nPag_pSeg:0
        idLine:1,       owner:0,        nSegment:0,     nPag_pSeg:0
        idLine:2,       owner:0,        nSegment:0,     nPag_pSeg:0
        idLine:3,       owner:0,        nSegment:0,     nPag_pSeg:0
        (...)
[Process status]:
  [SLEEP]: IDs
  [SEARCH]: IDs
  [BLOCK]: IDs
  [DEAD]: IDs
  [END]: IDs
```

#### Finisher

Run

`$ ./finisher`

