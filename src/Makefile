all: dcisg clean

dcisg: sync.o dcisg.o
	gcc -std=c99 -o dcisg sync.o dcisg.o
	
sync.o: sync.c sync.h
	gcc -std=c99 -c sync.c
	
dcisg.o: dcisg.c dcisg.h sync.h
	gcc -std=c99 -c dcisg.c
	
clean:
	rm sync.o dcisg.o
	