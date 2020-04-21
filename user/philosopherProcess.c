#include "philosopherProcess.h"


void main_PhilosopherProcess() {

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

                char t[10];
                for(int i = 0; i < NUM_PHIL - 1; i++) {
                    itoa(t, i);
                    if(philosophers[i%NUM_PHIL].left == FREE) {
                        write(STDOUT_FILENO, "|", 1);
                    } else {
                        write(STDOUT_FILENO, "X", 1);
                    }
                    write(STDOUT_FILENO, " [ ", 3);
                    write(STDOUT_FILENO, t, strlen(t));
                    write(STDOUT_FILENO, " ] ", 3);
                }
                itoa(t, 15);
                if(philosophers[15].left == FREE) {
                    write(STDOUT_FILENO, "|", 1);
                } else {
                    write(STDOUT_FILENO, "X", 1);
                }
                write(STDOUT_FILENO, " [ ", 3);
                write(STDOUT_FILENO, t, strlen(t));
                write(STDOUT_FILENO, " ] ", 3);
                if(philosophers[15].right == FREE) {
                    write(STDOUT_FILENO, "|", 1);
                } else {
                    write(STDOUT_FILENO, "X\n\n", 3);
                }

        for(int j = 0; j < 3; j++) {
            wait(rand2());
        }

        if(state == THINKING){
            if(*philosophers[pid].left == FREE && *philosophers[pid].right == FREE) {
                lock_mutex(philosophers[pid%NUM_PHIL].left);
                lock_mutex(philosophers[pid%NUM_PHIL].right);

                itoa(s, pid);
                write(STDOUT_FILENO, "\n\nphilosopher ", 14);
                write(STDOUT_FILENO, s, strlen(s));
                write(STDOUT_FILENO, " picked up their forks and is eating\n\n", 38);

                

                state = EATING;
            }
            else {
                itoa(s, pid);
                write(STDOUT_FILENO, "\n\nphilosopher ", 14);
                write(STDOUT_FILENO, s, strlen(s));
                write(STDOUT_FILENO, " couldn't eat, not enough forks\n\n", 33);
            }
        }
        for(int j = 0; j < 3; j++) {
            wait(rand2());
        }
        
        if(state == EATING) {
            unlock_mutex(philosophers[pid%NUM_PHIL].left);
            unlock_mutex(philosophers[pid%NUM_PHIL].right);

            itoa(s, pid);
            write(STDOUT_FILENO, "\n\nphilosopher ", 14);
            write(STDOUT_FILENO, s, strlen(s));
            write(STDOUT_FILENO, " finished and replaced their forks\n\n", 36);

            state = THINKING;
        }

    }
    exit(EXIT_SUCCESS);
}
