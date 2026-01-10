#pragma once
#include <Python.h>

PyMODINIT_FUNC PyInit__eternalhush();
PyObject* err_write(PyObject *self, PyObject *args);
PyObject* info_write(PyObject *self, PyObject *args);
int eternalhush_exec(PyObject* module);

static PyModuleDef_Slot eternalhush_slots[] = {
   {Py_mod_exec, eternalhush_exec},
   {Py_mod_multiple_interpreters, Py_MOD_PER_INTERPRETER_GIL_SUPPORTED},
   {0, NULL}
};
static PyMethodDef methodsDef[] = {
    { "write", err_write, METH_VARARGS, nullptr },
    { NULL, NULL, 0, NULL }
};

static struct PyModuleDef moduleDef = {
    PyModuleDef_HEAD_INIT,
    "_eternalhush",
    "EternalHush's internal module",
    0,
    methodsDef,
    eternalhush_slots
};
