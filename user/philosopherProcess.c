#include "philosopherProcess.h"


void main_PhilosopherProcess() {

    int state = THINKING;
    char s[10];
    int pid;

    while(1) {

        for(int j = 0; j < 3; j++){
            //wait(rand2());
        }

        if (state == THINKING) {
            pid = phil(TAKEN);
            itoa(s, pid);
            write(STDOUT_FILENO, s, strlen(s));
            write(STDOUT_FILENO, "eating\n", 7);
            state = EATING;
        }

        if (state == EATING) {
            pid = phil(FREE);
            itoa(s, pid);
            write(STDOUT_FILENO, s, strlen(s));
            write(STDOUT_FILENO, "not eating\n", 11);
            state = THINKING;
        }
    }
    exit(EXIT_SUCCESS);
}
