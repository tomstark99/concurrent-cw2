#include "philosopherMutex.h"

/* example for solving the IPC and dining philosophers problem using mutexes
 * it is very similar to the implementation that uses semaphores but rather than sleeping execution of the thread at the line of waiting for a semaphore
 * it simply locks the mutex (binary semaphore) and moves on with execution because of this the setup for eating and returning forks is slightly different
 * the principle is still the same and makes use of philosophers needing to be hungry in order to eat to avoid starvation.
 * 
 * I have found that the mutex implementation works much better because the difference of how many times a philosopher has eaten increases at a much slower rate
 * than the semaphore implementation
 */
void main_PhilosopherMutex() {

    char s[10]; // for printing to the qemu, I use the itoa function to convert an into to a char array so I can print it.
    int pid; // this pid is the index a given philosopher has within the philosopher array not the procTab array.
    int max; int min; int dif;

    for(int i = 0; i < NUM_PHIL; i++) {
        /* This is needed because the file that calls fork is not the same file that executes the logic for the philosophers
         * this file has no context to which philosopher is currently executing it except for the currently executing process
         * that is found in hilevel.c because of this a singular system call is made per philsosopher when the philosophers are initialised
         * the pid of the process in hilevel was captured when the fork call was made and can therefore be used to find the relevant
         * local philosopher pid to reference it during the execution.
         */
        int aux = phil(); // a system call is made to determine which philosopher is currently executing.
        if(aux == philosophers[i].pid) {
            pid = i; // this is determined to find the index within the philosopher array.
        }
    }
    
    philosophers[pid%NUM_PHIL].state = HUNGRY; // an initial state of hungry is set for all philosophers which means they are all allowed to request to eat.
    philosophers[pid%NUM_PHIL].eaten = 0; // the number of times a philosopher has eaten is initialised and set to 0.

    while(1) {

/* printing the table of forks and philosophers to the qemu in the form '| [ n ] |' where '|' is a free fork 'X' a taken fork and 'n' being the philosopher number
 * in the qemu window you will see something like this | [ 1 ] | [ 2 ] X [ 3 ] X [ 4 ] | [ 5 ] | ...
 * each philosopher individually will be labelled as follows | [ x:b ] | where '|' and 'X' are the same as mentioned above
 * x is the number of the philosopher and
 * b is the number of times that philosopher has eaten, I am keeping track of this to show that no philosopher is starved and my solution prevents starvation
 * I have decided to omit printing the state of the philosopher because I felt it took up too much room and was not vital to know.
 * for example, this indicates that philosopher 3 is currently eating e.g. the forks next to him are taken meaning if 2 or 4 requested to eat they couldn't because
 * both of their forks aren't free.
 */
        max = 0; min = 10000000; dif = 0;
        /* this informs the user of the operating system if there is any starvation amongst the philosophers
         * it works out per iteration max and min values for how many times the philsophers have eaten and
         * the the difference of the values, ideally this difference value should be as small as possible
         * which would mean the philsophers have an equal opportunity to eat
         */
        for(int i = 0; i < NUM_PHIL; i++) {
            if(max <= philosophers[i%NUM_PHIL].eaten) {
                max = philosophers[i%NUM_PHIL].eaten;
            }
            if(min >= philosophers[i%NUM_PHIL].eaten) {
                min = philosophers[i%NUM_PHIL].eaten;
            }
            
        }
        dif = max - min;
        char t[10];
        itoa(t, dif);
        write(STDOUT_FILENO, "\n\n", 2);
        write(STDOUT_FILENO, "dif: ", 5);
        write(STDOUT_FILENO, t, strlen(t)); // print out the difference value
        itoa(t, max);
        write(STDOUT_FILENO, " max: ", 6);
        write(STDOUT_FILENO, t, strlen(t)); // print out the max value
        itoa(t, min);
        write(STDOUT_FILENO, " min: ", 6);
        write(STDOUT_FILENO, t, strlen(t)); // print out the min value
        write(STDOUT_FILENO, "\n", 1);

        /* this is the printing logic for the philosopher table, it prints out philosopher 1 to 15 in the loop and leaves out 16
         * 16 needs to be printed separately because it prints out the wrap around fork value (so left and right fork) wheras all
         * the others just print out their left fork this means when philsopher 1 or 16 is eating its left or right fork respectively will
         * be the same
         */
        for(int i = 0; i < NUM_PHIL - 1; i++) {
            itoa(t, i+1);
            if(*philosophers[i%NUM_PHIL].left == FREE) {
                write(STDOUT_FILENO, "|", 1);
            } else {
                write(STDOUT_FILENO, "X", 1);
            }
            write(STDOUT_FILENO, " [ ", 3);
            write(STDOUT_FILENO, t, strlen(t));
            write(STDOUT_FILENO, ":", 1);
            itoa(t, philosophers[i%NUM_PHIL].eaten);
            write(STDOUT_FILENO, t, strlen(t));
            write(STDOUT_FILENO, " ] ", 3);
        }
        itoa(t, 16);
        if(*philosophers[15].left == FREE) {
            write(STDOUT_FILENO, "|", 1);
        } else {
            write(STDOUT_FILENO, "X", 1);
        }
        write(STDOUT_FILENO, " [ ", 3);
        write(STDOUT_FILENO, t, strlen(t));
        write(STDOUT_FILENO, ":", 1);
        itoa(t, philosophers[15].eaten);
        write(STDOUT_FILENO, t, strlen(t));
        write(STDOUT_FILENO, " ] ", 3);
        if(*philosophers[15].right == FREE) {
            write(STDOUT_FILENO, "|", 1);
        } else {
            write(STDOUT_FILENO, "X\n\n", 3);
        }

// philosopher logic begins
// ======================================================================================

/* the execution logic for mutexes is also a lot simpler than semaphores because I have not used a waiter
 * that needs to be requested in order to eat, I have not found this to be a problem in execution and
 * therefore did not feel the need to add one, if anything this can highlight the potential differences
 * of having a waiter and omiting it.
 * 
 * because this is an alternative implementation I have decided to make the structure of the code different too
 * instead of staying within the if statement for checking the forks for the entire time that the philosopher is
 * eating, I come out of the statement and have a different one that checks if the philosohper is eating to then
 * change the status and release the forks
 */

        for(int j = 0; j < 3; j++) {
            wait(rand2());
        }

        if(philosophers[pid].state == HUNGRY){ // check if the philosopher is hungry
            if(*philosophers[pid].left == FREE && *philosophers[pid].right == FREE) { // check if both forks are available
                mutex_lock(philosophers[pid%NUM_PHIL].left); // lock both forks, remembering that execution continues
                mutex_lock(philosophers[pid%NUM_PHIL].right);

                itoa(s, pid+1);
                write(STDOUT_FILENO, "\nphilosopher ", 14);
                write(STDOUT_FILENO, s, strlen(s));
                write(STDOUT_FILENO, " picked up their forks and is eating\n", 37);

                philosophers[pid].state = EATING;
                philosophers[pid].eaten++;
                
            }
            else {
                itoa(s, pid+1);
                write(STDOUT_FILENO, "\nphilosopher ", 13);
                write(STDOUT_FILENO, s, strlen(s));
                write(STDOUT_FILENO, " couldn't eat, not enough forks\n", 32);
            } 
        }
        for(int j = 0; j < 3; j++) {
            wait(rand2());
        }

        if(philosophers[pid].state == THINKING) {
            wait(rand2());
            philosophers[pid].state = HUNGRY;
        }
        
        if(philosophers[pid].state == EATING) {
            mutex_unlock(philosophers[pid%NUM_PHIL].left);
            mutex_unlock(philosophers[pid%NUM_PHIL].right);

            itoa(s, pid+1);
            write(STDOUT_FILENO, "\nphilosopher ", 13);
            write(STDOUT_FILENO, s, strlen(s));
            write(STDOUT_FILENO, " finished and replaced their forks\n", 35);

            philosophers[pid].state = THINKING;
        }

    }
    exit(EXIT_SUCCESS);
}
