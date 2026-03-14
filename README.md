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

##APIs used
| System Call / API           | Used In                       | Purpose / Why Used                                                                                                                |
| --------------------------- | ----------------------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| `fork()`                    | Controller                    | To create **separate processes** for Drone1, Drone2, Drone3. Each drone runs independently.                                       |
| `execl()`                   | Controller                    | To **replace the forked process** with the actual drone program (`drone1`, `drone2`, `drone3`). Ensures each drone runs its code. |
| `mqd_t` / `mq_open()`       | Controller & Drones           | To **create or open POSIX message queues** (`/drone_queue`) for inter-process communication (IPC).                                |
| `mq_send()`                 | Drone1, Drone2, Drone3        | To **send telemetry/alert messages** from drones to the controller.                                                               |
| `mq_receive()`              | Controller (Telemetry Thread) | To **receive messages** from drones in real-time.                                                                                 |
| `mq_close()`                | Controller & Drones           | To **close the message queue** when done.                                                                                         |
| `mq_unlink()`               | Controller                    | To **delete the message queue** after system shutdown. Prevents resource leakage.                                                 |
| `open()`                    | Controller                    | To **open `log.txt`** for writing telemetry messages.                                                                             |
| `write()`                   | Controller (Telemetry Thread) | To **write received messages into `log.txt`**.                                                                                    |
| `close()`                   | Controller                    | To **close the log file** after shutdown.                                                                                         |
| `signal()`                  | Controller                    | To **catch signals** (SIGINT) for **graceful shutdown**.                                                                          |
| `kill()`                    | Controller                    | To **terminate drone processes** safely on shutdown.                                                                              |
| `waitpid()`                 | Controller                    | To **wait for drone processes to exit** before final cleanup.                                                                     |
| `pthread_create()`          | Controller                    | To **create a separate telemetry thread** that continuously reads messages from the queue.                                        |
| `pthread_t`                 | Controller                    | Used to **manage the telemetry thread**.                                                                                          |
| `sem_open()`                | Drone1, Drone2, Drone3        | To **create/open a semaphore** for controlling access to shared console printing (optional in your current code).                 |
| `sem_wait()` / `sem_post()` | Drone1, Drone2, Drone3        | To **lock/unlock the semaphore** during battery charging messages so console output doesn’t mix.                                  |
| `perror()`                  | All processes                 | To **print error messages** if system calls fail.                                                                                 |
| `exit()`                    | All processes                 | To **terminate a process** immediately on error or after execution.                                                               |
| `sleep()`                   | Drones & Controller           | To **simulate time delays** for drone movement, battery charging, or controller idle loop.                                        |
| `srand()` / `rand()`        | Drones                        | To **generate random positions or intruder coordinates** in the simulation.                                                       |
| `strcpy()` / `sprintf()`    | Drones                        | To **format messages** before sending them to the message queue.                                                                  |

##Initial Block diagram



































