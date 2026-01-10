#include <Windows.h>
#include <Python.h>
#include <CtxobjModule.h>
PyObject* ExceptionTaskAborted;

 PyObject* echo(PyObject* self, PyObject* args) {
    PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
    PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    PyObject* command = PyObject_GetAttrString(entry_point, "echo");
    PyObject* result = PyObject_CallObject(command, args);
    if (PyErr_Occurred())
    {
        PyErr_Print();
    }
    Py_RETURN_NONE;
}

 PyObject* run(PyObject* self, PyObject* args) {
    PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
    PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    PyObject* command = PyObject_GetAttrString(entry_point, "run");
    PyObject* result = PyObject_CallObject(command, args);

    if (PyErr_Occurred())
    {
        PyErr_Print();
    }
    Py_RETURN_NONE;
}

 PyObject* dialog(PyObject* self, PyObject* args) {
    PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
    PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    PyObject* command = PyObject_GetAttrString(entry_point, "dialog");
    PyObject* result = PyObject_CallObject(command, args);
    if (PyErr_Occurred())
    {
        PyErr_Print();
    }
    return result;
}

 PyObject* option(PyObject* self, PyObject* args) {
    PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
    PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    PyObject* command = PyObject_GetAttrString(entry_point, "option");
    PyObject* result = PyObject_CallObject(command, args);
    if (PyErr_Occurred())
    {
        PyErr_Print();
    }
    return result;
}

 PyObject* print_critical(PyObject* self, PyObject* args) {
    PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
    PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    PyObject* command = PyObject_GetAttrString(entry_point, "print_critical");
    PyObject* result = PyObject_CallObject(command, args);
    if (PyErr_Occurred())
    {
        PyErr_Print();
    }
    Py_RETURN_NONE;
}

 PyObject* print_info(PyObject* self, PyObject* args) {
    PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
    PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    PyObject* command = PyObject_GetAttrString(entry_point, "print_info");
    PyObject* result = PyObject_CallObject(command, args);
    if (PyErr_Occurred())
    {
        PyErr_Print();
    }
    Py_RETURN_NONE;
}

 PyObject* set_hostname(PyObject* self, PyObject* args) {
    PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
    PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    PyObject* command = PyObject_GetAttrString(entry_point, "update_hostname");
    PyObject* result = PyObject_CallObject(command, args);
    if (PyErr_Occurred())
    {
        PyErr_Print();
    }
    Py_RETURN_NONE;
}

 PyObject* set_env(PyObject* self, PyObject* args) {
    PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
    PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    PyObject* command = PyObject_GetAttrString(entry_point, "set_env");
    PyObject* result = PyObject_CallObject(command, args);
    if (PyErr_Occurred())
    {
        PyErr_Print();
    }
    Py_RETURN_NONE;
}

 PyObject* get_env(PyObject* self, PyObject* args) {
    PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
    PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    PyObject* command = PyObject_GetAttrString(entry_point, "get_env");
    PyObject* result = PyObject_CallObject(command, args);
    if (PyErr_Occurred())
    {
        PyErr_Print();
    }
    return result;
}
 PyObject* set_global_env(PyObject* self, PyObject* args) {
     PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
     PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
     PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
     PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
     PyObject* command = PyObject_GetAttrString(entry_point, "set_global_env");
     PyObject* result = PyObject_CallObject(command, args);
     if (PyErr_Occurred())
     {
         PyErr_Print();
     }
     Py_RETURN_NONE;
 }

 PyObject* get_global_env(PyObject* self, PyObject* args) {
     PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
     PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
     PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
     PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
     PyObject* command = PyObject_GetAttrString(entry_point, "get_global_env");
     PyObject* result = PyObject_CallObject(command, args);
     if (PyErr_Occurred())
     {
         PyErr_Print();
     }
     return result;
 }
 PyObject* lock_session(PyObject* self, PyObject* args) {
    PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
    PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    PyObject* command = PyObject_GetAttrString(entry_point, "lock_session");
    PyObject* result = PyObject_CallObject(command, args);
    if (PyErr_Occurred())
    {
        PyErr_Print();
    }
    Py_RETURN_NONE;
}
 PyObject* unlock_session(PyObject* self, PyObject* args) {
     PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
     PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
     PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
     PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
     PyObject* command = PyObject_GetAttrString(entry_point, "unlock_session");
     PyObject* result = PyObject_CallObject(command, args);
     if (PyErr_Occurred())
     {
         PyErr_Print();
     }
     Py_RETURN_NONE;
 }

 PyObject* get_first_available(PyObject* self, PyObject* args) {
    PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
    PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
    PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
    PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
    PyObject* command = PyObject_GetAttrString(entry_point, "get_first_available");
    PyObject* result = PyObject_CallNoArgs(command);
    if (PyErr_Occurred())
    {
        PyErr_Print();
    }
    return result;
}

 PyObject* suppress_msg_type(PyObject* self, PyObject* args) {
     PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
     PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
     PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
     PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
     PyObject* command = PyObject_GetAttrString(entry_point, "suppress_msg_type");
     PyObject* result = PyObject_CallObject(command, args);
     if (PyErr_Occurred())
     {
         PyErr_Print();
     }
     return result;
 }
 PyObject* disable_suppress(PyObject* self, PyObject* args) {
     PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
     PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
     PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
     PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
     PyObject* command = PyObject_GetAttrString(entry_point, "disable_suppress");
     PyObject* result = PyObject_CallObject(command, args);
     if (PyErr_Occurred())
     {
         PyErr_Print();
     }
     return result;
 }

 PyObject* get_locked_module(PyObject* self, PyObject* args) {
     PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
     PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
     PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
     PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
     PyObject* command = PyObject_GetAttrString(entry_point, "get_lock_name");
     PyObject* result = PyObject_CallObject(command, args);
     if (PyErr_Occurred())
     {
         PyErr_Print();
     }
     return result;
 }

 PyObject* add_crypto_key(PyObject* self, PyObject* args) {
     PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
     PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
     PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
     PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
     PyObject* command = PyObject_GetAttrString(entry_point, "add_crypto_key");
     PyObject* result = PyObject_CallObject(command, args);
     if (PyErr_Occurred())
     {
         PyErr_Print();
     }
     return result;
 }
 PyObject* query_key_storage(PyObject* self, PyObject* args) {
     PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
     PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
     PyObject* jgateway = PyObject_GetAttrString(ctxobj, "py4j_gw");
     PyObject* entry_point = PyObject_GetAttrString(jgateway, "entry_point");
     PyObject* command = PyObject_GetAttrString(entry_point, "query_key_storage");
     PyObject* result = PyObject_CallObject(command, args);
     if (PyErr_Occurred())
     {
         PyErr_Print();
     }
     return result;
 }

 PyObject* PyInit_ctxobj() {
    PyObject* ctxObj_module = PyModule_Create(&ctxobj_moduleDef);

    PyModule_AddIntConstant(ctxObj_module, "ECHO_DEFAULT", 0);
    PyModule_AddIntConstant(ctxObj_module, "ECHO_GOOD", 1);
    PyModule_AddIntConstant(ctxObj_module, "ECHO_ERROR", 2);
    PyModule_AddIntConstant(ctxObj_module, "ECHO_WARNING", 3);
    PyModule_AddIntConstant(ctxObj_module, "ECHO_NOENDL", 4);
    
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

    PyModule_AddStringConstant(ctxObj_module, "KEYTYPE_KEYPAIR", "KEYPAIR");

    ExceptionTaskAborted = PyErr_NewExceptionWithDoc("ctxObj.ExceptionTaskAborted", "task aborted with error", NULL, NULL);
    Py_XINCREF(ExceptionTaskAborted);
    PyModule_AddObject(ctxObj_module, "ExceptionTaskAborted", ExceptionTaskAborted);

    return ctxObj_module;
}
