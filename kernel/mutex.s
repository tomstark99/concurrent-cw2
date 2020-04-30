@ this is a general implementation for mutexes in C, using mutex_lock to lock a mutex and mutex_unlock to free it again

.equ locked,1
.equ unlocked,0

@ lock_mutex
@ Declare for use from C as extern void lock_mutex(void * mutex);
    .global mutex_lock
mutex_lock:
    LDR     r1, =locked
1:  LDREX   r2, [r0]
    CMP     r2, r1        @ Test if mutex is locked or unlocked
    BEQ     2f            @ If locked - wait for it to be released, from 2
    STREXNE r2, r1, [r0]  @ Not locked, attempt to lock it
    CMPNE   r2, #1        @ Check if Store-Exclusive failed
    BEQ     1b            @ Failed - retry from 1
    # Lock acquired
    DMB                   @ Required before accessing protected resource
    BX      lr

2:  @ Take appropriate action while waiting for mutex to become unlocked
    @ WAIT_FOR_UPDATE
    B       1b            @ Retry from 1


@ unlock_mutex
@ Declare for use from C as extern void unlock_mutex(void * mutex);
    .global mutex_unlock
mutex_unlock:
    LDR     r1, =unlocked
    DMB                   @ Required before releasing protected resource
    STR     r1, [r0]      @ Unlock mutex
    @ SIGNAL_UPDATE   
    BX      lr
