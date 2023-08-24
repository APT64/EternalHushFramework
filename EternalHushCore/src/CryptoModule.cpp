#include <CryptoModule.h>
#include <vector>
#include <fstream>
#include <string>
#pragma comment (lib, "bcrypt.lib")
#define NT_SUCCESS(Status)          (((NTSTATUS)(Status)) >= 0)

std::vector<BCRYPT_KEY_HANDLE> key_list;

NTSTATUS decrypt_data(BCRYPT_KEY_HANDLE hKey, PUCHAR encrypted_data, ULONG encrypted_size, PUCHAR& data, ULONG& size) {
	NTSTATUS status;

	status = BCryptDecrypt(hKey, encrypted_data, encrypted_size, NULL, NULL, NULL, NULL, NULL, &size, BCRYPT_PAD_PKCS1);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	data = (PUCHAR)malloc(size);
	status = BCryptDecrypt(hKey, encrypted_data, encrypted_size, NULL, NULL, NULL, data, size, &size, BCRYPT_PAD_PKCS1);
	if (!NT_SUCCESS(status))
	{
		return status;
	}
	return status;
}

NTSTATUS import_privkey(BCRYPT_ALG_HANDLE hProvider, PBYTE lpData, ULONG keySize, BCRYPT_KEY_HANDLE* hKey)
{
	NTSTATUS status;

	status = BCryptImportKeyPair(hProvider, NULL, BCRYPT_RSAFULLPRIVATE_BLOB, hKey, lpData, keySize, 0);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	return status;
}

NTSTATUS import_pubkey(BCRYPT_ALG_HANDLE hProvider, PBYTE lpData, ULONG keySize, BCRYPT_KEY_HANDLE* hKey)
{
	NTSTATUS status;


	status = BCryptImportKeyPair(hProvider, NULL, BCRYPT_RSAPUBLIC_BLOB, hKey, lpData, keySize, 0);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	return status;
}

NTSTATUS encrypt_data(BCRYPT_KEY_HANDLE hKey, PUCHAR data, ULONG size, PUCHAR& encrypted_data, ULONG& encrypted_size) {
	NTSTATUS status;
	BCRYPT_ALG_HANDLE hRsaAlg;

	status = BCryptEncrypt(hKey, data, size, NULL, NULL, NULL, NULL, NULL, &encrypted_size, BCRYPT_PAD_PKCS1);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	encrypted_data = (PUCHAR)HeapAlloc(GetProcessHeap(), 0, encrypted_size);
	status = BCryptEncrypt(hKey, data, size, NULL, NULL, NULL, encrypted_data, encrypted_size, &encrypted_size, BCRYPT_PAD_PKCS1);
	if (!NT_SUCCESS(status))
	{
		return status;
	}
	return status;
}


PyObject* import_rsa_key(PyObject* self, PyObject* args) {
	unsigned char* path;
	int type = 0;
	PyArg_ParseTuple(args, "si", &path, &type);
	BCRYPT_ALG_HANDLE hAlgorithm = 0;
	BCRYPT_KEY_HANDLE key = 0;

	BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_RSA_ALGORITHM, MS_PRIMITIVE_PROVIDER, 0);
	std::ifstream key_file((const char*)path, std::ios::binary);
	std::vector<char> key_bytes((std::istreambuf_iterator<char>(key_file)), (std::istreambuf_iterator<char>()));
	if (type == 0)
	{
		import_privkey(hAlgorithm, (PBYTE)(key_bytes.data()), key_bytes.size(), &key);
	}
	else if (type == 1)
	{
		import_pubkey(hAlgorithm, (PBYTE)(key_bytes.data()), key_bytes.size(), &key);
	}
	else
	{
		PyErr_SetString(PyExc_RuntimeError, "invalid key type");
	}

	return Py_BuildValue("K", key);
}

PyObject* decrypt_rsa_data(PyObject* self, PyObject* args) {
	Py_buffer data;
	BCRYPT_ALG_HANDLE hAlgorithm = 0;
	BCRYPT_KEY_HANDLE key = 0;
	PUCHAR decrypted_data = 0;
	ULONG decrypted_size = 0;

	PyArg_ParseTuple(args, "Ks*", &key, &data);
	decrypt_data(key, (PUCHAR)data.buf, data.len, decrypted_data, decrypted_size);

	return PyByteArray_FromStringAndSize((const char*)decrypted_data, decrypted_size);
}

PyObject* encrypt_rsa_data(PyObject* self, PyObject* args) {
	Py_buffer data;
	BCRYPT_ALG_HANDLE hAlgorithm = 0;
	BCRYPT_KEY_HANDLE key = 0;
	PUCHAR encrypted_data = 0;
	ULONG encrypted_size = 0;

	PyArg_ParseTuple(args, "Ks*", &key, &data);
	encrypt_data(key, (PUCHAR)data.buf, data.len, encrypted_data, encrypted_size);

	return PyByteArray_FromStringAndSize((const char*)encrypted_data, encrypted_size);
}