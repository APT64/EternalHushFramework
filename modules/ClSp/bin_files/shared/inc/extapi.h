#include <Windows.h>
#include <vector>

typedef struct {
	LONGLONG api_uid;
	LONGLONG api_ptr;
} EXTENSION_APIENTRY;

typedef struct {
	LONGLONG ext_uid;
	LONGLONG ext_base;
	PMODULE_CONTEXT pCtx;
	std::vector<EXTENSION_APIENTRY> api_table;
} EXTENSION_OBJECT, *PEXTENSION_OBJECT;