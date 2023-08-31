#include <Windows.h>
#include <Python.h>
#include <CtxobjModule.h>

PyObject* echo(PyObject* self, PyObject* args) {
    PyObject* ethu = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(ethu, "ctxObj");
    PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    PyObject* command = PyObject_GetAttrString(entry_point, "echo");
    PyObject* result = PyObject_CallObject(command, args);

    Py_RETURN_NONE;
}

PyObject* run(PyObject* self, PyObject* args) {
    PyObject* ethu = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(ethu, "ctxObj");
    PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    PyObject* command = PyObject_GetAttrString(entry_point, "run");
    PyObject* result = PyObject_CallObject(command, args);

    Py_RETURN_NONE;
}

PyObject* dialog(PyObject* self, PyObject* args) {
    PyObject* ethu = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(ethu, "ctxObj");
    PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    PyObject* command = PyObject_GetAttrString(entry_point, "dialog");
    PyObject* result = PyObject_CallObject(command, args);

    return result;
}

PyObject* option(PyObject* self, PyObject* args) {
    PyObject* ethu = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(ethu, "ctxObj");
    PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    PyObject* command = PyObject_GetAttrString(entry_point, "option");
    PyObject* result = PyObject_CallObject(command, args);

    return result;
}

PyObject* print_critical(PyObject* self, PyObject* args) {
    PyObject* ethu = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(ethu, "ctxObj");
    PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    PyObject* command = PyObject_GetAttrString(entry_point, "print_critical");
    PyObject* result = PyObject_CallObject(command, args);

    Py_RETURN_NONE;
}

PyObject* set_hostname(PyObject* self, PyObject* args) {
    PyObject* ethu = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(ethu, "ctxObj");
    PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    PyObject* command = PyObject_GetAttrString(entry_point, "update_hostname");
    PyObject* result = PyObject_CallObject(command, args);

    Py_RETURN_NONE;
}

PyObject* PyInit_ctxobj() {
    PyObject* ctxObj_module = PyModule_Create(&ctxobj_moduleDef);

    PyModule_AddIntConstant(ctxObj_module, "ECHO_DEFAULT", 0);
    PyModule_AddIntConstant(ctxObj_module, "ECHO_GOOD", 1);
    PyModule_AddIntConstant(ctxObj_module, "ECHO_ERROR", 2);
    PyModule_AddIntConstant(ctxObj_module, "ECHO_WARNING", 3);
    
    PyModule_AddIntConstant(ctxObj_module, "OPTION_YES", 1);
    PyModule_AddIntConstant(ctxObj_module, "OPTION_NO", 0);
    PyModule_AddIntConstant(ctxObj_module, "OPTION_CANCEL", -1);
    
    PyModule_AddIntConstant(ctxObj_module, "RUN_SILENT", 1);
    PyModule_AddIntConstant(ctxObj_module, "RUN_NORMAL", 0);

    PyModule_AddIntConstant(ctxObj_module, "TCP_CONNECTION", 2);
    PyModule_AddIntConstant(ctxObj_module, "HTTP_CONNECTION", 4);

    PyModule_AddIntConstant(ctxObj_module, "LONGLONG", 8);
    PyModule_AddIntConstant(ctxObj_module, "LONG", 4);
    PyModule_AddIntConstant(ctxObj_module, "SHORT", 2);
    PyModule_AddIntConstant(ctxObj_module, "CHAR", 1);

    PyModule_AddIntConstant(ctxObj_module, "PRIVATE_KEY", 0);
    PyModule_AddIntConstant(ctxObj_module, "PUBLIC_KEY", 1);

    PyModule_AddStringConstant(ctxObj_module, "INT", "INT");
    PyModule_AddStringConstant(ctxObj_module, "STRING", "STR");

    return ctxObj_module;
}
