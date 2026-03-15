
#include "drone2.h"
int main()
{
    mqd_t mq;
    sem_t *sem;
    
    sem=sem_open("/print_lock",O_CREAT,0644,1);
    
    int x=10,y=30,z=4;
    int tar_x=20,tar_y=40,tar_z=10;
    int battery=100;
    int battery_alert = 0;
    
    srand(time(NULL));
    mq = mq_open("/drone_queue", O_WRONLY);
    if(mq == (mqd_t)-1)
    {
        perror("mq_open failed");
        exit(1);
    }

    struct message msg;
    int captured = 0;
    while(1)
    {
      printf("Drone2 moving to its target\n");
      battery=battery-10;
    if(x<tar_x)
    {
      x++;
    }
    if(y<tar_y)
    {
      y++;
    }
    if(z<tar_z)
    {
      z++;
    }
    if(x==tar_x && y==tar_y && z==tar_z&& captured==0)
    {
    
      printf("Drone2 captured target\n");
      captured=1;
      strcpy(msg.text,"Drone2 captured the target\n");
      mq_send(mq,(char*)&msg,sizeof(msg),0);
    }
    
    
    if(battery<20&&battery_alert==0)
        {
          printf("Drone 2 battery Low\n");
          battery_alert = 1;
          strcpy(msg.text,"Drone2 battery low\n");
          mq_send(mq,(char*)&msg,sizeof(msg),0);
        }
        
        if(battery <= 0)
        {
            sem_wait(sem);
            printf("Drone2 battery charging...\n");
            sleep(1);
            printf("...\n");
            sleep(1);
            printf("...\n");
            sleep(1);
            printf("Drone2 battery charged\n");
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

    
