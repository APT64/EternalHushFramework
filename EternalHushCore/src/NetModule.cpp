#include <Windows.h>
#include <Python.h>
#include <TcpClient.h>
#include <vector>
#include <string>

#define TCP_CONNECTION 2
#define HTTP_CONNECTION 4

std::vector<TcpClient> connection_list;

extern PyObject* py4j_ep;

PyObject* create_new_connection(PyObject* self, PyObject* args) {
	int conn_port, conn_type;
	unsigned char* conn_addr;
	PyArg_ParseTuple(args, "sii", &conn_addr, &conn_port, &conn_type);

	if (conn_type == TCP_CONNECTION)
	{
		static TcpClient client;
		client.setup(conn_port, conn_addr);
		client.id = connection_list.size();
		connection_list.push_back(client);
		return Py_BuildValue("i", client.id);
	}
	Py_RETURN_NONE;
}

PyObject* tcp_send(PyObject* self, PyObject* args) {
	int conn_id;
	Py_buffer buffer;
	PyArg_ParseTuple(args, "is*", &conn_id, &buffer);

	for (int i = 0; i < connection_list.size(); i++)
	{
		if (connection_list.at(i).id == conn_id) {
			TcpClient client = connection_list.at(i);
			client.datasend(buffer.buf, buffer.len);
		}
	}
	Py_RETURN_NONE;
}

PyObject* tcp_recv(PyObject* self, PyObject* args) {
	int conn_id, packet_len;
	unsigned char* c_buffer;
	PyArg_ParseTuple(args, "ii", &conn_id, &packet_len);
	c_buffer = (unsigned char*)VirtualAlloc(0, packet_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	for (int i = 0; i < connection_list.size(); i++)
	{
		if (connection_list.at(i).id == conn_id) {
			TcpClient client = connection_list.at(i);
			client.datarecv(c_buffer, packet_len);
		}
	}
	return PyByteArray_FromStringAndSize((const char*)c_buffer, packet_len);
}

PyObject* export_global_connection(PyObject* self, PyObject* args) {
	/*int conn_id;
	PyArg_ParseTuple(args, "i", &conn_id);
	for (int i = 0; i < connection_list.size(); i++)
	{
		if (connection_list.at(i).id == conn_id) {
			TcpClient client = connection_list.at(i);

			PyObject* ethu = PyImport_GetModule(PyUnicode_FromString("_eternalhush"));
			if (!ethu)
			{
				Py_FatalError("Built-in modules not initialized");
			}
			PyObject* ctxObj = PyObject_GetAttrString(ethu, "ctxObj");
			PyObject* console_id = PyObject_GetAttrString(ctxObj, "console_id");

			PyObject* command = PyObject_GetAttrString(init_py4j(), "update_connection");
			PyObject* result = PyObject_CallObject(command, Py_BuildValue("Os", console_id, client.target_addr));

		}
	}*/
	Py_RETURN_NONE;
}