#ifndef DRONE1_H
#define DRONE1_H

#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>

struct message
{
    char text[100];
};

#endif
