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

PyObject* PyInit_ctxobj() {
    PyObject* ctxObj_module = PyModule_Create(&ctxobj_moduleDef);

    PyObject* ctxObj_dict = PyModule_GetDict(ctxObj_module);
    PyDict_SetItemString(ctxObj_dict, "ECHO_DEFAULT", Py_BuildValue("i", 0));
    PyDict_SetItemString(ctxObj_dict, "ECHO_GOOD", Py_BuildValue("i", 1));
    PyDict_SetItemString(ctxObj_dict, "ECHO_ERROR", Py_BuildValue("i", 2));
    PyDict_SetItemString(ctxObj_dict, "ECHO_WARNING", Py_BuildValue("i", 3));
    PyDict_SetItemString(ctxObj_dict, "OPTION_YES", Py_BuildValue("i", 1));
    PyDict_SetItemString(ctxObj_dict, "OPTION_NO", Py_BuildValue("i", 0));
    PyDict_SetItemString(ctxObj_dict, "OPTION_CANCEL", Py_BuildValue("i", -1));
    PyDict_SetItemString(ctxObj_dict, "RUN_SILENT", Py_BuildValue("i", 1));
    PyDict_SetItemString(ctxObj_dict, "RUN_NORMAL", Py_BuildValue("i", 0));
    return ctxObj_module;
}
