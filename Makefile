CC=gcc
CFLAGS=-I.
DEPS = ring_list.h libraries/utils/utils.h mac_manipulation_functions.h
OBJ = ring_list.o utils.o mac_manipulation_functions.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

ringlist: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)