sorttest: main.c
	gcc main.c -Wall -g -lm -o sorttest

andrun: sorttest
	./sorttest

andtime: sorttest
	time ./sorttest

clean:
	rm sorttest
