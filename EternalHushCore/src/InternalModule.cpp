#include <common.h>
#include <Python.h>
#include <CtxobjModule.h>
#include <InternalModule.h>

PyObject* PyInit_eternalhush() {

    PyObject* eternalhush_module = PyModule_Create(&moduleDef);
    
    PyModule_AddObject(eternalhush_module, "ctxObj", PyInit_ctxobj());
    return eternalhush_module;
}
