#include <Windows.h>
#include <Python.h>

PyObject* create_new_connection(PyObject* self, PyObject* args);
PyObject* export_global_connection(PyObject* self, PyObject* args);
PyObject* tcp_send(PyObject* self, PyObject* args);
PyObject* tcp_recv(PyObject* self, PyObject* args);