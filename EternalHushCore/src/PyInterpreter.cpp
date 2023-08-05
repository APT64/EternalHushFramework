#pragma once
#include <PyInterpreter.h>
#include <InternalModule.h>
#include <locale>
#include <codecvt>

void ErrCheck(PyObject *object) {
	if (object == nullptr) {
		PyErr_Print();
		Py_XDECREF(object);
	}
}

void PrepareSysModule() {
	WCHAR current_path[MAX_PATH];

	GetCurrentDirectoryW(sizeof(current_path), current_path);

	std::wstring current_path2 = current_path;

	AddSearchPath(current_path2);
	AddSearchPath(current_path2 + L"\\CoreLibs\\Lib\\site-packages");
	AddSearchPath(current_path2 + L"\\modules");
	AddSearchPath(current_path2 + L"\\base\\pylibs");
	AddSearchPath(current_path2 + L"\\base\\pylibs\\internal");
}

void AddSearchPath(std::wstring path) {
	std::string pythonized_path;

	pythonized_path.resize(path.size() + 100);

	int k = 0;
	for (int i = 0; path[i]; i++)
	{
		if (path[i] == '\\') {
			pythonized_path[k++] = '\\';
		}
		pythonized_path[k++] = path[i];
	}

	char line[MAX_PATH * 2];
	sprintf(line, "import sys\nsys.path.append(\"%s\")", pythonized_path.c_str());
	PyRun_SimpleStringFlags(line, NULL);
}


wchar_t* cstowc(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}

void ExecuteScript(char* script_name, int argc, char* argv[]) {
	PrepareSysModule();

	FILE* pScript = fopen(script_name, "r");
	wchar_t** wargv = new wchar_t*[argc];

	for (int i = 0; i < argc; i++)
	{
		wargv[i] = cstowc(argv[i]);
	}
	if (pScript)
	{
		PySys_SetArgv(argc, wargv);
		PyRun_SimpleFile(pScript, script_name);
	}
}