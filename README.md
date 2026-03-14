# multi-drone-monitoring
A real-time multi-drone monitoring and control system using POSIX message queues, threads, and signals in C.
## Scenario
Imagine a central control station that monitors three drones flying in different zones of a restricted airspace. Each drone has a specific mission and constantly reports its status to the controller:
Drone 1 patrols its assigned zone and ensures it doesn’t cross boundaries. If it moves out of its zone or its battery is low, it immediately alerts the controller.
Drone 2 is tasked with capturing a moving target. It moves step by step toward the target and notifies the controller once it successfully captures it. It also warns if its battery is low.
Drone 3 monitors for intruders. If any unauthorized object enters the protected airspace, it detects the intruder and instantly sends an alert to the controller. Battery warnings are also sent.
The controller acts as the central hub:
It runs a telemetry thread that constantly reads messages from the drones via a POSIX message queue.
Each alert is printed on the console and logged into a file (log.txt) for later review.
If the user wants to stop the system, pressing Ctrl + C triggers a graceful shutdown: the controller safely terminates all drones, closes the message queue, and finalizes the log.

## Initial Block diagram
![image alt]([image_url](https://github.com/nilayadesai/multi-drone-monitoring/blob/f058a2c55f374a13f824dfa686203336f2734a41/initial_block_diagram.png))

## System Architecture



## High-Level Data flow
Each drone (Drone1, Drone2, Drone3) generates telemetry or alert messages:
Drone1 → position and battery alerts
Drone2 → target capture and battery alerts
Drone3 → intruder detection and battery alerts
Drones send messages via POSIX message queue (/drone_queue).
The Controller runs a telemetry thread that reads all messages from the queue in real-time.
The Controller then:
Prints the message to console for monitoring
Logs the message to log.txt for record keeping
If Ctrl + C (SIGINT) is pressed, the Controller sends SIGTERM to all drones, closes the message queue, and exits safely.

Drone Sensors / Simulation
        ↓
Drone Logic Processing
        ↓
Alert Message Generated
        ↓
POSIX Message Queue (/drone_queue)
        ↓
Controller Telemetry Thread
        ↓
Console Display + Log File Storage

## Process design

Controller Process
Creates three child processes for Drone1, Drone2, Drone3 using fork()
Starts a telemetry thread (pthread_create) to read messages from the message queue
Handles SIGINT to shutdown drones and cleanup resources
Drone Processes
Each drone is a separate process running its own logic:
Generate telemetry (position, target, or intruder detection)
Monitor battery levels
Send alerts/messages via message queue (mq_send)
Optionally, use semaphores to synchronize console output during battery recharge messages
Inter-Process Communication
Message Queue (/drone_queue): Drones → Controller
Signals: Controller → Drones (SIGTERM) for shutdown
File Handling: Controller writes messages to log.txt
Here is the summary for Process design 
| Process    | Role                         |
| ---------- | ---------------------------- |
| Controller | Main process managing system |
| Drone1     | Simulates zone monitoring    |
| Drone2     | Simulates target capture     |
| Drone3     | Simulates intruder detection |

Process creation:
Controller
   |
   |----fork()----> Drone1
   |
   |----fork()----> Drone2
   |
   |----fork()----> Drone3

Each drone is executed using: execl("./droneX","droneX",NULL)

## Thread design
In this project, the controller needs to constantly monitor messages from multiple drones while still being able to handle shutdown signals (SIGINT) and do other tasks.
If we didn’t use threads, the controller would block while waiting for messages, and it wouldn’t respond to shutdown signals properly.
Using a telemetry thread allows concurrent message handling, making the system real-time and responsive.
Telemetry Thread
Created in controller.c using pthread_create().
Function: Continuously read messages from the POSIX message queue (/drone_queue).
Tasks it performs:
Receive message from a drone (mq_receive)
Print message on the console
Write message into log.txt for record keeping
Runs independently of the main controller process, so the main process can:
Spawn drone processes
Handle shutdown signals (SIGINT)
Perform other controller tasks
Controller Process
     |
     +--> Telemetry Thread
             |
             +--> Wait for messages from message queue
             +--> Print to console
             +--> Log to log.txt
             
  ## IPC (Inter-Process Communication) Mapping
1. A message queue is a FIFO data structure in kernel space that allows processes to send and receive messages asynchronously. Unlike pipes, message queues can be accessed by unrelated processes and support message prioritization.
In this project:
Drones are producers: they send telemetry and alert messages.
The controller is the consumer: it reads messages in real-time via a telemetry thread.
The queue ensures safe and orderly communication even if multiple drones send messages simultaneously.
| Component                     | Role                                 | Function                                                |
| ----------------------------- | ------------------------------------ | ------------------------------------------------------- |
| Drone1, Drone2, Drone3        | Producers                            | Send telemetry/battery/alert messages using `mq_send()` |
| Controller (Telemetry Thread) | Consumer                             | Reads messages using `mq_receive()` and logs them       |
| Queue Name                    | `/drone_queue`                       | Shared IPC channel for all drones and controller        |
| Message Structure             | `struct message { char text[100]; }` | Contains alert or telemetry information                 |

2. Signals
Signals are asynchronous notifications sent to processes to notify them of events like interrupts or termination requests.
SIGINT (Ctrl + C): Sent by the user to the controller to initiate graceful shutdown.
SIGTERM: Sent by the controller to terminate all drone processes safely.
| Signal  | Sender     | Receiver   | Purpose                               |
| ------- | ---------- | ---------- | ------------------------------------- |
| SIGINT  | User       | Controller | Initiates system shutdown             |
| SIGTERM | Controller | Drones     | Terminates drone processes gracefully |

3. Semaphores (Optional / Drones)
A semaphore is used to control access to shared resources, preventing race conditions in concurrent systems.
In this project:
/print_lock semaphore is used by drones to synchronize console output, so messages during battery recharge do not mix and remain readable.

4. File Handling (log.txt)
In operating systems, file handling allows a program to store, read, or update data on persistent storage. Files are important because they retain data even after the program terminates. Proper file handling ensures data consistency, safety, and easy debugging.
Controller startup:
Opens log.txt (creates if it doesn’t exist) using open().
Opens the POSIX message queue to receive messages from drones.
Telemetry Thread:
Reads messages from /drone_queue using mq_receive().
Writes each message to log.txt using write().
Ensures each message ends with a newline (\n) for readability.
Controller Shutdown:
Closes log.txt using close().
Ensures all messages are saved before process termination.
Drone1, Drone2, Drone3
         |
         V
   /drone_queue
         |
         V
Telemetry Thread
         |
         +--> Console (real-time alerts)
         |
         +--> log.txt (persistent storage)

## Error Handling Strategy

In any real-time system, errors can happen due to system failures, resource unavailability, or invalid operations. Proper error handling ensures that the system doesn’t crash unexpectedly and provides informative messages for debugging.
In this project, we follow a structured error handling approach for:
Message Queues
Process Creation
Threading
File Operations
Other System Calls
1. Message Queue Errors
Scenario: mq_open, mq_send, mq_receive can fail due to:
Queue already exists or doesn’t exist
Permission issues
Queue full or empty
Handling Strategy:
Check the return value of mq_open, mq_send, mq_receive
Use perror() to print descriptive error messages
Call exit(1) on critical failures (e.g., controller cannot open queue)
Example:
mq = mq_open("/drone_queue", O_CREAT | O_RDONLY, 0666, &attr);
if(mq == (mqd_t)-1) {
    perror("mq_open failed");
    exit(1);
}
2. Process Creation Errors
Scenario: fork() and execl() can fail due to:
Insufficient system resources
Invalid executable path
Handling Strategy:
Check the return value of fork()
In child process, check if execl() fails and call perror() + exit(1)
Example:
drone1_pid = fork();
if(drone1_pid == 0) {
    execl("./drone1","drone1",NULL);
    perror("exec failed");
    exit(1);
}
3. Threading Errors
Scenario: pthread_create() can fail if:
System resources are exhausted
Invalid thread attributes
Handling Strategy:
Check the return value of pthread_create()
Print an error message and exit if thread creation fails
4. File Handling Errors
Scenario: open(), write(), close() can fail if:
File permissions are incorrect
Disk is full
Handling Strategy:
Check file descriptor returned by open()
Check return value of write()
Print errors using perror() and exit if necessary
5. Signals and Safe Shutdown
If any process receives SIGINT, the controller:
Sends SIGTERM to all drones
Waits for drones to exit (waitpid)
Closes message queue (mq_close) and unlinks it (mq_unlink)
Closes log file
This ensures no resources are left open and the system shuts down gracefully, even if an error occurs.

## Testing Approach
Testing ensures that the multi-drone monitoring system behaves as expected under different scenarios. We focus on functional testing, integration testing, and system reliability.
Functional Testing: Checks if each component works correctly (drones sending messages, controller receiving and logging).
Integration Testing: Checks if all processes (drones + controller) work together seamlessly.
System Testing: Checks if the system behaves correctly under real-world scenarios, e.g., low battery, intruder detection, shutdown.
| Step | Test Scenario                                            | Expected Outcome                                                       |
| ---- | -------------------------------------------------------- | ---------------------------------------------------------------------- |
| 1    | Start Controller alone                                   | Controller opens `log.txt` and message queue without errors            |
| 2    | Start Drone1, Drone2, Drone3                             | Each drone prints telemetry messages and sends messages to the queue   |
| 3    | Controller Telemetry Thread receives messages            | Messages appear on **console** and are **logged in `log.txt`**         |
| 4    | Drone battery low                                        | Alert message sent to controller, appears on console and log           |
| 5    | Drone crosses zone / captures target / intruder detected | Correct alert message sent and logged                                  |
| 6    | Press Ctrl + C on controller                             | Controller terminates drones safely, closes message queue and log file |
| 7    | Attempt message queue after shutdown                     | Should fail, indicating queue is properly unlinked                     |
| 8    | Simultaneous messages from multiple drones               | Messages received in order without data loss                           |

Testing Notes
Randomization: Drones use rand() for position and intruder detection, simulating different real-world conditions.
Logs Verification: After system runs, log.txt can be reviewed to ensure all messages are captured accurately.
Edge Cases: Test battery reaching 0, drone crossing zone limits, and multiple drones sending alerts at the same time.

## Run Instructions
1. Make sure the project is built
Follow the Build Steps first (make all)
This ensures all executables are ready: controller, drone1, drone2, drone3
2. Start the Controller
Run the controller executable:
./controller
The controller automatically launches all three drones as separate processes.
You will see messages like:
Controller started
Drone1 started
Drone2 started
Drone3 started

3. Observe Drone Telemetry
The Telemetry Thread in the controller continuously receives messages from drones.
Messages are displayed on the console in real-time, for example:
Drone1 position: (35 40 15)
Drone2 captured the target
Drone3: Intruder detected at (12,25,18)
Drone1 battery low

5. Check Persistent Logs
All messages are also written to log.txt in the project directory:
Drone1 crossed its zone
Drone2 captured the target
Drone3: Intruder detected at (12,25,18)
This allows post-run analysis and debugging.

5. Graceful Shutdown
To stop the system, press Ctrl + C in the terminal running the controller.
Controller handles SIGINT, and performs:
Sends SIGTERM to all drone processes
Waits for drones to exit (waitpid())
Closes the message queue and log file
Prints:
Shutdown signal received
controller shutdown complete
6. Notes
Always run the controller first, as drones depend on the message queue created by it.
Drones run automatically — no need to start them manually.
You can rerun the project multiple times; log.txt will append new messages each time.

## System Calls used
| System Call / API  | Purpose                                                  | Used In                       | Why                                                            |
| ------------------ | -------------------------------------------------------- | ----------------------------- | -------------------------------------------------------------- |
| `fork()`           | Creates a new child process                              | Controller                    | To launch each drone as a separate process                     |
| `execl()`          | Replaces the current process image with a new executable | Controller (after fork)       | To run the drone executables (drone1, drone2, drone3)          |
| `signal()`         | Registers a handler for specific signals                 | Controller                    | To handle Ctrl+C (SIGINT) and perform graceful shutdown        |
| `kill()`           | Sends a signal to a process                              | Controller                    | To terminate drone processes safely using SIGTERM              |
| `waitpid()`        | Waits for a child process to terminate                   | Controller                    | Ensures drones exit before controller terminates               |
| `mq_open()`        | Opens or creates a POSIX message queue                   | Controller & Drones           | Enables inter-process communication (IPC)                      |
| `mq_send()`        | Sends a message to a POSIX message queue                 | Drones                        | To send telemetry and alert messages to controller             |
| `mq_receive()`     | Receives a message from a POSIX message queue            | Controller (telemetry thread) | To read messages from drones in real-time                      |
| `mq_close()`       | Closes a message queue descriptor                        | Controller                    | Cleans up resources before exiting                             |
| `mq_unlink()`      | Deletes a POSIX message queue from the system            | Controller                    | Prevents leftover queues and name conflicts                    |
| `open()`           | Opens/creates a file and returns a file descriptor       | Controller                    | To open `log.txt` for logging messages                         |
| `write()`          | Writes data to a file descriptor                         | Controller                    | To log drone messages into `log.txt`                           |
| `close()`          | Closes a file descriptor                                 | Controller                    | Proper resource cleanup                                        |
| `pthread_create()` | Creates a new thread in the process                      | Controller                    | To run the telemetry thread for concurrent message reading     |
| `sem_open()`       | Opens or creates a named semaphore                       | Drones                        | To synchronize console output during battery recharge messages |
| `sleep()`          | Pauses execution for a number of seconds                 | Controller & Drones           | Simulates real-time operations and intervals                   |
| `perror()`         | Prints descriptive error messages based on `errno`       | Controller & Drones           | For error handling when system calls fail                      |

## Sample run output
Controller started
Drone1 started
Drone2 started
Drone3 started

Drone1 position: (35, 40, 15)
Drone2 captured the target
Drone3: Intruder detected at (12,25,18)
Drone1 battery low


























