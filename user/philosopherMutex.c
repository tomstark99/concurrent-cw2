#include "philosopherMutex.h"


void main_PhilosopherMutex() {

    int state = THINKING;
    char s[10];
    int pid;

    for(int i = 0; i < NUM_PHIL; i++) {
        int aux = phil();
        if(aux == philosophers[i].pid) {
            pid = i;
        }
    }

    while(1) {

        for(int j = 0; j < 3; j++) {
            wait(rand2());
        }

        write(STDOUT_FILENO, "\n\n", 2);
        char t[10];
        for(int i = 0; i < NUM_PHIL - 1; i++) {
            itoa(t, i);
            if(*philosophers[i%NUM_PHIL].left == FREE) {
                write(STDOUT_FILENO, "|", 1);
            } else {
                write(STDOUT_FILENO, "X", 1);
            }
            write(STDOUT_FILENO, " [ ", 3);
            write(STDOUT_FILENO, t, strlen(t));
            write(STDOUT_FILENO, " ] ", 3);
        }
        itoa(t, 15);
        if(*philosophers[15].left == FREE) {
            write(STDOUT_FILENO, "|", 1);
        } else {
            write(STDOUT_FILENO, "X", 1);
        }
        write(STDOUT_FILENO, " [ ", 3);
        write(STDOUT_FILENO, t, strlen(t));
        write(STDOUT_FILENO, " ] ", 3);
        if(*philosophers[15].right == FREE) {
            write(STDOUT_FILENO, "|", 1);
        } else {
            write(STDOUT_FILENO, "X\n\n", 3);
        }

        if(state == THINKING){
            if(*philosophers[pid].left == FREE && *philosophers[pid].right == FREE) {
                mutex_lock(philosophers[pid%NUM_PHIL].left);
                mutex_lock(philosophers[pid%NUM_PHIL].right);

                itoa(s, pid);
                write(STDOUT_FILENO, "\nphilosopher ", 14);
                write(STDOUT_FILENO, s, strlen(s));
                write(STDOUT_FILENO, " picked up their forks and is eating\n", 37);

                state = EATING;
            }
            else {
                itoa(s, pid);
                write(STDOUT_FILENO, "\nphilosopher ", 13);
                write(STDOUT_FILENO, s, strlen(s));
                write(STDOUT_FILENO, " couldn't eat, not enough forks\n", 32);
            }
        }
        for(int j = 0; j < 3; j++) {
            wait(rand2());
        }
        
        if(state == EATING) {
            mutex_unlock(philosophers[pid%NUM_PHIL].left);
            mutex_unlock(philosophers[pid%NUM_PHIL].right);

            itoa(s, pid);
            write(STDOUT_FILENO, "\nphilosopher ", 13);
            write(STDOUT_FILENO, s, strlen(s));
            write(STDOUT_FILENO, " finished and replaced their forks\n", 35);

            state = THINKING;
        }

    }
    exit(EXIT_SUCCESS);
}
