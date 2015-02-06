all: dcisg clean

dcisg: sync.o dcisg.o
	$(CC) -o dcisg sync.o dcisg.o
	
sync.o: sync.c sync.h
	$(CC) -c sync.c
	
dcisg.o: dcisg.c dcisg.h sync.h
	$(CC) -c dcisg.c
	
clean:
	rm sync.o dcisg.o
	