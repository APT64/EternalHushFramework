#pragma once
#include <PyInterpreter.h>
#include <InternalModule.h>
#include <locale>
#include <codecvt>

PyObject* py4j_gw = 0;

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

PyObject* init_py4j() {
	PyObject* py4j_module = PyImport_ImportModule("py4j.java_gateway");
	PyObject* py4j_javagateway = PyObject_GetAttrString(py4j_module, "JavaGateway");
	PyObject* py4j_gatewayclient = PyObject_GetAttrString(py4j_module, "GatewayClient");

	PyObject* empty_args = PyTuple_New(0);
	PyObject* kwargs = Py_BuildValue("{s:i}", "port", 49406);
	PyObject* gw_client = PyObject_Call(py4j_gatewayclient, empty_args, kwargs);

	PyObject* jgateway = PyObject_CallOneArg(py4j_javagateway, gw_client);

	return jgateway;
}

void ExecuteScript(char* script_name, int console_id, int argc, char* argv[]) {
	PrepareSysModule();

	if (!py4j_gw)
	{
		py4j_gw = init_py4j();
	}

	FILE* pScript = fopen(script_name, "r");
	wchar_t** wargv = new wchar_t*[argc];

	for (int i = 0; i < argc; i++)
	{
		wargv[i] = cstowc(argv[i]);
	}
	if (pScript)
	{
		PySys_SetArgv(argc, wargv);
		char line[MAX_PATH * 2];
		sprintf(line, "import _eternalhush\n_eternalhush.ctxObj.console_id = %d", console_id);
		PyRun_SimpleStringFlags(line, NULL);		

		PyObject* ethu = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
		PyObject* ctxobj = PyObject_GetAttrString(ethu, "ctxObj");
		PyObject_SetAttrString(ctxobj, "py4j_gw", py4j_gw);

		PyRun_SimpleFile(pScript, script_name);
		
		PyObject_CallNoArgs(PyObject_GetAttrString(py4j_gw, "close"));
	}
}