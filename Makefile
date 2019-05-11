CFLAGS=-Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS: .c=.o)

3cc: $(OBJS)
	$(CC) -o 3cc $(OBJS) $(LDFLAGS)

$(OBJS): 3cc.h

test: 3cc
	./3cc -test
	./test.sh

clean:
	rm -f 3cc *.o *~ tmp*
