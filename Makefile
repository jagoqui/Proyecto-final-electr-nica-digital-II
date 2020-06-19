OBJS = obj/main.o obj/file.o obj/globals.o obj/controlUnit.o obj/breakpoint.o
BYNARY = bin/sim

all: clean bin/sim

#This target will compile all the file

bin/sim: $(OBJS)
		g++ -o $(BYNARY) $(OBJS)

obj/main.o: 
		g++ -c  src/main.cpp -o obj/main.o

obj/file.o:
		g++ -c -Iinclude src/file.cpp -o obj/file.o

obj/globals.o:
		g++ -c -Iinclude src/globals.cpp -o obj/globals.o

obj/controlUnit.o:
		g++ -c -Iinclude src/controlUnit.cpp -o obj/controlUnit.o

obj/breakpoint.o:
		g++ -c -Iinclude src/breakpoint.cpp -o obj/breakpoint.o

clean:
		rm -f $(BYNARY) $(OBJS)


		
