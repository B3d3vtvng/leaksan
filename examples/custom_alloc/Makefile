all: custom_alloc

custom_alloc: custom_alloc.c
	gcc -o custom_alloc custom_alloc.c -llksan -g

clean:
	rm -f custom_alloc *.out