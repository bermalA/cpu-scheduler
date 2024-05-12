Process Scheduling with Memory Allocation Simulation in C

Author: Bermal ARATOĞLU

Project Overview:

In this project, a simulation of process scheduling with memory allocation in C is implemented. The program utilizes various data structures and algorithms to simulate the behavior of a CPU scheduler and memory manager.

Data Structures:

    Process: Simulates a CPU process with attributes such as arrival time, priority, memory requirements, etc. Each process is uniquely identified by its name attribute.
    MemoryBlock: Represents memory blocks in the computer's memory, facilitating memory allocation and deallocation. It stores the process name, start address, and size of each memory block.
    CpuQueue: Holds a queue of processes to be simulated, enabling the application of scheduling algorithms.

Scheduling Algorithms:

    First Come First Serve (FCFS): Processes are scheduled based on their arrival time. Processes with priority "0" are queued into a CpuQueue named CPU-1 queue.

    Shortest Job First (SJF): Processes are scheduled based on their burst times, minimizing waiting time. Processes with priority "1" are placed in a CpuQueue named CPU-2 queue_high.

    Round Robin (RR): A preemptive scheduling algorithm where each process is assigned a fixed time slice (quantum). Processes with priority values "2" and "3" are queued into CPU-2 queue_medium and CPU-2 queue_low, respectively, with different quantum values.

Memory Management:

    allocateMemory: Allocates memory for processes based on their priority and memory requirements. It searches for available memory blocks and selects the most suitable one. If a block has more memory than required, it splits the block to make excess memory available.
    deallocateMemory: Deallocates memory previously assigned to a process, allowing it to be reused. It resets the process name associated with the memory block and merges adjacent free memory blocks.

File I/O:

    read_process: Reads process information from an input file, initializes the processes as Process struct array, and returns the array.
    write_to_output: Writes necessary information to the output file using the fprintf function. Additionally, it prints debug information to the console.

Main Function:

    Reads and initializes processes using read_process and allocateMemory.
    Enqueues processes based on their priorities to different queues.
    Simulates CPU queues according to their burst times.
    Writes simulation results to the output file using write_to_output.
    Frees memory and closes the file before exiting the program.

Conclusion:

This project provides a comprehensive simulation of process scheduling with memory allocation in C, utilizing various data structures and algorithms. It demonstrates the implementation of fundamental scheduling algorithms and memory management techniques, contributing to a better understanding of operating system concepts.
