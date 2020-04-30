#include "philosopherSemaphore.h"
#include "philosophers.h"

/* example for solving the IPC and dining philosophers problem using semaphores
 * it is very similar to the implementation that uses mutexes but rather than locking the mutex (a binary semaphore) it sleeps execution of the thread at 
 * the line it requests a semaphore (to equal 0), the philosopher is then prevented from moving moving ahead with execution until access to the
 * critical section has become available, it does this for both requesting the waiter (a critical section) and requesting to use the forks if they
 * are free (another critical section while they are eating)
 * this also makes use of philosophers needing to be hungry in order to eat to avoid starvation
 */
void main_PhilosopherSemaphore() {

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
 * each philosopher individually will be labelled as follows | [ a/x:b ] | where '|' and 'X' are the same as mentioned above
 * a is the state of the philosopher e.g. e = eating, t = thinking, h = hungry,
 * x is the number of the philosopher and
 * b is the number of times that philosopher has eaten, I am keeping track of this to show that no philosopher is starved and my solution prevents starvation
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
        itoa(t, pid+1);
        write(STDOUT_FILENO, "\n\n", 2);
        write(STDOUT_FILENO, "philosopher ", 12);
        write(STDOUT_FILENO, t, strlen(t));
        write(STDOUT_FILENO, "\n[ WAITER ] ", 12);
        if(*global_waiter == FREE) { // if a process happens to stop execution as it still has access to the waiter this means all the other processes will have to sit out and wait for the waiter to become available again when the next execution of the process then releases the waiter, because of this if the waiter is taken we inform the user by printing it
            write(STDOUT_FILENO, "free ", 5); 
        } else {
            write(STDOUT_FILENO, "taken ", 6);
        }
        itoa(t, dif);
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
            if(philosophers[i%NUM_PHIL].state == EATING) {
                write(STDOUT_FILENO, " [ e/", 5);
            } else if (philosophers[i%NUM_PHIL].state == HUNGRY) {
                write(STDOUT_FILENO, " [ h/", 5);
            } else if (philosophers[i%NUM_PHIL].state == THINKING) {
                write(STDOUT_FILENO, " [ t/", 5);
            } else {
                write(STDOUT_FILENO, " [ ", 3);
            } 
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
        if(philosophers[15].state == EATING) {
        write(STDOUT_FILENO, " [ e/", 5);
        } else if (philosophers[15].state == HUNGRY) {
            write(STDOUT_FILENO, " [ h/", 5);
        } else if (philosophers[15].state == THINKING) {
            write(STDOUT_FILENO, " [ t/", 5);
        } else {
            write(STDOUT_FILENO, " [ ", 3);
        } 
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
// ================================================================================

        for(int j = 0; j < 3; j++) { // a philosopher waits for a random time before requesting the waiter
            wait(rand2());
        }

        /* this is old logic for preventing starvation, I used to make a philosopher sit out of two executions of the while loop
         * which also included all the wait periods so it wouldn't instantly cycle through them. This seemed to work but also
         * didn't seem like a very generic IPC mechanism and I therefore opted to use the wait mechanism like for all the rest
         * now when a philosopher has its three states hungry before it eats and thinking after it eats, to get from thinking to
         * hungry it simply waits a certain amount of time like it has done before it can request the waiter. This is certainly
         * more intuitive.
         */

        // if(sit_out < 2) { // once a philosopher has eaten it must sit out two turns before it becomes hungry again and can request to eat
        //     sit_out++;
        // } else {
        //     sit_out = 0;
        //     philosophers[pid%NUM_PHIL].state = HUNGRY;
        // }

        if(philosophers[pid%NUM_PHIL].state == HUNGRY) { // a philosopher may only request to eat if they are hungry

            sem_wait(global_waiter); // request the waiter, execution halts here before it is allowed access to the waiter e.g. allowed into the critical section this ensures mutual exclusion

            itoa(s, pid+1);
            write(STDOUT_FILENO, "\n\nphilosopher ", 14);
            write(STDOUT_FILENO, s, strlen(s));
            write(STDOUT_FILENO, " has the waiter\n", 17); // notify the qemu which philosopher currently has the waiter

            if(*philosophers[pid].left == FREE && *philosophers[pid].right == FREE) { // check to make sure both forks are free in order to eat

                sem_wait(philosophers[pid%NUM_PHIL].left); // lock both forks if they are free
                sem_wait(philosophers[pid%NUM_PHIL].right);

                itoa(s, pid+1);
                write(STDOUT_FILENO, "philosopher ", 14);
                write(STDOUT_FILENO, s, strlen(s));
                write(STDOUT_FILENO, " picked up their forks and is eating\n", 38); // notify the qemu that a philosopher is eating

                philosophers[pid%NUM_PHIL].state = EATING; // change the state to eating
                philosophers[pid%NUM_PHIL].eaten++; // add one to the number of times it has been allowed to eat

                sem_post(global_waiter); // release the waiter so a different philosopher may request to eat, if this is done after it has finised eating this is firstly unintuitive and secondly makes it very frequent that a philosopher holds onto the waiter beyond its turn therefore denying any philosopher the chance to eat a whole execution cycle

                itoa(s, pid+1);
                write(STDOUT_FILENO, "philosopher ", 14);
                write(STDOUT_FILENO, s, strlen(s));
                write(STDOUT_FILENO, " released the waiter\n\n", 22); // notify about the release of the waiter

                for(int j = 0; j < 3; j++) { // philosopher eats for a random time before they put down their forks again
                    wait(rand2());
                } 

                sem_post(philosophers[pid%NUM_PHIL].left);
                sem_post(philosophers[pid%NUM_PHIL].right); // release both left and right fork at the same time

                philosophers[pid%NUM_PHIL].state = THINKING; // change the state to thinking i.e. not hungry this means it cannot eat again straight away
                
                itoa(s, pid+1);
                write(STDOUT_FILENO, "\nphilosopher ", 14);
                write(STDOUT_FILENO, s, strlen(s));
                write(STDOUT_FILENO, " finished and replaced their forks\n", 36); // notify
            }
            else { // if both forks are not free go here
                sem_post(global_waiter); // release the waiter again so the next philosopher can ask to eat

                itoa(s, pid+1);
                write(STDOUT_FILENO, "philosopher ", 14);
                write(STDOUT_FILENO, s, strlen(s));
                write(STDOUT_FILENO, " released the waiter (couldn't eat)\n\n", 37); // notify that a philosopher couldn't eat
            }
        }
        if(philosophers[pid].state == THINKING) { // when a philosopher is not hungry it falls into here where it is made to wait a random time again before it becomes hungry again.
            for(int j = 0; j < 3; j++) {
                wait(rand2());
            }
            philosophers[pid].state = HUNGRY;
        }
    }
    exit(EXIT_SUCCESS); // exit with success
}
