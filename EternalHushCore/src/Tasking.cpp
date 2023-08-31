#include <Windows.h>
#include <common.h>
#include <PyInterpreter.h>
#include <thread>
#include <PyThread.h>

EXPORT VOID RunScript(char* name, int console_id, int argc, char* argv[]) {
	auto main_gil_state = PyGILState_Ensure();
	auto old_thread_state = PyThreadState_Get();
	auto new_thread_state = Py_NewInterpreter();
	PyThreadState_Swap(new_thread_state);

	auto sub_thread_state = PyEval_SaveThread();
	auto sub_gil_state = PyGILState_Ensure();

	ExecuteScript(name, console_id, argc, argv);

	PyGILState_Release(sub_gil_state);
	PyEval_RestoreThread(sub_thread_state);
	Py_EndInterpreter(new_thread_state);
	PyThreadState_Swap(old_thread_state);
	PyGILState_Release(main_gil_state);
}
