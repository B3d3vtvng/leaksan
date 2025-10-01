all: leak_detection

leak_detection: leak_detection.c
	gcc -o leak_detection leak_detection.c -llksan -g

clean:
	rm -f leak_detection *.out