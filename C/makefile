CC = gcc
# CFLAGS = -Wall -std=c11 -g -fPIC

# remove -Wall as there will be 100's of -Wchar-subscripts warnings in my program as I used char's to access array elements to save memory opposed to int's
#this is a design choice and is not actually an error
CFLAGS = -std=c11 -g -fPIC -D_POSIX_C_SOURCE=200112L 
LDFLAGS = -pthread -lrt

PYTHON_INCLUDES = \
    $(shell python3.11-config --includes) \
    -I$(shell python3.11 -c "import numpy; print(numpy.get_include())")
SWIG = swig
SWIG_LIB        := $(shell $(SWIG) -swiglib)
SWIG_INCLUDES   := -I. -I$(SWIG_LIB)

all: python/_chezz.so #this will comile the C code and set up a shared libary so we can use swig to call this code from python
# all: engine #this is just if we want the C code to get built

python/chezz.py: chezz.i chezz.h
	@mkdir -p python
	$(SWIG) -python -builtin $(SWIG_INCLUDES) -outdir python $<

chezz_wrap.c: python/engine.py

chezzMoves.o: chezzMoves.c chezz.h 
	$(CC) $(CFLAGS) $(PYTHON_INCLUDES) -c $< -o $@

chezzUI.o: chezzUI.c chezz.h 
	$(CC) $(CFLAGS) $(PYTHON_INCLUDES) -c $< -o $@

chezz.o: chezz.c chezz.h 
	$(CC) $(CFLAGS) $(PYTHON_INCLUDES) -c $< -o $@

engine.o: Engine.c chezz.h
	$(CC) $(CFLAGS) $(PYTHON_INCLUDES) -c $< -o $@

eval.o: Eval.c chezz.h
	$(CC) $(CFLAGS) $(PYTHON_INCLUDES) -c $< -o $@

opening.o: opening.c chezz.h
	$(CC) $(CFLAGS) $(PYTHON_INCLUDES) -c $< -o $@	

# Compile SWIG wrapper
chezz_wrap.o: chezz_wrap.c chezz.h
	$(CC) $(filter-out -D_POSIX_C_SOURCE=200112L, $(CFLAGS)) \
	$(PYTHON_INCLUDES) -c chezz_wrap.c -o $@

# Build shared library for Python
python/_chezz.so: chezz.o chezzUI.o chezzMoves.o engine.o eval.o opening.o chezz_wrap.o
	@mkdir -p python
	$(CC) -shared $^ -o python/_chezz.so $(LDFLAGS) \
	$(shell python3.11-config --ldflags)




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
