#include "philosophers.h"

extern void main_PhilosopherProcess();
const uint32_t locked = 0;

void initialiseMutexes() {
    for (int i = 0; i < NUM_PHIL; i++) {
        forks[i] = FREE;
    }
}

void runPhilosophers() {
    for (int i = 0; i < NUM_PHIL; i++) {
        philosophers[i].left = &forks[(i-1) % NUM_PHIL];
        philosophers[i].right = &forks[i % NUM_PHIL];
    }
    int aux = 0;
    for (int i = 0; i < NUM_PHIL; i++) {
        aux = fork();
        if(aux != 0) {
            puts( "parent\n", 7 );
        }
        else if( aux == 0 ) {
            exec(&main_PhilosopherProcess);

        }
    }
    
}

void main_Philosophers() {
    initialiseMutexes();
    runPhilosophers();
    exit(EXIT_SUCCESS); 
}
