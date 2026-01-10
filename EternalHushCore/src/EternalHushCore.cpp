#include <EternalHushCore.h>
#include <InternalModule.h>
#include <common.h>
#include <Python.h>
#include <Handlers.h>
#include <InternalModule.h>
#include <structs.h>
#include <globalvars.h>
#include <random>

PyThreadState* g_state;

std::wstring SetPythonPath(std::wstring path, PyConfig *c) {

    std::wstring dlls_siffix = L"\\CoreLibs\\DLLs";
    std::wstring lib_siffix = L"\\CoreLibs\\Lib";

    std::wstring dlls_path = path + dlls_siffix;
    std::wstring lib_path = path + lib_siffix;
    PyWideStringList_Append(&c->module_search_paths, dlls_path.c_str());
    PyWideStringList_Append(&c->module_search_paths, lib_path.c_str());


    return (lib_path + L";" + dlls_path);
}

class InitPython {
public:
    InitPython() {
        printf("[CORE] (EternalHushCore.cpp:InitPython) started initialization of python interpreter\n");
        PyImport_AppendInittab("_eternalhush", &PyInit__eternalhush);

        WCHAR current_path[MAX_PATH];
        GetCurrentDirectoryW(sizeof(current_path), current_path);
        std::wstring current_path2 = current_path;

        PyConfig c{};
        PyConfig_InitPythonConfig(&c);
        c.write_bytecode = 0;
        PyConfig_SetString(&c, &c.home, (current_path2 + L"\\CoreLibs").c_str());
       // c.module_search_paths_set = 1;
      //  SetPythonPath(current_path2, &c);

        auto s = Py_InitializeFromConfig(&c);
        if (PyStatus_Exception(s)) {
            Py_ExitStatusException(s);
        }

        PyConfig_Clear(&c);
   //     Py_DECREF(PyImport_ImportModule("threading"));
      
    //    main_gil_state = PyGILState_Ensure();
    //    main_thread_state = PyEval_SaveThread();
         g_state = PyThreadState_Get();
         printf("[CORE] (EternalHushCore.cpp:InitPython) python started from config\n");
    }

    ~InitPython() {
     //   PyEval_RestoreThread(main_thread_state);
     //   PyGILState_Release(main_gil_state);
    }
private:
    PyGILState_STATE main_gil_state;
    PyThreadState* main_thread_state;
};

void init() {
    static InitPython py_init;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved)
{

    switch (fdwReason)
    {

    case DLL_PROCESS_ATTACH:
        init();
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:

        break;
    }
    return TRUE;
}