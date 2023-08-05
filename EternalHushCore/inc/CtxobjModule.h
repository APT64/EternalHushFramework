#include <Python.h>
#include <Windows.h>

PyObject* PyInit_ctxobj();
PyObject* echo(PyObject* self, PyObject* args);
PyObject* run(PyObject* self, PyObject* args);
PyObject* dialog(PyObject* self, PyObject* args);
PyObject* option(PyObject* self, PyObject* args);

static PyMethodDef ctxobj_methodsDef[] = {
   { "echo", echo, METH_VARARGS, nullptr },
   { "run", run, METH_VARARGS, nullptr },
   { "dialog", dialog, METH_VARARGS, nullptr },
   { "option", option, METH_VARARGS, nullptr },

   { nullptr, nullptr, 0, nullptr }
};

static PyModuleDef ctxobj_moduleDef = {
   PyModuleDef_HEAD_INIT,
   "ui",
   "EternalHush's script context module",
   -1,
   ctxobj_methodsDef
};