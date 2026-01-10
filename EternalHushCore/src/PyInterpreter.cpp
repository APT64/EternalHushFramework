#pragma once
#include <PyInterpreter.h>
#include <InternalModule.h>
#include <locale>
#include <codecvt>
#include <vector>
#include <TaskingExceptions.h>
#include <filesystem>
#include <TaskFormat.h>

#define FORCE  0x00000001
#define BACKGROUND  0x00000002
#define NOLOG  0x00000004
#define RESTART  0x00000008

#define TASK_RUNNING 1
#define TASK_INTERNAL_ERROR 4
#define TASK_ABORTED 8
#define TASK_COMPLETED 0

static PyMethodDef stdout_def = { "stdout", &info_write, METH_VARARGS, "info_write" };
static PyMethodDef stderr_def = { "stderr", &err_write, METH_VARARGS, "err_write" };

PyObject* _py4j_gw = 0;

namespace fs = std::filesystem;

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
	AddSearchPath(current_path2 + L"\\base\\pylibs");
	PyRun_SimpleStringFlags("sys.dont_write_bytecode = True", 0);
	printf("[CORE] (PyInterpreter.cpp:PrepareSysModule) 'sys' module prepared\n");
}

std::string PythonizePath(std::string path) {
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
	return pythonized_path;
}

std::string PythonizePath(std::wstring path) {
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
	return pythonized_path;
}

void AddSearchPath(std::wstring path) {

	char line[MAX_PATH * 2];
	sprintf(line, "import sys\nsys.path.append(\"%s\")", PythonizePath(path).c_str());
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
	printf("[CORE] (PyInterpreter.cpp:init_py4j) initializing py4j gateway\n");
	PyObject* py4j_module = PyImport_ImportModule("py4j.java_gateway");
	PyObject* py4j_javagateway = PyObject_GetAttrString(py4j_module, "JavaGateway");
	PyObject* py4j_gatewayclient = PyObject_GetAttrString(py4j_module, "GatewayClient");

	PyObject* empty_args = PyTuple_New(0);
	PyObject* kwargs = Py_BuildValue("{s:i}", "port", 49406);
	PyObject* gw_client = PyObject_Call(py4j_gatewayclient, empty_args, kwargs);

	PyObject* jgateway = PyObject_CallOneArg(py4j_javagateway, gw_client);

	return jgateway;
}

PyObject* CopySysList(PyObject* original_list) {
	if (!PyList_Check(original_list)) {
		PyErr_SetString(PyExc_TypeError, "Expected a list object.");
		return NULL;
	}

	Py_ssize_t size = PyList_Size(original_list);
	PyObject* new_list = PyList_New(size);

	if (new_list == NULL) {
		return NULL; // Memory allocation failed
	}

	for (Py_ssize_t i = 0; i < size; i++) {
		PyObject* item = PyList_GetItem(original_list, i);
		if (item == NULL) {
			Py_DECREF(new_list); // Clean up if an error occurs
			return NULL;
		}
		// PyList_SetItem steals a reference, so Py_INCREF the item before setting
		Py_INCREF(item);
		if (PyList_SetItem(new_list, i, item) < 0) {
			Py_DECREF(new_list); // Clean up
			Py_DECREF(item); // Clean up the item reference
			return NULL;
		}
	}

	return new_list;
}
INT ExecuteScript(char* script_name, int console_id, DWORD flags, std::vector <ARGUMENT> args, char* uid) {
	PrepareSysModule();
	//MessageBoxA(0, "prepared", 0, 0);
	DWORD status = 1;
	PyObject* py4j_gw = init_py4j();
	//MessageBoxA(0, "py4j init", 0, 0);
	FILE* pScript = fopen(script_name, "r");

	if (pScript)
	{
		PyRun_SimpleStringFlags("import warnings\nwarnings.filterwarnings('ignore')", 0);

		char line[MAX_PATH * 2];
		//PyRun_SimpleStringFlags("if len(sys.argv) == 1:\n\tsys.argv.pop(0)", 0);
		sprintf(line, "import _eternalhush\n_eternalhush.ctxObj.console_id = %d\n_eternalhush.ctxObj.task_uid = '%s'", console_id, uid);
		PyRun_SimpleStringFlags(line, NULL);
		PyRun_SimpleStringFlags("_eternalhush.ctxObj.args = []", 0);
		for (int i = 0; i < args.size(); i++)
		{
			if (args.at(i).type == "$INT")
			{
				std::string name = args.at(i).name;
				LONGLONG val;
				memcpy(&val, args.at(i).data.data(), args.at(i).data.size());
				sprintf(line, "_eternalhush.ctxObj.args.append(('%s',0x%llx))", name.c_str(), val);
			}
			else if (args.at(i).type == "$STR") {
				std::string name = args.at(i).name;
				std::string val(args.at(i).data.begin(), args.at(i).data.end());
				sprintf(line, "_eternalhush.ctxObj.args.append(('%s','%s'))",name.c_str() , val.c_str());
			}
			else if (args.at(i).type == "$FLAG") {
				std::string name = args.at(i).name;
				sprintf(line, "_eternalhush.ctxObj.args.append(('%s', True))", name.c_str());
			}
			PyRun_SimpleStringFlags(line, 0);
		}

		//	PySys_SetArgv(argc, wargv);

			//MessageBoxA(0, "const", 0, 0);

		PyRun_SimpleStringFlags("_eternalhush.ctxObj.force_flag = 0", 0);

		if (flags & FORCE)
		{
			PyRun_SimpleStringFlags("_eternalhush.ctxObj.force_flag = 1", 0);
		}

		sprintf(line, "import os\n_eternalhush.ctxObj.workdir = os.path.realpath(os.path.dirname(\"%s\") + \"\\\\..\")", PythonizePath(script_name).c_str());
		//	MessageBoxA(0, "os init", 0, 0);
		PyRun_SimpleStringFlags(line, NULL);
		//MessageBoxA(0, line, 0, 0);
		PyRun_SimpleStringFlags("_eternalhush.ctxObj.files = os.path.join(_eternalhush.ctxObj.workdir, \"files\")", NULL);
		PyRun_SimpleStringFlags("_eternalhush.ctxObj.storage = os.path.join(_eternalhush.ctxObj.workdir, \"storage\")", NULL);

		//MessageBoxA(0, "id", 0, 0);
		PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
		//MessageBoxA(0, "mod", 0, 0);
		PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");
		//MessageBoxA(0, "ctxobj", 0, 0);
		PyObject_SetAttrString(ctxobj, "py4j_gw", py4j_gw);
		//MessageBoxA(0, "py4j_gw", 0, 0);


		PyObject* sys = PyImport_GetModule(PyUnicode_FromString("sys"));
		PyObject* _stdout = PyObject_GetAttrString(sys, "stdout");
		PyObject* stdout_write = PyCFunction_New(&stdout_def, NULL);
		PyObject_SetAttrString(_stdout, "write", stdout_write);


		PyObject* _stderr = PyObject_GetAttrString(sys, "stderr");
		PyObject* stderr_write = PyCFunction_New(&stderr_def, NULL);
		PyObject_SetAttrString(_stderr, "write", stderr_write);

		//PyRun_SimpleFile(pScript, script_name);
		std::string module_directory = (fs::current_path() / script_name).parent_path().u8string();
		std::string module_name = (fs::current_path() / script_name).stem().u8string();

		PyObject* sysPath = PySys_GetObject("path");
		PyList_Append(sysPath, PyUnicode_FromString(module_directory.c_str()));
		printf("[CORE] (PyInterpreter.cpp:ExecuteScript) global vars initialized\n");

		PyObject* py_module = PyImport_ImportModule(module_name.c_str());
		PyObject* py_eternalhush_module = PyImport_ImportModule("eternalhush");
		
		if (!py_module || !py_eternalhush_module)
		{
			PyErr_Print();
			if (py_eternalhush_module) Py_DECREF(py_eternalhush_module);
			if (py_module) Py_DECREF(py_module);
			status = TASK_INTERNAL_ERROR;
			goto End;
		}
		PyObject* parser_class = PyObject_GetAttrString(PyObject_GetAttrString(py_eternalhush_module, "data"), "ArgParser");
		PyObject* parser_object = PyObject_CallNoArgs(parser_class);
		//Py_DECREF(parser_class);
		//Py_DECREF(py_eternalhush_module);

		PyObject* do_parse = PyObject_GetAttrString(parser_object, "parse");
		PyObject* parsed_args = PyObject_CallNoArgs(do_parse);

		printf("[CORE] (PyInterpreter.cpp:ExecuteScript) internal module _eternalhush loaded\n");
	
		//Py_DECREF(do_parse);
		//Py_DECREF(parser_object);
		PyObject* script_entrypoint = PyObject_GetAttrString(py_module, "main");
		if (!script_entrypoint)
		{
			PyErr_SetString(PyExc_NameError, "entrypoint 'main' not found in script");
			PyErr_Print();
			status = TASK_INTERNAL_ERROR;
			goto End;
		}
		PyObject* script_return_value = 0;
		try {
			printf("[CORE] (PyInterpreter.cpp:ExecuteScript) calling script main\n");
			script_return_value = PyObject_CallOneArg(script_entrypoint, parsed_args);
			printf("[CORE] (PyInterpreter.cpp:ExecuteScript) script finished\n");
		}
		catch(std::exception e){
			std::cout << e.what();
		}
		status = TASK_COMPLETED;
		if (PyErr_Occurred()) {
			if (PyErr_ExceptionMatches(ExceptionTaskAborted)) {
				status = TASK_ABORTED;
				PyErr_SetString(ExceptionTaskAborted, "current task aborted");
				PyErr_Print();
			}
			else{
				status = TASK_INTERNAL_ERROR;
				PyErr_Print();
			}
		}
		//HERE HANDLING RETN VALUE
		if (script_return_value)
		{
			Py_DECREF(script_return_value);
		}
		
		printf("[CORE] (PyInterpreter.cpp:ExecuteScript) closing gateway connection\n");
		PyObject* gw_close = PyObject_GetAttrString(py4j_gw, "close");
		Py_XDECREF(PyObject_CallNoArgs(gw_close));
		Py_XDECREF(gw_close);
	}
End:
	return status;
}


void ExecuteScript3(char* script_name, int console_id, int argc, char* argv[]) {
	PrepareSysModule();
	//PyRun_SimpleString("import _xxsubinterpreters as interp");
	auto py4j_gw = init_py4j();

	FILE* pScript = fopen(script_name, "r");
	wchar_t** wargv = new wchar_t*[argc];

	for (int i = 0; i < argc; i++)
	{
		wargv[i] = cstowc(argv[i]);
	}
	if (pScript)
	{
		char line[MAX_PATH * 2];
	//	PyRun_SimpleStringFlags("sys.argv.pop(0)", 0);

		for (int i = 0; i < argc; i++)
		{
			sprintf(line, "sys.argv.append('%s')", argv[i]);
			PyRun_SimpleStringFlags(line, 0);
		}

	//	PySys_SetArgv(argc, wargv);

		sprintf(line, "import _eternalhush\n_eternalhush.ctxObj.console_id = %d", console_id);
		PyRun_SimpleStringFlags(line, NULL);	

		sprintf(line, "import os\n_eternalhush.ctxObj.workdir = os.path.realpath(os.path.dirname(\"%s\") + \"\\\\..\")", PythonizePath(script_name).c_str());

		PyRun_SimpleStringFlags(line, NULL);
		PyRun_SimpleStringFlags("_eternalhush.ctxObj.files = os.path.join(_eternalhush.ctxObj.workdir, \"files\")", NULL);
		PyRun_SimpleStringFlags("_eternalhush.ctxObj.storage = os.path.join(_eternalhush.ctxObj.workdir, \"storage\")", NULL);

		PyObject* yggd = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));

		PyObject* ctxobj = PyObject_GetAttrString(yggd, "ctxObj");

		PyObject_SetAttrString(ctxobj, "py4j_gw", py4j_gw);


		PyObject* sys = PyImport_GetModule(PyUnicode_FromString("sys"));

		PyObject* _stdout = PyObject_GetAttrString(sys, "stdout");

		PyObject* stdout_write = PyCFunction_New(&stdout_def, NULL);

		PyObject_SetAttrString(_stdout, "write", stdout_write);
		MessageBoxA(0, "prerun", 0, 0);
		
		PyRun_SimpleFile(pScript, script_name);
		MessageBoxA(0, "run", 0, 0);
		Py_XDECREF(PyObject_CallNoArgs(PyObject_GetAttrString(py4j_gw, "close")));
	}
}