all: custom_alloc

custom_alloc: custom_alloc.c
	gcc -o custom_alloc custom_alloc.c -llksan -g

clean:
	rm -r custom_alloc custom_alloc.dSYM