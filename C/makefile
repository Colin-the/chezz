CC = gcc
# CFLAGS = -Wall -std=c11 -g -fPIC

# remove -Wall as there will be 100's of -Wchar-subscripts warnings in my program as I used char's to access array elements to save memory opposed to int's
#this is a design choice and is not actually an error
CFLAGS = -std=c11 -g -fPIC -D_POSIX_C_SOURCE=200112L 
LDFLAGS = -pthread -lrt

all: engine

chezzMoves.o: chezzMoves.c chezz.h 
	$(CC) $(CFLAGS) -c chezzMoves.c -o chezzMoves.o

chezzUI.o: chezzUI.c chezz.h 
	$(CC) $(CFLAGS) -c chezzUI.c -o chezzUI.o

chezz.o: chezz.c chezz.h 
	$(CC) $(CFLAGS) -c chezz.c -o chezz.o

engine.o: Engine.c chezz.h
	$(CC) $(CFLAGS) -c Engine.c -o engine.o

eval.o: Eval.c chezz.h
	$(CC) $(CFLAGS) -c Eval.c -o eval.o

opening.o: opening.c chezz.h
	$(CC) $(CFLAGS) -c opening.c -o opening.o

engine: chezz.o chezzUI.o chezzMoves.o engine.o eval.o opening.o
	$(CC) chezz.o chezzUI.o chezzMoves.o engine.o eval.o opening.o -o run $(LDFLAGS)

clean:  
	rm -f *.o *.so run
