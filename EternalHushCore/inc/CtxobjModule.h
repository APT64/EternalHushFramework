#include <Python.h>
#include <Windows.h>
#include <NetModule.h>
#include <CryptoModule.h>

PyObject* PyInit_ctxobj();
PyObject* echo(PyObject* self, PyObject* args);
PyObject* run(PyObject* self, PyObject* args);
PyObject* dialog(PyObject* self, PyObject* args);
PyObject* option(PyObject* self, PyObject* args);
PyObject* print_critical(PyObject* self, PyObject* args);
PyObject* set_hostname(PyObject* self, PyObject* args);
PyObject* set_env(PyObject* self, PyObject* args);
PyObject* get_env(PyObject* self, PyObject* args);
PyObject* lock_session(PyObject* self, PyObject* args);

static PyMethodDef ctxobj_methodsDef[] = {
   { "echo", echo, METH_VARARGS, nullptr },
   { "run", run, METH_VARARGS, nullptr },
   { "dialog", dialog, METH_VARARGS, nullptr },
   { "option", option, METH_VARARGS, nullptr },
   { "set_hostname", set_hostname, METH_VARARGS, nullptr },

   { "set_env", set_env, METH_VARARGS, nullptr },
   { "get_env", get_env, METH_VARARGS, nullptr },

   { "create_new_connection", create_new_connection, METH_VARARGS, nullptr },
   { "add_exception_handler", add_exception_handler, METH_VARARGS, nullptr },
   { "tcp_send", tcp_send, METH_VARARGS, nullptr },
   { "tcp_recv", tcp_recv, METH_VARARGS, nullptr },
   { "lock_session", lock_session, METH_VARARGS, nullptr },

   { "import_rsa_key", import_rsa_key, METH_VARARGS, nullptr },
   { "decrypt_rsa_data", decrypt_rsa_data, METH_VARARGS, nullptr },
   { "encrypt_rsa_data", encrypt_rsa_data, METH_VARARGS, nullptr },
   { "decrypt_aes_data", decrypt_aes_data, METH_VARARGS, nullptr },
   { "encrypt_aes_data", encrypt_aes_data, METH_VARARGS, nullptr },

   { nullptr, nullptr, 0, nullptr }
};

static PyModuleDef ctxobj_moduleDef = {
   PyModuleDef_HEAD_INIT,
   "ctxObj",
   "EternalHush's script context module",
   -1,
   ctxobj_methodsDef
};