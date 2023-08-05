#include <Windows.h>
#include <iostream>
#include <vector>

typedef struct _TASK_OBJECT {
	std::string name;
	std::vector<std::string> args;

} TASK_OBJECT, *PTASK_OBJECT;