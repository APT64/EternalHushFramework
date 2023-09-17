#include <Windows.h>
#include <Python.h>

PyObject* import_rsa_key(PyObject* self, PyObject* args);
PyObject* decrypt_rsa_data(PyObject* self, PyObject* args);
PyObject* encrypt_rsa_data(PyObject* self, PyObject* args);
PyObject* decrypt_aes_data(PyObject* self, PyObject* args);
PyObject* encrypt_aes_data(PyObject* self, PyObject* args);