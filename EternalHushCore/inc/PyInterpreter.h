#pragma once
#include <iostream>
#include <Python.h>
#include <Windows.h>
#include <InternalModule.h>

void SetPythonPath(std::wstring path);
void ExecuteScript(char* script_name, int console_id, int argc, char* argv[]);
void AddSearchPath(std::wstring path);
void ErrCheck(PyObject *object);
