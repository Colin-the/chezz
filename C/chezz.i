// Interface file for how the python will interact with the C code

%module chezz

%{
#include "chezz.h"
%}
%include "numpy.i"
%init %{
  import_array();
%}

%include "typemaps.i"

// Tell SWIG to handle `char*` as a string
%typemap(in) char* {
    if (!PyBytes_Check($input)) {
        PyErr_SetString(PyExc_TypeError, "Expected bytes");
        SWIG_fail;
    }
    Py_ssize_t len;
    char *ptr;
    if (PyBytes_AsStringAndSize($input, &ptr, &len) == -1) {
        PyErr_SetString(PyExc_ValueError, "Invalid input string");
        SWIG_fail;
    }
    $1 = ptr;
}

// Ensure `char*` is properly managed
%typemap(freearg) char* {
    // No manual memory management needed for stack-allocated input
}

// This applies the 2D‐array typemap to your board parameter
%apply (unsigned char IN_ARRAY2, int DIM1, int DIM2)
  { (unsigned char board[8][8], int DIM1, int DIM2) };

// Include the header
%include "chezz.h"
