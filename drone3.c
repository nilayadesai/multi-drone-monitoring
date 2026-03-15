#include "drone3.h"

int main()
{
    mqd_t mq;
    sem_t *sem;
    sem=sem_open("/print_lock",O_CREAT,0644,1);
    
    int max_x = 60;
    int max_y = 70;
    int max_z = 75;

    int intruder_x;
    int intruder_y;
    int intruder_z;

    int battery = 100;
    int battery_alert = 0;

    srand(time(NULL));
    mq = mq_open("/drone_queue", O_WRONLY);
    if(mq == (mqd_t)-1)
    {
        perror("mq_open failed");
        exit(1);
    }

    struct message msg;
    

    while(1)
    {
        
        intruder_x = rand()%70;
        intruder_y = rand()%75;
        intruder_z = rand()%80;

        printf("Intruder position: (%d %d %d)\n", intruder_x, intruder_y, intruder_z);

        if(intruder_x<=max_x && intruder_y<=max_y && intruder_z<=max_z )
        {
            printf("Intruder detected in airspace\n");

           sprintf(msg.text, "Drone3: Intruder detected at (%d,%d,%d)\n", intruder_x, intruder_y, intruder_z);

            mq_send(mq,(char*)&msg,sizeof(msg),0);
        }
        battery=battery-10;

        if(battery<20 &&battery_alert==0 )
        {
            printf("Drone3 battery low!!!\n");
            strcpy(msg.text, "Drone3 battery low!!!\n");
            mq_send(mq,(char*)&msg,sizeof(msg),0);
            battery_alert=1;
        }
        if(battery <= 0)
        {
            sem_wait(sem);
            printf("Drone3 battery charging...\n");
            sleep(1);
            printf("...\n");
            sleep(1);
            printf("...\n");
            sleep(1);
            printf("Drone3 battery charged\n");

            strcpy(msg.text,"Drone battery recharged");
            mq_send(mq,(char*)&msg,sizeof(msg),0);
            sem_post(sem);

            battery = 100;
            battery_alert = 0;
          }
    sleep(2);
    }
    return 0;
    }

