#include "drone1.h"
int main()
{
    mqd_t mq;
    sem_t *sem;
    sem=sem_open("/print_lock",O_CREAT,0644,1);
    int x,y,z;
    srand(time(NULL));
mq = mq_open("/drone_queue", O_WRONLY);
if(mq == (mqd_t)-1)
    {
        perror("mq_open failed");
        exit(1);
    }

    struct message msg;

    int battery=100;
    int battery_alert=0;

    while(1)
    {
        x=rand() % 70;
        y=rand() % 70;
        z=rand() % 30;
        battery=battery-10;
        printf("Drone1 position: (%d %d %d)\n",x,y,z);
        
        if(x > 50 || y > 50 || z > 20)
        {
        printf("drone1 crossed its zone\n");
        strcpy(msg.text,"Drone1 crossed its zone\n");
        mq_send(mq,(char*)&msg,sizeof(msg),0);
        
        }
        if(battery<20&& battery_alert==0)
        {
        printf("Drone 1 battery Low\n");
        strcpy(msg.text,"Drone1 battery low\n");
        mq_send(mq,(char*)&msg,sizeof(msg),0);
        battery_alert=1;
        }
        if(battery == 0)
{
    sem_wait(sem);
    printf("Drone1 battery charging...\n");
    sleep(1);

    printf("...\n");
    sleep(1);

    printf("...\n");
    sleep(1);

    printf("Drone1 battery charged\n");

    strcpy(msg.text,"Drone1 battery recharged");
    mq_send(mq,(char*)&msg,sizeof(msg),0);
    sem_post(sem);

    battery = 100;
    battery_alert = 0;
}
        sleep(2);
    }

    return 0;
}
