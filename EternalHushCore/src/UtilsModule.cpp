#include <Windows.h>
#include <UtilsModule.h>
#include <vector>
#include <fstream>
#include <string>

typedef short CSHORT;
typedef struct _TIME_FIELDS {
	CSHORT Year;
	CSHORT Month;
	CSHORT Day;
	CSHORT Hour;
	CSHORT Minute;
	CSHORT Second;
	CSHORT Milliseconds;
	CSHORT Weekday;
} TIME_FIELDS;
typedef VOID (*pRtlTimeToTimeFields)(
  PLARGE_INTEGER Time,
  TIME_FIELDS*   TimeFields
);

PyObject* util_systemtime_to_datetime(PyObject* self, PyObject* args) {
	unsigned long long systime;
	TIME_FIELDS tf = {0};
	PyArg_ParseTuple(args, "K", &systime);
	LPVOID ntdll = (LPVOID)GetModuleHandleA("ntdll.dll");
	pRtlTimeToTimeFields RtlTimeToTimeFields = (pRtlTimeToTimeFields)GetProcAddress((HMODULE)ntdll, "RtlTimeToTimeFields");
	RtlTimeToTimeFields((PLARGE_INTEGER)&systime, &tf);
	
	PyObject* list = PyList_New(NULL);
	Py_XINCREF(list);

	PyList_Append(list, PyLong_FromLong((LONG)tf.Year));
	PyList_Append(list, PyLong_FromLong((LONG)tf.Month));
	PyList_Append(list, PyLong_FromLong((LONG)tf.Day));
	PyList_Append(list, PyLong_FromLong((LONG)tf.Hour));
	PyList_Append(list, PyLong_FromLong((LONG)tf.Minute));
	PyList_Append(list, PyLong_FromLong((LONG)tf.Second));
	PyList_Append(list, PyLong_FromLong((LONG)tf.Milliseconds));
	PyList_Append(list, PyLong_FromLong((LONG)tf.Weekday));

	return list;
}