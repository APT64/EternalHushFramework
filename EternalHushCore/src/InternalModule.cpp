#include <common.h>
#include <Python.h>
#include <CtxobjModule.h>
#include <InternalModule.h>

PyObject* err_write(PyObject* self, PyObject* args) {
    print_critical(self, args);
    return Py_BuildValue("");
}

PyObject* PyInit_eternalhush() {

    PyObject* eternalhush_module = PyModule_Create(&moduleDef);
    PySys_SetObject("stderr", eternalhush_module);
    PyModule_AddObject(eternalhush_module, "ctxObj", PyInit_ctxobj());
    return eternalhush_module;
}
