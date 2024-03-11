CFLAGS	:= -g -Wall -Wextra -Wconversion -pedantic

TARGETS	:= array_test0 array_test1

all: $(TARGETS)

clean:
	rm -f *.o $(TARGETS)

array_test0: array_test0.c
	$(CC) $(CFLAGS) -o $@ $<
array_test1: array_test1.c array.c array.h
	$(CC) $(CFLAGS) -o $@ $< array.c
test: array_test0 array_test1
	-./array_test0
	-./array_test1

valgrind: array_test0 array_test1
	-valgrind -q ./array_test0
	-valgrind -q ./array_test1

.PHONY: all clean test valgrind
