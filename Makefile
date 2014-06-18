CC=gcc
LIB_HEADERS = -I/libraries
LIB = -pthread -lm

ring_manager.out: main.o 
	$(CC) $(LIB_HEADERS) -o $@ $<  $(LIB)

main.o: main.c 
	$(CC) -c  main.c

clean:
	rm -rf *.o 
	rm ring_manager.out