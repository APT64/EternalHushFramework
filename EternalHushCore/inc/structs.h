#include <Windows.h>

typedef struct _MODULE_CONTEXT {
	char** option;
	int option_count;
} MODULE_CONTEXT, * PMODULE_CONTEXT;