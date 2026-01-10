#include <common.h>
#include <Python.h>
#include <CtxobjModule.h>
#include <InternalModule.h>

PyObject* err_write(PyObject* self, PyObject* args) {
    PyObject* console_id = NULL, * task_uid = NULL;
    PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
    console_id = PyObject_GetAttrString(ctxobj, "console_id");
    task_uid = PyObject_GetAttrString(ctxobj, "task_uid");
    char* str;
    PyArg_ParseTuple(args, "s", &str);
    PyObject* args_new = Py_BuildValue("sOO", str, console_id, task_uid);

    print_critical(self, args_new);
    return Py_BuildValue("");
}
 PyObject* info_write(PyObject* self, PyObject* args) {
    PyObject* console_id = NULL, *task_uid = NULL;
    PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
    console_id = PyObject_GetAttrString(ctxobj, "console_id");
    task_uid = PyObject_GetAttrString(ctxobj, "task_uid");
    char* str;
    PyArg_ParseTuple(args, "s", &str);
    PyObject* args_new = Py_BuildValue("sOO", str, console_id, task_uid);

    print_info(self, args_new);
    return Py_BuildValue("");
}
int eternalhush_exec(PyObject* module) {
     if (PyModule_AddObject(module, "ctxObj", PyInit_ctxobj()) < 0)
     {
         return -1;
     }
     return 0;
 };
PyMODINIT_FUNC PyInit__eternalhush() {

  //  PyObject* eternalhush_module = PyModule_Create(&moduleDef);

   // PySys_SetObject("stderr", eternalhush_module);
  //  PyModule_AddObject(eternalhush_module, "ctxObj", PyInit_ctxobj());
    PyObject* pymodule = PyModuleDef_Init(&moduleDef);
    return pymodule;
}
