#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 5
#define QUANTUM 2
#define MAX_QUEUE 1000

typedef struct {
    int pid;
    int at;
    int bt;
    int remaining;
    int ct;
    int tat;
    int wt;
} Process;

int main(int argc, char *argv[]) {
    unsigned int seed;

    if (argc > 1)
        seed = (unsigned int)strtoul(argv[1], NULL, 10);
    else
        seed = (unsigned int)time(NULL);

    srand(seed);

    Process p[N];
    int order[N];

    printf("Random Seed: %u\n", seed);
    printf("Time Quantum: %d\n\n", QUANTUM);
    printf("Process\tAT\tBT\n");

    // Generate random process data.
    for (int i = 0; i < N; i++) {
        p[i].pid = i + 1;
        p[i].at = rand() % 21;
        p[i].bt = 2 + rand() % 11;
        p[i].remaining = p[i].bt;
        p[i].ct = 0;
        p[i].tat = 0;
        p[i].wt = 0;
        order[i] = i;

        printf("P%d\t%d\t%d\n", p[i].pid, p[i].at, p[i].bt);
    }

    // Sort processes by arrival time, then by PID.
    for (int i = 0; i < N - 1; i++) {
        for (int j = i + 1; j < N; j++) {
            if (p[order[j]].at < p[order[i]].at ||
                (p[order[j]].at == p[order[i]].at &&
                 p[order[j]].pid < p[order[i]].pid)) {
                int temp = order[i];
                order[i] = order[j];
                order[j] = temp;
            }
        }
    }

    int queue[MAX_QUEUE];
    int front = 0, rear = 0;
    int next = 0;
    int completed = 0;
    int current_time = 0;

    int gantt_pid[MAX_QUEUE];
    int gantt_start[MAX_QUEUE];
    int gantt_end[MAX_QUEUE];
    int gantt_count = 0;

    while (completed < N) {

        // Add processes that have arrived.
        while (next < N && p[order[next]].at <= current_time) {
            queue[rear++] = order[next++];
        }

        // If no process is ready, CPU remains idle.
        if (front == rear) {
            if (next < N) {
                gantt_pid[gantt_count] = -1;
                gantt_start[gantt_count] = current_time;
                gantt_end[gantt_count] = p[order[next]].at;
                gantt_count++;

                current_time = p[order[next]].at;
                continue;
            }
        }

        // Select the first process in the ready queue.
        int current = queue[front++];

        int run_time = p[current].remaining < QUANTUM
                     ? p[current].remaining : QUANTUM;

        gantt_pid[gantt_count] = p[current].pid;
        gantt_start[gantt_count] = current_time;

        // Execute the process.
        current_time += run_time;
        p[current].remaining -= run_time;

        gantt_end[gantt_count] = current_time;
        gantt_count++;

        // Add newly arrived processes before re-queuing
        // the process that just used the CPU.
        while (next < N && p[order[next]].at <= current_time) {
            queue[rear++] = order[next++];
        }

        if (p[current].remaining > 0) {
            queue[rear++] = current;
        } else {
            p[current].ct = current_time;
            completed++;
        }
    }

    // Calculate CT, TAT, and WT.
    float total_tat = 0;
    float total_wt = 0;

    for (int i = 0; i < N; i++) {
        p[i].tat = p[i].ct - p[i].at;
        p[i].wt = p[i].tat - p[i].bt;

        total_tat += p[i].tat;
        total_wt += p[i].wt;
    }

    // Print Gantt chart.
    printf("\nGantt Chart:\n");

    for (int i = 0; i < gantt_count; i++) {
        if (gantt_pid[i] == -1)
            printf("| IDLE ");
        else
            printf("| P%d ", gantt_pid[i]);
    }
    printf("|\n");

    if (gantt_count > 0) {
        printf("%d", gantt_start[0]);

        for (int i = 0; i < gantt_count; i++) {
            printf("\t%d", gantt_end[i]);
        }
        printf("\n");
    }

    // Print scheduling results.
    printf("\nProcess\tAT\tBT\tCT\tTAT\tWT\n");

    for (int i = 0; i < N; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",
               p[i].pid, p[i].at, p[i].bt,
               p[i].ct, p[i].tat, p[i].wt);
    }

    printf("\nAverage Turnaround Time: %.2f\n", total_tat / N);
    printf("Average Waiting Time: %.2f\n", total_wt / N);

    return 0;
}