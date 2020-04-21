#include "philosophers.h"

extern void main_PhilosopherProcess();
//const uint32_t locked = 0;

char s[10];

void initialiseMutexes() {
    for (int i = 0; i < NUM_PHIL; i++) {
        forks[i] = FREE;
    }
}

void runPhilosophers() {
    int aux = 0;
    for (int i = 0; i < NUM_PHIL; i++) {
        philosophers[i].left = &forks[(i) % NUM_PHIL];
        philosophers[i].right = &forks[(i + 1) % NUM_PHIL];
        aux = fork();
        if(aux != 0) {
            itoa(s, aux);
            write(STDOUT_FILENO, s, strlen(s));
            philosophers[i].pid = aux;
        }
        else if( aux == 0 ) {

            exec(&main_PhilosopherProcess);

            // while(1) {

            //     for(int j = 0; j < 3; j++) {
            //         wait(rand2());
            //     }

            //     sem_wait(waiter);

            //     itoa(s, i);
            //     write(STDOUT_FILENO, "\n\nphilosopher ", 14);
            //     write(STDOUT_FILENO, s, strlen(s));
            //     write(STDOUT_FILENO, " has the waiter\n\n", 17);

            //     if(*philosophers[i].left == FREE && *philosophers[i].right == FREE) {

            //         sem_wait(philosophers[i%NUM_PHIL].left);
            //         sem_wait(philosophers[i%NUM_PHIL].right);

            //         itoa(s, i);
            //         write(STDOUT_FILENO, "\n\nphilosopher ", 14);
            //         write(STDOUT_FILENO, s, strlen(s));
            //         write(STDOUT_FILENO, " picked up their forks and is eating\n\n", 38);

            //         for(int j = 0; j < 3; j++) {
            //             wait(rand2());
            //         }

            //         sem_post(philosophers[i%NUM_PHIL].left);
            //         sem_post(philosophers[i%NUM_PHIL].right);

            //         write(STDOUT_FILENO, "\n\nphilosopher ", 14);
            //         write(STDOUT_FILENO, s, strlen(s));
            //         write(STDOUT_FILENO, " finished and replaced their forks\n\n", 36);

            //         sem_post(waiter);

            //         itoa(s, i);
            //         write(STDOUT_FILENO, "\n\nphilosopher ", 14);
            //         write(STDOUT_FILENO, s, strlen(s));
            //         write(STDOUT_FILENO, " released the waiter\n\n", 22);
            //     }
            //     else {
            //         sem_post(waiter);

            //         itoa(s, i);
            //         write(STDOUT_FILENO, "\n\nphilosopher ", 14);
            //         write(STDOUT_FILENO, s, strlen(s));
            //         write(STDOUT_FILENO, " released the waiter (couldn't eat)\n\n", 37);
            //     }
            // }
        }
    }
}

void main_Philosophers() {
    initialiseMutexes();
    runPhilosophers();
    exit(EXIT_SUCCESS); 
}
