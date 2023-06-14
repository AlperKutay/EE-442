# EE-449
# Project Name

*Homework 1 - Chemical Reaction Simulation Using Pthread Library

*Homework 2 - User Level Thread Scheduling

*Homework 3 - File System

## Table of Contents

- [Project Description](#project-description)
- [Compalition](#Compalition)

## Project Description
  Homework 1 - Chemical Reaction Simulation Using Pthread Library

  In the first part of this assignment, you are asked to simulate chemical reactions to form carbondioxide,
  nitrogen dioxide, sulfur dioxide and thorium dioxide. Due to synchronization problems it is hard to
  obtain these molecules properly. In order to obtain carbondioxide molecule, one C atom and two O
  atoms must get all together at the same time. Similarly, one N atom-two O atoms, one S atom-two O
  atoms, and one Th atom-two O atoms must get all together simultaneously to form nitrogen dioxide,
  sulfur dioxide and thorium dioxide molecules, respectively.
  We need to think atoms as threads. When we get all the required atoms at the same time, the related
  chemical reaction happens. You are expected to use only mutexes, locks and condition variables for
  synchronization. You should avoid starvation, busy waiting etc.
  In order to change the parameters of the program, command-line options should be used in this
  assignment.


  Homework 2 - User Level Thread Scheduling
  
  Threads can be separated into two kinds: kernel-level threads and user-level threads. Kernel-level threads are
  managed and scheduled by the kernel. User-level threads are needed to be managed by the programmer and
  they are seen as a single-threaded process from the kernel’s point of view. The user-level threads have some
  advantages and disadvantages over kernel-level threads.
  Advantages:
   User-level threads can be implemented on operating systems which do not support threads.
   Because there is no trapping in kernel, context switching is faster.
   Programmer has direct control over the scheduling policy.
  Disadvantages:
   Blocking system calls block the entire process.
   In the case of a page fault, the entire process is blocked, even if some threads might be runnable.
   Because kernel sees user-level threads as a single process, they cannot take advantage of multiple CPUs.
   Programmer has to make sure that threads give up the CPU voluntarily or has to implement a periodic
  interrupt which schedules the threads.
  For this homework, you will write a program which manages user-level threads and schedules them using a
  preemptive scheduler of your own. You will use <ucontext.h> to implement user-level threads.

  Homework 3 - File System
  
  In this homework, you are expected to implement a simple user mode file system (FS) that employs a
  file allocation table (FAT). The FS obtained could be used on a physical drive or on a disk image to be
  named as “disk”.
  The FS consist of three parts:
  1. File Allocation Table
  2. File List
  3. Disk Data
  Block size is 512 bytes, meaning that the files are written in 512 byte blocks. FS has no directories. 


## Compalition
  #Homework 1 - Chemical Reaction Simulation Using Pthread Library
  ```
  gcc -o main main.c -pthread -lm
  ```
  #Homework 2 - User Level Thread Scheduling
  ```
  gcc -o main main.c
  ```
  
  #Homework 3 - File System
  ```
  gcc -o main main.c 
  ```

