# My Zynq FreeRTOS Sandbox
This is my sandbox for experimenting with the features offered by the AMD (Xilinx) FreeRTOS port. The main platform used is the Digilent Zybo-z7-20. The implemented system is quite simple, comprising a range of GPIO features (LEDs, buttons, switches, and test outputs for monitoring) and two UART communication channels. The IDE is Vitis 2023.2 (Classic version). More details on the system can be found here (TBD). A list of projects is as follows:

- [Semaphore Project 1: UART command Handler](semaphore-project-1-uart-command-handler)
- [Queue Project 1: UART Command Handler](queue-project-1-uart-command-handler)
- [Semaphore/Queue Project 2: HW Tick Timers and UART Command Handler](semaphorequeue-project-2-hw-tick-timers-and-uart-command-handler)
- [Notifier Project 1: UART Comms Done Notifier System](notifier-project-1-uart-comms-done-notifier-system)
<br/><br/>
## Projects
### Semaphore Project 1: UART command Handler
**Brief Description:** Three tasks are running on the system:
1. Idle
2. 10ms background task
3. A Command Handler task
   
[Source Code](/2023.2/zybo-z7-20/hw-proj1/vitis_classic/semaphore_uart_cmd_handler/src)

The command handler task blocks waiting for a semaphore sent by the UART ISR. When the UART receives 64 bytes from the host software, the interrupt fires and the semaphore is sent. The command handler task is unblocked and it executes the requested command. The task then sends the command response back to the host software and returns to the blocked state. (Note that while the UART Tx interrupt fires after the response phase, it is not specifically tied to any task functionality.)

<br/><br/>
![Tasks for Semaphore Project 1: UART command Handler](/assets/images/semaphore-uart-cmd-handler-tasks.png)

<br/><br/>
### Queue Project 1: UART Command Handler
**Brief Description:** Four tasks are running on the system:
1. Idle
2. 10ms background task
3. Command handler receive task
4. Command handler transmit task

[Source Code](/2023.2/zybo-z7-20/hw-proj1/vitis_classic/queue_uart_cmd_handler/src)

This project is similar to the one above, except that queues are now used in the system instead of a semaphore. The command handler Rx task blocks waiting for a queue packet, which contains the 64-bytes of data and the data source. When the UART receives the data from the host SW, the UART Rx interrupt fires and the ISR sends the packet to the command handler Rx task. This task executes the command, and sends a queue Tx packet to the command handler Tx task (which was blocked waiting for the packet). The Tx task sends the 64 bytes of data back to the host software. 

<br/><br/>
![Tasks for Semaphore Project 1: UART command Handler](/assets/images/queue-uart-cmd-handler-tasks.png)

<br/><br/>
### Semaphore/Queue Project 2: HW Tick Timers and UART Command Handler
**Brief Description:** Six tasks are running on the system:
1. Idle
2. 10ms background task
3. Command handler receive task
4. Command handler transmit task
5. Hardware Timer Task 1 (75us)
6. Hardware Timer Task 2 (100us)

[Source Code](/2023.2/zybo-z7-20/hw-proj1/vitis_classic/semaphore_hw_tick_timers/src)

Two hardware tick timers are added to the system in this project. The first two triple timer counter modules of TTC0 in the Zynq-7000 processor are used for HW timing: TTC0-0 is set to generate an interrupt every 75us; TTC0-1 is set to generate an interrupt every 100us. A timer task is associated with each TTC module; each task remains blocked waiting for a semaphore that is sent by the related timer ISR. When the semaphore is received, the task is unblocked and it executes before returning to the blocked state. Note that the command handler tasks are still present in the system. The HW tick timers must run at the highest priorities (relatively) if timing is the most important factor in the system.

<br/><br/>
![Semaphore/Queue Project 2: HW Tick Timers](/assets/images/semaphore-tick-timers-tasks2.png)


Full system:
<br/><br/>
![Semaphore/Queue Project 2: HW Tick Timers and UART Command Handler](/assets/images/semaphore-tick-timers-tasks1.png)

<br/><br/>
### Notifier Project 1: UART Comms Done Notifier System
**Brief Description:** Seven tasks are running on the system:
1. Idle
2. 10ms background task
3. Command handler receive task
4. Command handler transmit task
5. Hardware Timer Task 1 (75us)
6. Hardware Timer Task 2 (100us)
7. Low priority UART Comms Done Task

[Source Code](/2023.2/zybo-z7-20/hw-proj1/vitis_classic/uart_comms_done_notifier/src)

In this system, a low-priority task is added as a basic demonstration of notifiers. The UART Comms Done task waits on a notifier sent by the Command handler Tx task; As the name suggests, the notifier indicates that the UART Rx-Tx transaction is complete. The notifier is set to increment every time it is sent, and so it is used to keep a count of UART transactions in the system.

<br/><br/>
![Notifer Project 1: UART Comms Done Notifier System](/assets/images/notifier-uart-comms-done-tasks.png)
