#pragma once
#include <Python.h>

PyObject* PyInit_eternalhush();

static PyMethodDef methodsDef[] = {
    { nullptr, nullptr, 0, nullptr }
};

static PyModuleDef moduleDef = {
    PyModuleDef_HEAD_INIT,
    "_eternalhush",
    "EternalHush's internal module",
    -1,
    methodsDef
};
