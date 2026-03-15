CC = gcc
CFLAGS = -pthread -lrt

all: controller drone1 drone2 drone3

controller: controller.c controller.h
	$(CC) controller.c -o controller $(CFLAGS)

drone1: drone1.c drone1.h
	$(CC) drone1.c -o drone1 $(CFLAGS)

drone2: drone2.c drone2.h
	$(CC) drone2.c -o drone2 $(CFLAGS)

drone3: drone3.c drone3.h
	$(CC) drone3.c -o drone3 $(CFLAGS)

run: all
	./controller

clean:
	rm -f controller drone1 drone2 drone3 log.txt
