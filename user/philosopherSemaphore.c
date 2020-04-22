#include "philosopherSemaphore.h"
#include "philosophers.h"


void main_PhilosopherSemaphore() {

    char s[10];
    int pid;
    int sit_out = -1;

    for(int i = 0; i < NUM_PHIL; i++) {
        int aux = phil();
        if(aux == philosophers[i].pid) {
            pid = i;
        }
    }
    
    philosophers[pid%NUM_PHIL].state = HUNGRY;

    while(1) {

        for(int j = 0; j < 3; j++) {
            wait(rand2());
        }
        char t[10];
        itoa(t, pid+1);
        write(STDOUT_FILENO, "\n\n", 2);
        write(STDOUT_FILENO, "philosopher ", 12);
        write(STDOUT_FILENO, t, strlen(t));
        write(STDOUT_FILENO, "\n[ WAITER ] ", 12);
        if(*global_waiter == FREE) {
            write(STDOUT_FILENO, "free\n", 5);
        } else {
            write(STDOUT_FILENO, "taken\n", 6);
        }
        for(int i = 0; i < NUM_PHIL - 1; i++) {
            itoa(t, i+1);
            if(*philosophers[i%NUM_PHIL].left == FREE) {
                write(STDOUT_FILENO, "|", 1);
            } else {
                write(STDOUT_FILENO, "X", 1);
            }
            if(philosophers[i%NUM_PHIL].state == EATING) {
                write(STDOUT_FILENO, " [e", 3);
            } else if (philosophers[i%NUM_PHIL].state == HUNGRY) {
                write(STDOUT_FILENO, " [h", 3);
            } else if (philosophers[i%NUM_PHIL].state == THINKING) {
                write(STDOUT_FILENO, " [t", 3);
            } else {
                write(STDOUT_FILENO, " [ ", 3);
            } 
            write(STDOUT_FILENO, t, strlen(t));
            write(STDOUT_FILENO, " ] ", 3);
        }
        itoa(t, 16);
        if(*philosophers[15].left == FREE) {
            write(STDOUT_FILENO, "|", 1);
        } else {
            write(STDOUT_FILENO, "X", 1);
        }
        if(philosophers[15].state == EATING) {
        write(STDOUT_FILENO, " [e", 3);
        } else if (philosophers[15].state == HUNGRY) {
            write(STDOUT_FILENO, " [h", 3);
        } else if (philosophers[15].state == THINKING) {
            write(STDOUT_FILENO, " [t", 3);
        } else {
            write(STDOUT_FILENO, " [ ", 3);
        } 
        write(STDOUT_FILENO, t, strlen(t));
        write(STDOUT_FILENO, " ] ", 3);
        if(*philosophers[15].right == FREE) {
            write(STDOUT_FILENO, "|", 1);
        } else {
            write(STDOUT_FILENO, "X\n\n", 3);
        }

// ================================================================================

        if(sit_out < 2) {
            sit_out++;
        } else {
            sit_out = 0;
            philosophers[pid%NUM_PHIL].state = HUNGRY;
        }

        if(philosophers[pid%NUM_PHIL].state == HUNGRY) {

            sem_wait(global_waiter);

            itoa(s, pid+1);
            write(STDOUT_FILENO, "\n\nphilosopher ", 14);
            write(STDOUT_FILENO, s, strlen(s));
            write(STDOUT_FILENO, " has the waiter\n", 17);

            if(*philosophers[pid].left == FREE && *philosophers[pid].right == FREE) {

                sem_wait(philosophers[pid%NUM_PHIL].left);
                sem_wait(philosophers[pid%NUM_PHIL].right);

                itoa(s, pid+1);
                write(STDOUT_FILENO, "philosopher ", 14);
                write(STDOUT_FILENO, s, strlen(s));
                write(STDOUT_FILENO, " picked up their forks and is eating\n", 38);

                philosophers[pid%NUM_PHIL].state = EATING;

                sem_post(global_waiter);

                itoa(s, pid+1);
                write(STDOUT_FILENO, "philosopher ", 14);
                write(STDOUT_FILENO, s, strlen(s));
                write(STDOUT_FILENO, " released the waiter\n\n", 22);

                for(int j = 0; j < 3; j++) {
                    wait(rand2());
                }

                sem_post(philosophers[pid%NUM_PHIL].left);
                sem_post(philosophers[pid%NUM_PHIL].right);

                philosophers[pid%NUM_PHIL].state = THINKING;
                
                itoa(s, pid+1);
                write(STDOUT_FILENO, "\nphilosopher ", 14);
                write(STDOUT_FILENO, s, strlen(s));
                write(STDOUT_FILENO, " finished and replaced their forks\n", 36);
            }
            else {
                sem_post(global_waiter);

                itoa(s, pid+1);
                write(STDOUT_FILENO, "philosopher ", 14);
                write(STDOUT_FILENO, s, strlen(s));
                write(STDOUT_FILENO, " released the waiter (couldn't eat)\n\n", 37);
            }
        }
    }
    exit(EXIT_SUCCESS);
}
