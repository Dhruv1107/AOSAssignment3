all: main.o simulateleague.o
	gcc -o premierleague main.o simulateleague.o -O0
	
main.o: main.c premierleague.h
	gcc -c main.c -O0

simulateleague.o: simulateleague.c premierleague.h
	gcc -c simulateleague.c -O0
	
clean:
	rm -f *.o *.out
