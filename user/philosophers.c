#include "philosophers.h"

extern void main_PhilosopherProcess();
const uint32_t locked = 0;

void initialiseMutexes() {

}

void runPhilosophers() {
    int aux = 0;
    for (int i = 0; i < NUM_PHIL; i++) {
        aux = fork();
        if(aux != 0) {
            puts( "parent\n", 7 );
        }
        else if( aux == 0) {
            exec(&main_PhilosopherProcess);
        }
    }
    
}

void main_Philosophers() {
    initialiseMutexes();
    runPhilosophers();
    exit(EXIT_SUCCESS); 
}
