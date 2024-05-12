#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PROCESS 100
#define RAM_SIZE 2048

//// *******DEFINITIONS******** ////

typedef struct
{
    char name[10];
    int arrival_time;
    int priority;
    int burst_time;
    int memory;
    int cpu_percentage;
} Process;

typedef struct
{
    Process queue[MAX_PROCESS];
    int front;
    int back;
} CpuQueue;

typedef struct
{
    char process_name[10];
    int start_address;
    int size;
} MemoryBlock;

//// *******GLOBAL VARIABLES******** ////

MemoryBlock memory[RAM_SIZE];
int cpu2_high_priority_quantum = 8;
int cpu2_medium_priority_quantum = 16;

//// *******FUNCTION PROTOTYPES******** ////

int read_process(const char filename[], Process *processes);
void init_CPUQueue(CpuQueue *queue);
void enqueueProcess(Process *process, CpuQueue *cpu1_queue, CpuQueue *cpu2_queue_high, CpuQueue *cpu2_queue_medium, CpuQueue *cpu2_queue_low);
void first_come_first_serve(Process *process, CpuQueue *cpu1_queue);
void shortest_job_first(Process *process, CpuQueue *cpu2_queue_high);
void round_robin(Process *process, CpuQueue *queue, int quantum_time);
void simulateExecution(CpuQueue *cpu1_queue, CpuQueue *cpu2_queue_high, CpuQueue *cpu2_queue_medium, CpuQueue *cpu2_queue_low);
int allocateMemory(char process_name[], int required_memory, int priority);
void deallocateMemory(char process_name[]);

//// *******READ FILE******** ////

int read_process(const char filename[], Process *processes)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        printf("Error opening file");
        return 0;
    }

    int process_count = 0;
    char line[256];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        Process *process = &processes[process_count];

        sscanf(line, "%9[^,],%d,%d,%d,%d,%d", process->name, &process->arrival_time,
               &process->priority, &process->burst_time, &process->memory, &process->cpu_percentage);

        process_count++;
    }

    fclose(file);

    return process_count;
}

//// *******START QUEUE******** ////

void init_CPUQueue(CpuQueue *queue)
{
    queue->front = -1;
    queue->back = -1;
}

//// *******PROCESS ALGORITHMS******** ////

void first_come_first_serve(Process *process, CpuQueue *cpu1_queue)
{
    if (cpu1_queue->back == MAX_PROCESS - 1)
    {
        printf("Queue is full \n");
        return;
    }

    if (cpu1_queue->front == -1)
    {
        cpu1_queue->front = 0;
    }

    cpu1_queue->back++;
    cpu1_queue->queue[cpu1_queue->back] = *process;
}

void shortest_job_first(Process *process, CpuQueue *cpu2_queue_high)
{
    int i;

    if (cpu2_queue_high->front == -1)
    {
        cpu2_queue_high->front = 0;
        cpu2_queue_high->back = 0;
        cpu2_queue_high->queue[cpu2_queue_high->back] = *process;
    }
    else
    {
        for (i = cpu2_queue_high->back; i >= cpu2_queue_high->front; i--)
        {
            if (process->burst_time < cpu2_queue_high->queue[i].burst_time)
            {
                cpu2_queue_high->queue[i + 1] = cpu2_queue_high->queue[i];
            }
            else
            {
                break;
            }
        }

        cpu2_queue_high->queue[i + 1] = *process;
    }

    cpu2_queue_high->back++;
}

void round_robin(Process *process, CpuQueue *queue, int quantum_time)
{
    if (queue->back == MAX_PROCESS - 1)
    {
        printf("Queue is full");
        return;
    }

    if (queue->front == -1)
    {
        queue->front = 0;
    }

    queue->back++;
    queue->queue[queue->back] = *process;
}

//// *******PROCESS ENQUEUE******** ////

void enqueueProcess(Process *process, CpuQueue *cpu1_queue, CpuQueue *cpu2_queue_high, CpuQueue *cpu2_queue_medium, CpuQueue *cpu2_queue_low)
{
    if (process->priority == 0)
    {
        first_come_first_serve(process, cpu1_queue);
    }
    else if (process->priority == 1)
    {
        shortest_job_first(process, cpu2_queue_high);
    }
    else if (process->priority == 2)
    {
        round_robin(process, cpu2_queue_medium, cpu2_medium_priority_quantum);
    }
    else if (process->priority == 3)
    {
        round_robin(process, cpu2_queue_low, cpu2_high_priority_quantum);
    }
    else
    {
        printf("Error enqueueing process");
    }
}

//// *******PROCESS EXECUTION******** ////
void simulateExecution(CpuQueue *cpu1_queue, CpuQueue *cpu2_queue_high, CpuQueue *cpu2_queue_medium, CpuQueue *cpu2_queue_low)
{
    Process current_process;

    printf("CPU-1 queue(priority-0) (FCFS) → ");
    while (cpu1_queue->front <= cpu1_queue->back)
    {
        current_process = cpu1_queue->queue[cpu1_queue->front];
        cpu1_queue->front++;

        printf("%s ", current_process.name);
    }
    printf("\n");

    printf("CPU-2 queue_high(priority-1) (SJF) → ");
    while (cpu2_queue_high->front <= cpu2_queue_high->back)
    {
        current_process = cpu2_queue_high->queue[cpu2_queue_high->front];
        cpu2_queue_high->front++;

        printf("%s ", current_process.name);
    }
    printf("\n");

    printf("CPU-2 queue_medium(priority-2) (RR-q8) → ");
    while (cpu2_queue_medium->front <= cpu2_queue_medium->back)
    {
        current_process = cpu2_queue_medium->queue[cpu2_queue_medium->front];
        cpu2_queue_medium->front++;

        printf("%s ", current_process.name);
    }
    printf("\n");

    printf("CPU-2 queue_low(priority-3) (RR-q16) → ");
    while (cpu2_queue_low->front <= cpu2_queue_low->back)
    {
        current_process = cpu2_queue_low->queue[cpu2_queue_low->front];
        cpu2_queue_low->front++;

        printf("%s ", current_process.name);
    }
    printf("\n");
}

//// *******MEMORY ALLOCATE & DEALLOCATE******** ////
int allocateMemory(char process_name[], int required_memory, int priority)
{
    int highest_priority = -1;
    int available_block_index = -1;

    for (int i = 0; i < RAM_SIZE; i++)
    {
        if (memory[i].process_name[0] == '\0' && memory[i].size >= required_memory)
        {
            if (priority > highest_priority)
            {
                highest_priority = priority;
                available_block_index = i;
            }
            else if (priority == highest_priority && memory[i].size > memory[available_block_index].size)
            {
                available_block_index = i;
            }
        }
    }

    if (available_block_index != -1)
    {
        strcpy(memory[available_block_index].process_name, process_name);
        if (memory[available_block_index].size > required_memory)
        {
            memory[available_block_index + 1].start_address = memory[available_block_index].start_address + required_memory;
            memory[available_block_index + 1].size = memory[available_block_index].size - required_memory;
            memory[available_block_index + 1].process_name[0] = '\0';
        }
        memory[available_block_index].size = required_memory;
        return memory[available_block_index].start_address;
    }

    return -1;
}

void deallocateMemory(char process_name[])
{
    for (int i = 0; i < RAM_SIZE; i++)
    {
        if (strcmp(memory[i].process_name, process_name) == 0)
        {
            memory[i].process_name[0] = '\0';
            if (i > 0 && memory[i - 1].process_name[0] == '\0')
            {
                memory[i - 1].size += memory[i].size;
            }
            if (i < RAM_SIZE - 1 && memory[i + 1].process_name[0] == '\0')
            {
                memory[i].size += memory[i + 1].size;
            }
        }
    }
}

void write_to_output(FILE *output_file, const char *message)
{
    fprintf(output_file, "%s\n", message);
    printf("%s\n", message);
}

//// *******MAIN METHOD******** ////

int main()
{
    Process processes[MAX_PROCESS];
    int process_count = read_process("input.txt", processes);

    if (process_count == 0)
    {
        printf("Error reading processes from file\n");
        return 1;
    }

    FILE *output_file = fopen("output.txt", "w");
    if (output_file == NULL)
    {
        printf("Error opening output file\n");
        return 1;
    }

    CpuQueue cpu1_queue, cpu2_queue_high, cpu2_queue_medium, cpu2_queue_low;
    init_CPUQueue(&cpu1_queue);
    init_CPUQueue(&cpu2_queue_high);
    init_CPUQueue(&cpu2_queue_medium);
    init_CPUQueue(&cpu2_queue_low);

    for (int i = 0; i < process_count; i++)
    {
        Process *process = &processes[i];
        enqueueProcess(process, &cpu1_queue, &cpu2_queue_high, &cpu2_queue_medium, &cpu2_queue_low);
    }

    for (int i = 0; i < process_count; i++)
    {
        Process *process = &processes[i];
        fprintf(output_file, "Process %s is queued to be assigned to CPU-1.\n", process->name);
    }

    fprintf(output_file, "Simulation of process execution begins.\n");

    fprintf(output_file, "All processes completed and terminated.\n");

    simulateExecution(&cpu1_queue, &cpu2_queue_high, &cpu2_queue_medium, &cpu2_queue_low);

    fclose(output_file);

    return 0;
}