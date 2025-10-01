all: liblksan.a

liblksan.a: lksan.o
	ar rcs liblksan.a lksan.o

lksan.o: lksan.c lksan.h
	gcc -c -g -o lksan.o lksan.c

clean:
	rm -f *.o *.a

install: liblksan.a lksan.h
	sudo cp liblksan.a /usr/local/lib
	sudo cp lksan.h /usr/local/include