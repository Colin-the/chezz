CC = gcc
# CFLAGS = -Wall -std=c11 -g -fPIC

# remove -Wall as there will be 100's of -Wchar-subscripts warnings in my program as I used char's to access array elements to save memory opposed to int's
#this is a design choice and is not actually an error
CFLAGS = -std=c11 -g -fPIC -D_POSIX_C_SOURCE=200112L 
LDFLAGS = -pthread -lrt

PYTHON_INCLUDES = $(shell python3.11-config --includes)
SWIG = swig
SWIG_LIB        := $(shell $(SWIG) -swiglib)
SWIG_INCLUDES   := -I. -I$(SWIG_LIB)

all: python/_chezz.so #this will comile the C code and set up a shared libary so we can use swig to call this code from python
# all: engine #this is just if we want the C code to get built

# chezz_wrap.c: chezz.i chezz.h
# 	$(SWIG) -python -o chezz_wrap.c chezz.i

python/engine.py: chezz.i chezz.h
	$(SWIG) -python -builtin -I. -outdir ../python/ $<

chezz_wrap.c: python/engine.py

chezzMoves.o: chezzMoves.c chezz.h 
	$(CC) $(CFLAGS) -c chezzMoves.c -o chezzMoves.o $(PYTHON_INCLUDES)

chezzUI.o: chezzUI.c chezz.h 
	$(CC) $(CFLAGS) -c chezzUI.c -o chezzUI.o $(PYTHON_INCLUDES)

chezz.o: chezz.c chezz.h 
	$(CC) $(CFLAGS) -c chezz.c -o chezz.o $(PYTHON_INCLUDES)

engine.o: Engine.c chezz.h
	$(CC) $(CFLAGS) -c Engine.c -o engine.o $(PYTHON_INCLUDES)

eval.o: Eval.c chezz.h
	$(CC) $(CFLAGS) -c Eval.c -o eval.o $(PYTHON_INCLUDES)

opening.o: opening.c chezz.h
	$(CC) $(CFLAGS) -c opening.c -o opening.o $(PYTHON_INCLUDES)

# Compile SWIG wrapper
chezz_wrap.o: python/engine.py chezz_wrap.c chezz.h
	$(CC) $(CFLAGS) -c chezz_wrap.c -o $@ $(PYTHON_INCLUDES)

# Build shared library for Python
python/_chezz.so: chezz.o chezzUI.o chezzMoves.o engine.o eval.o opening.o chezz_wrap.o
	$(CC) -shared $^ -o ../python/_chezz.so $(LDFLAGS)




# chezzMoves.o: chezzMoves.c chezz.h 
# 	$(CC) $(CFLAGS) -c chezzMoves.c -o chezzMoves.o

# chezzUI.o: chezzUI.c chezz.h 
# 	$(CC) $(CFLAGS) -c chezzUI.c -o chezzUI.o

# chezz.o: chezz.c chezz.h 
# 	$(CC) $(CFLAGS) -c chezz.c -o chezz.o

# engine.o: Engine.c chezz.h
# 	$(CC) $(CFLAGS) -c Engine.c -o engine.o

# eval.o: Eval.c chezz.h
# 	$(CC) $(CFLAGS) -c Eval.c -o eval.o

# opening.o: opening.c chezz.h
# 	$(CC) $(CFLAGS) -c opening.c -o opening.o

# engine: chezz.o chezzUI.o chezzMoves.o engine.o eval.o opening.o
# 	$(CC) chezz.o chezzUI.o chezzMoves.o engine.o eval.o opening.o -o run $(LDFLAGS)

clean:  
	rm -f *.o *.so run chezz_wrap.c ../python/chezz.py ../python/_chezz.so
