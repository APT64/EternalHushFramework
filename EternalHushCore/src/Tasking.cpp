#include <Windows.h>
#include <common.h>
#include <PyInterpreter.h>
#include <thread>
#include <PyThread.h>
#include <TaskingExceptions.h>
#include <vector>
#include <TaskFormat.h>

extern PyThreadState* g_state;
std::vector<THREAD_INFO_BLOCK> active_threads;

std::vector<ARGUMENT> unpack_args(char* data, int len) {
	int name_len = 0, data_len = 0, offset = 0;
	char type[5];
	char* pname, * pdata = 0;
	std::vector<ARGUMENT> args_vec;
	while (offset != len)
	{
		memcpy(&name_len, data + offset, sizeof(name_len));
		name_len = _byteswap_ulong(name_len);

		pname = new char[name_len];
		memcpy(pname, data + offset + sizeof(name_len), name_len);
		memcpy(type, data + offset + sizeof(name_len) + name_len, 4);
		type[4] = '\x0';
		if (type == std::string("$INT"))
		{
			data_len = 8;
			pdata = new char[8];
			memcpy(pdata, data + offset + sizeof(name_len) + name_len + 4, data_len);
			*(PULONGLONG)pdata = _byteswap_uint64(*(PULONGLONG)pdata);
			offset = offset + sizeof(name_len) + name_len + 4 + data_len;
		}
		else if (type == std::string("$STR")) {
			memcpy(&data_len, data + offset + sizeof(name_len) + name_len + 4, 4);
			data_len = _byteswap_ulong(data_len);
			pdata = new char[data_len];
			memcpy(pdata, data + offset + sizeof(name_len) + name_len + 4 + 4, data_len);
			offset = offset + sizeof(name_len) + name_len + 4 + 4 + data_len;
		}
		ARGUMENT a;
		a.data = std::vector<char>(pdata, pdata + data_len);
		a.name = pname;
		a.type = type;
		args_vec.push_back(a);
	}
	printf("[CORE] (PyInterpreter.cpp:unpack_args) arguments unpacked\n");
	return args_vec;
}


void CleanupThread(THREAD_INFO_BLOCK* block) {
	Py_EndInterpreter(block->tstate);
	PyThreadState_Swap(block->prev_tstate);

	if (block->ptask_info){
		LocalFree(block->ptask_info);
		block->ptask_info = NULL;
	}
	printf("[CORE] (Tasking.cpp:CleanupThread) thread %s cleaned up\n", block->uid);
}

void UnlinkActiveThreadBlock(THREAD_INFO_BLOCK* block) {
	for (int i = 0; i < active_threads.size(); i++)
	{
		if (strcmp(active_threads.at(i).uid, block->uid) == NULL) {
			active_threads.erase(active_threads.begin()+i);
			printf("[CORE] (Tasking.cpp:UnlinkActiveThreadBlock) thread %s unlinked\n", block->uid);
			break;
		}
	}
}

EXPORT int RunScript(char* name, int console_id, DWORD flags, int arglen, char* argv, char* uid) {
	//auto new_thread = PyThreadState_New(g_state->interp); //creating sub-main thread
//	MessageBoxA(0, "start", 0, 0);

	auto prev = PyThreadState_Swap(0); //swap
	printf("[CORE] (Tasking.cpp:RunScript) thread state saved\n");
//	MessageBoxA(0, "save", 0, 0);
	PyInterpreterConfig conf = { 0 };
	conf.check_multi_interp_extensions = 1;
	conf.allow_threads = 1;
	conf.allow_daemon_threads = 1;
	conf.gil = PyInterpreterConfig_OWN_GIL;
	DWORD tid = GetThreadId(GetCurrentThread());
	PyThreadState* tstate;
	Py_NewInterpreterFromConfig(&tstate, &conf); // creating new interpreter in new thread

	PTASK_INFO_BLOCK task_info = (PTASK_INFO_BLOCK)LocalAlloc(LPTR, sizeof(TASK_INFO_BLOCK));
	std::vector<ARGUMENT> args;
	if (arglen != 0) args = unpack_args((char*)argv, arglen);

	task_info->arglist = args;
	task_info->console_id = console_id;
	task_info->flags = flags;
	task_info->name = name;


	THREAD_INFO_BLOCK t_info;
	t_info.cid = console_id;
	t_info.tid = tid;
	t_info.uid = uid;
	t_info.tstate = tstate;
	t_info.prev_tstate = prev;
	t_info.ptask_info = task_info;
	active_threads.push_back(t_info);

	int result = ExecuteScript(name, console_id, flags, args, uid); //PyRun_String-like func
	t_info.ptask_info->task_result = result;
	CleanupThread(&t_info);
	UnlinkActiveThreadBlock(&t_info);
	return result;
}

EXPORT VOID TerminateRunningScript(char *uid) {
	int thread_id = 0;
	for (int i = 0; i < active_threads.size(); i++){
		if (strcmp(uid, active_threads.at(i).uid) == NULL) {
			printf("[CORE] (Tasking.cpp:TerminateRunningScript) thread %s force-terminated\n", uid);
			thread_id = active_threads.at(i).tstate->thread_id;
			auto previous = PyThreadState_Swap(active_threads.at(i).tstate);
			INT exc_state = PyThreadState_SetAsyncExc(thread_id, ExceptionTaskAborted);
			if (exc_state > 1)
			{
				printf("Failed to raise exception in thread\n");
			}
			PyThreadState_Swap(previous);
			return;
		}
	}
}

EXPORT DWORD GetTaskFlags(char* uid) {
	for (int i = 0; i < active_threads.size(); i++) {
		if (strcmp(uid, active_threads.at(i).uid) == NULL) {
			return active_threads.at(i).ptask_info->flags;
		}
	}
	return 0;
}

EXPORT const char* GetTaskScriptName(char* uid) {
	for (int i = 0; i < active_threads.size(); i++) {
		if (strcmp(uid, active_threads.at(i).uid) == NULL) {
			printf("[CORE] (Tasking.cpp:GetTaskScriptName) found task\n", uid);

			return active_threads.at(i).ptask_info->name.c_str();
		}
	}
	return "";
}

EXPORT DWORD GetTaskCid(char* uid) {
	for (int i = 0; i < active_threads.size(); i++) {
		if (strcmp(uid, active_threads.at(i).uid) == NULL) {
			printf("[CORE] (Tasking.cpp:GetTaskCid) found task\n", uid);

			return active_threads.at(i).ptask_info->console_id;
		}
	}
	return -2;
}