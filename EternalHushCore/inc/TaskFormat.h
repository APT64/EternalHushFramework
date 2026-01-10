#pragma once
#include <Windows.h>
#include <Python.h>
#include <iostream>
#include <vector>

struct ARGUMENT {
	std::string name;
	std::vector<char> data;
	std::string type;
};

typedef struct TASK_INFO_BLOCK {
	DWORD console_id;
	DWORD flags;
	std::string name;
	std::vector<ARGUMENT> arglist;
	int task_result;
} *PTASK_INFO_BLOCK;

typedef struct THREAD_INFO_BLOCK {
	DWORD tid;
	DWORD cid;
	char* uid;
	PyThreadState* tstate;
	PyThreadState* prev_tstate;
	PTASK_INFO_BLOCK ptask_info;
};
