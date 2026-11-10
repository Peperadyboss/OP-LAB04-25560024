#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 5

typedef struct {
    int id;
    int at;
    int bt;
    int ct;
    int tat;
    int wt;
    int done;
} Process;

int main(int argc, char *argv[]) {
    Process p[N];
    unsigned int seed = (argc > 1)
        ? (unsigned int)strtoul(argv[1], NULL, 10)
        : (unsigned int)time(NULL);

    srand(seed);

    printf("===== SJF Scheduling =====\n");
    printf("Random seed: %u\n\n", seed);

    // Generate random Arrival Time and Burst Time
    printf("Generated Process Data:\n");
    printf("Process\tAT\tBT\n");

    for (int i = 0; i < N; i++) {
        p[i].id = i + 1;
        p[i].at = rand() % 21;       // 0 to 20
        p[i].bt = 2 + rand() % 11;   // 2 to 12
        p[i].done = 0;

        printf("P%d\t%d\t%d\n",
               p[i].id, p[i].at, p[i].bt);
    }

    int time = 0;
    int completed = 0;
    float totalWT = 0, totalTAT = 0;

    printf("\nGantt Chart:\n");

    while (completed < N) {
        int selected = -1;

        // Find the shortest arrived process
        for (int i = 0; i < N; i++) {
            if (!p[i].done && p[i].at <= time) {
                if (selected == -1 ||
                    p[i].bt < p[selected].bt ||
                    (p[i].bt == p[selected].bt &&
                     p[i].at < p[selected].at)) {
                    selected = i;
                }
            }
        }

        // If no process has arrived, move time forward
        if (selected == -1) {
            int nextArrival = -1;

            for (int i = 0; i < N; i++) {
                if (!p[i].done &&
                    (nextArrival == -1 ||
                     p[i].at < nextArrival)) {
                    nextArrival = p[i].at;
                }
            }

            time = nextArrival;
            continue;
        }

        int start = time;
        time += p[selected].bt;

        p[selected].ct = time;
        p[selected].tat = p[selected].ct - p[selected].at;
        p[selected].wt = p[selected].tat - p[selected].bt;
        p[selected].done = 1;

        printf("P%d (%d-%d)\n",
               p[selected].id, start, time);

        totalWT += p[selected].wt;
        totalTAT += p[selected].tat;
        completed++;
    }

    printf("\nResults:\n");
    printf("Process\tAT\tBT\tCT\tTAT\tWT\n");

    for (int i = 0; i < N; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",
               p[i].id, p[i].at, p[i].bt,
               p[i].ct, p[i].tat, p[i].wt);
    }

    printf("\nAverage Waiting Time = %.2f\n",
           totalWT / N);
    printf("Average Turnaround Time = %.2f\n",
           totalTAT / N);

    return 0;
}
