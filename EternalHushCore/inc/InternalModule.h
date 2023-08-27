#pragma once
#include <Python.h>

PyObject* PyInit_eternalhush();
PyObject* err_write(PyObject *self, PyObject *args);

static PyMethodDef methodsDef[] = {
    { "write", err_write, METH_VARARGS, nullptr },
    { nullptr, nullptr, 0, nullptr }
};

static PyModuleDef moduleDef = {
    PyModuleDef_HEAD_INIT,
    "_eternalhush",
    "EternalHush's internal module",
    -1,
    methodsDef
};
