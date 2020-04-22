#include "philosophers.h"

extern void main_PhilosopherMutex();
extern void main_PhilosopherSemaphore();
//const uint32_t locked = 0;

char s[10];

void initialiseMutexes() {
    waiter = FREE;
    for (int i = 0; i < NUM_PHIL; i++) {
        forks[i] = FREE;
    }
}

void runPhilosophers() {
    int aux = 0;
    global_waiter = &waiter;
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

            //exec(&main_PhilosopherMutex);
            exec(&main_PhilosopherSemaphore);
            
        }
    }
}

void main_Philosophers() {
    initialiseMutexes();
    runPhilosophers();
    exit(EXIT_SUCCESS); 
}
