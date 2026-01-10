#pragma once
#include <iostream>
#include <Python.h>
#include <TaskFormat.h>
#include <Windows.h>
#include <InternalModule.h>
#include <vector>

void SetPythonPath(std::wstring path);
INT ExecuteScript(char* script_name, int console_id, DWORD flags, std::vector<ARGUMENT> args, char* uid);
void AddSearchPath(std::wstring path);
void ErrCheck(PyObject *object);
