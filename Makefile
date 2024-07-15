CC = gcc
CFLAGS = -pthread

all: ex3.out

ex3.out: main.o semaphore.o queue.o producer.o dispatch.o coeditor.o screen_manager.o item.o
	$(CC) $(CFLAGS) -o ex3.out main.o semaphore.o queue.o producer.o dispatch.o coeditor.o screen_manager.o item.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

semaphore.o: semaphore.c
	$(CC) $(CFLAGS) -c semaphore.c

queue.o: queue.c
	$(CC) $(CFLAGS) -c queue.c

producer.o: producer.c
	$(CC) $(CFLAGS) -c producer.c

dispatch.o: dispatch.c
	$(CC) $(CFLAGS) -c dispatch.c

coeditor.o: coeditor.c
	$(CC) $(CFLAGS) -c coeditor.c

screen_manager.o: screen_manager.c
	$(CC) $(CFLAGS) -c screen_manager.c

item.o: item.c
	$(CC) $(CFLAGS) -c item.c

clean:
	rm -f *.o ex3.out
