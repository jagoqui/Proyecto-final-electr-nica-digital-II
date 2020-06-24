OBJS = obj/main.o obj/file.o obj/globals.o obj/controlUnit.o obj/breakpoint.o
BYNARY = bin/sim
CFLAGS = -g -Wall

all: clear clean bin/sim

#This target will compile all the file

bin/sim: $(OBJS)
		g++ -o $(BYNARY) $(OBJS)

obj/main.o: 
		g++ $(CFLAGS) -c  src/main.cpp -o obj/main.o

obj/file.o:
		g++ $(CFLAGS) -c -Iinclude src/file.cpp -o obj/file.o

obj/globals.o:
		g++ $(CFLAGS) -c -Iinclude src/globals.cpp -o obj/globals.o 

obj/controlUnit.o:
		g++ $(CFLAGS) -c -Iinclude src/controlUnit.cpp -o obj/controlUnit.o

obj/breakpoint.o:
		g++ $(CFLAGS) -c -Iinclude src/breakpoint.cpp -o obj/breakpoint.o

clear:
		clear

clean:
		rm -f $(BYNARY) $(OBJS)




		
