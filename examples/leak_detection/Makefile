all: leak_detection

leak_detection: leak_detection.c
	gcc -o leak_detection leak_detection.c -llksan -g

clean:
	rm -r leak_detection leak_detection.dSYM