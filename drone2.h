#ifndef DRONE2_H
#define DRONE2_H

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
