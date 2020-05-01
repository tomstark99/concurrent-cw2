# concurrent-cw2

use `make build` to build the project

use `make launch-qemu` to launch the QEMU in the emulation terminal

use `nc 127.0.0.1 1235` to connect the console to the QEMU in the console terminal

use `make launch-gdb` to launch the debugger in the debugging terminal

at this point you are ready for QEMU to execute the kernel image so typing `c` or `continue` in the debugging terminal will resume execution e.g. where execution was started initially or was last halted

you should see `console$` be output on the console terminal here you can issue two commands

`execute` followed by the name of the program you wish to execute

`terminate` followed by the pid number of the process you wish to kill

list of programs you can execute

- `P3`
- `P4`
- `P5`
- `philosophers`
