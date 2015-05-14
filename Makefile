all: random.o main.o
	gcc *.o -o sim -lm
random.o: random.c random.h
	gcc random.c -c -O2
main.o: main.c
	gcc main.c -c -O2
clean:
	rm *.o sim
