all: main

main: main.o event.o PRIO.o scheduler.o
	g++ main.o event.o PRIO.o scheduler.o -o main

main.o: main.cpp
	g++ -c main.cpp

event.o: event.cpp
	g++ -c event.cpp

PRIO.o: PRIO.cpp
	g++ -c PRIO.cpp

scheduler.o: scheduler.cpp
	g++ -c scheduler.cpp

clean:
	rm *o main