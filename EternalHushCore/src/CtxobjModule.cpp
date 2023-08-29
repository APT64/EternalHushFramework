#include <Windows.h>
#include <Python.h>
#include <CtxobjModule.h>

PyObject* init_py4j() {
    PyObject* py4j_module = PyImport_ImportModule("py4j.java_gateway");
    PyObject* py4j_javagateway = PyObject_GetAttrString(py4j_module, "JavaGateway");
    PyObject* py4j_gatewayparams = PyObject_GetAttrString(py4j_module, "GatewayParameters");

    PyObject* empty_args = PyTuple_New(0);
    PyObject* kwargs = Py_BuildValue("{s:i}", "port", 49406);
    PyObject* gw_params = PyObject_Call(py4j_gatewayparams, empty_args, kwargs);

    kwargs = Py_BuildValue("{s:O}", "gateway_parameters", gw_params);
    PyObject* jgateway = PyObject_Call(py4j_javagateway, empty_args, kwargs);

    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    return entry_point;
}

PyObject* echo(PyObject* self, PyObject* args) {
    PyObject* command = PyObject_GetAttrString(init_py4j(), "echo");
    PyObject* result = PyObject_CallObject(command, args);

    Py_RETURN_NONE;
}

PyObject* run(PyObject* self, PyObject* args) {
    PyObject* command = PyObject_GetAttrString(init_py4j(), "run");
    PyObject* result = PyObject_CallObject(command, args);

    Py_RETURN_NONE;
}

PyObject* dialog(PyObject* self, PyObject* args) {
    PyObject* command = PyObject_GetAttrString(init_py4j(), "dialog");
    PyObject* result = PyObject_CallObject(command, args);

    return result;
}

PyObject* option(PyObject* self, PyObject* args) {
    PyObject* command = PyObject_GetAttrString(init_py4j(), "option");
    PyObject* result = PyObject_CallObject(command, args);

    return result;
}

PyObject* print_critical(PyObject* self, PyObject* args) {
    PyObject* command = PyObject_GetAttrString(init_py4j(), "print_critical");
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
