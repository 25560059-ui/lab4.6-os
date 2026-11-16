/*
 * Lab 4 - Exercise 4.6: Round Robin Scheduling Simulation
 * University of Information Technology (UIT)
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iRemainingBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
    int hasStarted;
} PCB;

void generateRRProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        P[i].iPID = i + 1;
        P[i].iArrival = rand() % 15;      // Arrival trong [0, 14]
        P[i].iBurst = 2 + (rand() % 11);  // Burst trong [2, 12]
        P[i].iRemainingBurst = P[i].iBurst;
        P[i].hasStarted = 0;
        P[i].iWaiting = 0;
        P[i].iTaT = 0;
        P[i].iResponse = -1;
    }
}

void runRR(int testCaseNum, int quantum) {
    int n = 5;
    PCB P[10];
    generateRRProcess(n, P);

    // Sắp xếp theo Arrival Time
    for(int i = 0; i < n - 1; i++) {
        for(int j = i + 1; j < n; j++) {
            if(P[i].iArrival > P[j].iArrival) {
                PCB temp = P[i];
                P[i] = P[j];
                P[j] = temp;
            }
        }
    }

    int currentTime = P[0].iArrival;
    int completed = 0;
    int queue[100], head = 0, tail = 0;
    int inQueue[10] = {0};

    queue[tail++] = 0;
    inQueue[0] = 1;

    while(head < tail) {
        int idx = queue[head++];
        inQueue[idx] = 0;

        if (!P[idx].hasStarted) {
            P[idx].iResponse = currentTime - P[idx].iArrival;
            P[idx].hasStarted = 1;
        }

        int execTime = (P[idx].iRemainingBurst > quantum) ? quantum : P[idx].iRemainingBurst;
        currentTime += execTime;
        P[idx].iRemainingBurst -= execTime;

        for(int i = 0; i < n; i++) {
            if(P[i].iArrival <= currentTime && P[i].iRemainingBurst > 0 && !inQueue[i]) {
                int alreadyIn = 0;
                for(int k = head; k < tail; k++) {
                    if(queue[k] == i) { alreadyIn = 1; break; }
                }
                if(!alreadyIn && i != idx && P[i].iRemainingBurst == P[i].iBurst && P[i].iArrival <= currentTime) {
                    queue[tail++] = i;
                    inQueue[i] = 1;
                }
            }
        }

        if(P[idx].iRemainingBurst > 0) {
            queue[tail++] = idx;
            inQueue[idx] = 1;
        } else {
            P[idx].iFinish = currentTime;
            P[idx].iTaT = P[idx].iFinish - P[idx].iArrival;
            P[idx].iWaiting = P[idx].iTaT - P[idx].iBurst;
            completed++;
        }

        if(head == tail && completed < n) {
            for(int i = 0; i < n; i++) {
                if(P[i].iRemainingBurst > 0) {
                    currentTime = P[i].iArrival;
                    queue[tail++] = i;
                    inQueue[i] = 1;
                    break;
                }
            }
        }
    }

    // Sắp xếp lại theo PID để in báo cáo
    for(int i = 0; i < n - 1; i++) {
        for(int j = i + 1; j < n; j++) {
            if(P[i].iPID > P[j].iPID) {
                PCB temp = P[i];
                P[i] = P[j];
                P[j] = temp;
            }
        }
    }

    printf("\n================ ROUND ROBIN TEST CASE %d (Q=%d) ================\n", testCaseNum, quantum);
    printf("PID\tArrival\tBurst\tFinish\tWaiting\tResponse\tTaT\n");
    double totalW = 0, totalTaT = 0;
    for(int i = 0; i < n; i++) {
        printf("P%d\t%d\t\t%d\t%d\t%d\t%d\t\t%d\n", 
            P[i].iPID, P[i].iArrival, P[i].iBurst, 
            P[i].iFinish, P[i].iWaiting, P[i].iResponse, P[i].iTaT);
        totalW += P[i].iWaiting;
        totalTaT += P[i].iTaT;
    }
    printf("-> Average Waiting Time (AWT)   = %.2f\n", totalW / n);
    printf("-> Average Turnaround Time (ATaT) = %.2f\n", totalTaT / n);
}

int main() {
    srand(time(2026));
    for(int t = 1; t <= 3; t++) {
        runRR(t, 4);
    }
    return 0;
}
