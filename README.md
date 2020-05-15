# concurrent-cw2

## branch `master`

---

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

## branch `stage3_lcd`

---

use `make build` to build the project

use `make launch-qemu` to launch the QEMU in the emulation terminal

use `nc 127.0.0.1 1235` to connect the console to the QEMU in the console terminal

wait for the QEMU LCD window to show, after which use `make launch-gdb` to launch the debugger in the debugging terminal

at this point you are ready for QEMU to execute the kernel image and display the LCD so typing `c` or `continue` in the debugging terminal will resume execution e.g. where execution was started initially or was last halted and prompt the LCD to be drawn.

you should see `console$` be output on the console terminal, but also see the LCD screen draw some information relating to the operating system.

At any one time the list of processes will be displayed with information about their operation

- A **red** process indicates an inactive or (`STATUS_TERMINATED`) process, with no program information
- A **green** process indicates an active process, with the program it is executing displayed below it
- A **white** process indicates a currently executing process which means this process has been chosen to execute

### Keyboard Operation

As the LCD should say there are 5 possible keyboard commands

- `3` (on the number row) will execute the program `P3`
- `4` (on the number row) will execute the program `P4`
- `5` (on the number row) will execute the program `P5`
- `P` will execute the program `philosophers`
- `T` will terminate all processes and reset the kernel

(note running this on a mac e.g. through ssh with X forwarding gives an unknown keycodes error because of the non-standard format the corresponding keys therefore are `H` = `3`, `G` = `4`, `Z` = `5` and `9` = `P`)

### Mouse Operation

The cursor is moveable (albeit with some patience). Clicking the LCD will now capture mouse and keyboard interrupts, therefore clicking the LCD at a point close to the drawn cursor is preferable. The cursor can be moved along the screen and when positioned over the X in the top right corner, all processes will be terminated and the kernel will reset.

Clicking the blue buttons for the programs will also execute them.

### Console operation

Operating the console is still very much the same as before with two commands

`execute` followed by the name of the program you wish to execute

`terminate` followed by the pid number of the process you wish to kill, or by typing `all` to kill all processes and reset the kernel.

list of programs you can execute

- `P3`
- `P4`
- `P5`
- `philosophers`
