#include <Windows.h>
#include <Python.h>
#include <TcpClient.h>
#include <vector>
#include <string>

#define BREAK_EXECUTION 0xFFFFFFFF

std::vector<Connection*> connection_list;

PyObject* create_new_connection(PyObject* self, PyObject* args) {
	int conn_port, conn_type;
	unsigned char* conn_addr;
	PyArg_ParseTuple(args, "sii", &conn_addr, &conn_port, &conn_type);

	if (conn_type == TCP_CONNECTION)
	{
		Connection* tcp_connection = new Connection;
		TcpClient client(tcp_connection);
		if (client.setup(conn_port, conn_addr))
		{
			tcp_connection->id = connection_list.size();
			connection_list.push_back(tcp_connection);
			return Py_BuildValue("i", tcp_connection->id);
		}
	}
	Py_RETURN_NONE;
}

PyObject* add_exception_handler(PyObject* self, PyObject* args) {
	unsigned char* mod_name, *func_name;
	int conn_id;
	PyArg_ParseTuple(args, "iss", &conn_id , &mod_name, &func_name);

	for (int i = 0; i < connection_list.size(); i++)
	{
		if (connection_list.at(i)->id == conn_id) {
			PyObject *handler_obj = PyImport_ImportModule((const char*)mod_name);
			handler h;
			h.main_name = (char*)func_name;
			h.module_obj = handler_obj;
			connection_list.at(i)->handler_list.push_back(h);
		}
	}

	Py_RETURN_NONE;
}

PyObject* tcp_send(PyObject* self, PyObject* args) {
	int conn_id, send_bytes;
	Py_buffer buffer;
	PyArg_ParseTuple(args, "is*", &conn_id, &buffer);

	for (int i = 0; i < connection_list.size(); i++)
	{
		if (connection_list.at(i)->id == conn_id) {
			send_bytes = TcpClient(connection_list.at(i)).datasend(buffer.buf, buffer.len);
		}
	}
	return PyLong_FromLong(send_bytes);
}

PyObject* tcp_recv(PyObject* self, PyObject* args) {
	int conn_id, packet_len;
	unsigned char* c_buffer;
	PyArg_ParseTuple(args, "ii", &conn_id, &packet_len);
	c_buffer = (unsigned char*)VirtualAlloc(0, packet_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	PyObject* retn = 0;

	for (int i = 0; i < connection_list.size(); i++)
	{
		if (connection_list.at(i)->id == conn_id) {
			auto current_conn = connection_list.at(i);
			TcpClient(current_conn).datarecv(c_buffer, packet_len);
			retn = PyByteArray_FromStringAndSize((const char*)c_buffer, packet_len);

			for (int j = 0; j < current_conn->handler_list.size(); j++)
			{
				PyObject* handler_func = PyObject_GetAttrString(current_conn->handler_list.at(j).module_obj, current_conn->handler_list.at(j).main_name);
				if (!PyCallable_Check(handler_func))
				{
					PyErr_SetString(PyExc_RuntimeError, "Handler function is not callable");
					PyErr_Print();
					Py_RETURN_NONE;
				}
				PyObject* return_data = PyObject_CallFunctionObjArgs(handler_func, retn);
				int islong = PyLong_Check(return_data);
				if (islong && PyLong_AsLong(return_data) == BREAK_EXECUTION)
				{
					MessageBoxA(0, 0, 0, 0);
					Py_RETURN_NONE;
				}
				
			}
		}
	}

	if (!retn)
	{
		Py_RETURN_NONE;
	}
	return retn;
}