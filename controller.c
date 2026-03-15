#include "controller.h"
int fd;
mqd_t mq;

pid_t drone1_pid, drone2_pid, drone3_pid;

void handle_signal(int sig)
{
    printf("Shutdown signal received\n");
    kill(drone1_pid, SIGTERM);
    kill(drone2_pid, SIGTERM);
    kill(drone3_pid, SIGTERM);

    waitpid(drone1_pid, NULL, 0);
    waitpid(drone2_pid, NULL, 0);
    waitpid(drone3_pid, NULL, 0);
    close(fd);
    
    mq_close(mq);
    mq_unlink("/drone_queue");
    printf("controller shoutdown complete\n");
    exit(0);
}
void* telemetry_function(void *arg)
{
    struct message msg;
    while(1)
    {
       mq_receive(mq, (char *)&msg, sizeof(msg), NULL);
        //sem_wait(sem);
        printf("Received: %s\n", msg.text);
        //sem_post(sem);
        write(fd, msg.text, strlen(msg.text));
        write(fd, "\n", 1); 
    }

    return NULL;
}
int main()
{
    printf("\n Controller started\n");
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct message);
    attr.mq_curmsgs = 0;

    mq = mq_open("/drone_queue", O_CREAT | O_RDONLY, 0666, &attr);

    if(mq == (mqd_t)-1)
    {
        perror("mq_open failed");
        exit(1);
    }

    fd = open("log.txt", O_CREAT | O_WRONLY | O_APPEND, 0666);

    if(fd < 0)
    {
        perror("file open failed");
        exit(1);
    }

    signal(SIGINT, handle_signal);
    pthread_t telemetry_thread;
    pthread_create(&telemetry_thread,NULL,telemetry_function,NULL);
    
    drone1_pid = fork();
    if(drone1_pid==0)
    {
    printf("Drone1 started\n");
    execl("./drone1","drone1",NULL);
    perror("exec failed");
    exit(1);
    }
    
    drone2_pid = fork();
    if(drone2_pid==0)
    {
    printf("Drone2 started\n");
    execl("./drone2","drone2",NULL);
    perror("exec failed");
    exit(1);
    
    }

    drone3_pid = fork();
    if(drone3_pid==0)
    {
    printf("Drone3 started\n");
    execl("./drone3","drone3",NULL);
    perror("exec failed");
    exit(1);
    }
    
      while(1)
    {
        sleep(5);
    }
    


    return 0;
}
