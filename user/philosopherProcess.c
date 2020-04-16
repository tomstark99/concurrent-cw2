#include "philosopherProcess.h"


void main_PhilosopherProcess() {

    int state = THINKING;
    char s[10];
    int pid;

    while(1) {

        if (state == THINKING) {
            pid = phil(TAKEN);
            itoa(s, pid);
            write(STDOUT_FILENO, s, strlen(s));
            state = EATING;
        }

        if (state == EATING) {
            pid = phil(FREE);
            itoa(s, pid);
            write(STDOUT_FILENO, s, strlen(s));
            state = THINKING;
        }
    }
    puts( "I am a philosopher\n", 19 );

    exit(EXIT_SUCCESS);
}
