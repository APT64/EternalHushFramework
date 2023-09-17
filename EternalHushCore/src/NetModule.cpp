#include <Windows.h>
#include <Python.h>
#include <TcpClient.h>
#include <vector>
#include <string>

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
	for (int i = 0; i < connection_list.size(); i++)
	{
		if (connection_list.at(i)->id == conn_id) {
			TcpClient(connection_list.at(i)).datarecv(c_buffer, packet_len);
		}
	}
	return PyByteArray_FromStringAndSize((const char*)c_buffer, packet_len);
}