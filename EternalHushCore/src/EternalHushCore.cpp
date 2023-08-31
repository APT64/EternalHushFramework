#include <EternalHushCore.h>
#include <InternalModule.h>
#include <common.h>
#include <Python.h>
#include <Handlers.h>
#include <InternalModule.h>
#include <structs.h>
#include <globalvars.h>
#include <random>

void SetPythonPath(std::wstring path) {

    std::wstring dlls_siffix = L"\\CoreLibs\\DLLs";
    std::wstring lib_siffix = L"\\CoreLibs\\Lib";

    std::wstring dlls_path = path + dlls_siffix;
    std::wstring lib_path = path + lib_siffix;

    Py_SetPath((lib_path + L";" + dlls_path).c_str());
}

class InitPython {
public:
    InitPython() {
        PyImport_AppendInittab("_eternalhush", &PyInit_eternalhush);

        WCHAR current_path[MAX_PATH];
        GetCurrentDirectoryW(sizeof(current_path), current_path);
        std::wstring current_path2 = current_path;
        SetPythonPath(current_path2);

        Py_Initialize();
        Py_DECREF(PyImport_ImportModule("threading"));
        main_gil_state = PyGILState_Ensure();
        main_thread_state = PyEval_SaveThread();
    }

    ~InitPython() {
        PyEval_RestoreThread(main_thread_state);
        Py_Finalize();
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

        if (lpvReserved != nullptr)
        {
            break;
        }

        break;
    }
    return TRUE;
}