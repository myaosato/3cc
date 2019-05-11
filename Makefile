3cc: 3cc.c

test: 3cc
	./3cc -test
	./test.sh

clean:
	rm -f 3cc *.o *~ tmp*
