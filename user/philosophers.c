#include "philosophers.h"

extern void main_PhilosopherProcess();
const uint32_t locked = 0;

void initialiseMutexes() {

}

void runPhilosophers() {
    int aux = 0;
    for (int i = 0; i < NUM_PHILOS; i++) {
        aux = fork();
        if(aux != 0) {

        }
        else if( aux == 0) {
            exec(&main_PhilosopherProcess);
        }
    }
    exit(EXIT_SUCCESS); 
}

void main_Philosophers() {
    initialiseMutexes();
    runPhilosophers();
    
}
