#include <CryptoModule.h>
#include <vector>
#include <fstream>
#include <string>
#include <AES.h>
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
		PyErr_SetString(PyExc_KeyError, "Invalid private key");
		PyErr_Print();
	}

	return status;
}

NTSTATUS import_pubkey(BCRYPT_ALG_HANDLE hProvider, PBYTE lpData, ULONG keySize, BCRYPT_KEY_HANDLE* hKey)
{
	NTSTATUS status;


	status = BCryptImportKeyPair(hProvider, NULL, BCRYPT_RSAPUBLIC_BLOB, hKey, lpData, keySize, 0);
	if (!NT_SUCCESS(status))
	{
		PyErr_SetString(PyExc_KeyError, "Invalid public key");
		PyErr_Print();
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
		if (!NT_SUCCESS(import_privkey(hAlgorithm, (PBYTE)(key_bytes.data()), key_bytes.size(), &key)))
		{
			Py_RETURN_NONE;
		}
	}
	else if (type == 1)
	{
		if (!NT_SUCCESS(import_pubkey(hAlgorithm, (PBYTE)(key_bytes.data()), key_bytes.size(), &key)))
		{
			Py_RETURN_NONE;
		}
	}
	else
	{
		PyErr_SetString(PyExc_RuntimeError, "Invalid key type");
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

PyObject* decrypt_aes_data(PyObject* self, PyObject* args) {
	Py_buffer data, key, iv;

	PyArg_ParseTuple(args, "s*s*s*", &key, &iv, &data);

	if (data.len % 16 != 0 || data.len == 0)
	{
		PyErr_SetString(PyExc_ArithmeticError, "Invalid data for decryption");
		PyErr_Print();
		Py_RETURN_NONE;
	}

	AES aes(AESKeyLength::AES_256);
	std::vector<UCHAR> enc_data((char*)data.buf, (char*)data.buf + data.len);
	std::vector<UCHAR> aes_key((char*)key.buf, (char*)key.buf + key.len);
	std::vector<UCHAR> aes_iv((char*)iv.buf, (char*)iv.buf + iv.len);

	std::vector<UCHAR> dec = aes.DecryptCBC(enc_data, aes_key, aes_iv);
	return PyByteArray_FromStringAndSize((const char*)dec.data(), dec.size());
}

PyObject* encrypt_aes_data(PyObject* self, PyObject* args) {
	Py_buffer data, key, iv;

	PyArg_ParseTuple(args, "s*s*s*", &key, &iv, &data);

	AES aes(AESKeyLength::AES_256);

	std::vector<UCHAR> plain_data((char*)data.buf, (char*)data.buf + data.len);
	std::vector<UCHAR> aes_key((char*)key.buf, (char*)key.buf + key.len);
	std::vector<UCHAR> aes_iv((char*)iv.buf, (char*)iv.buf + iv.len);
	while (plain_data.size() % 16 != 0 || plain_data.size() == 0) {
		plain_data.resize(plain_data.size() + 1);
	}

	std::vector<UCHAR> enc = aes.EncryptCBC(plain_data, aes_key, aes_iv);
	return PyByteArray_FromStringAndSize((const char*)enc.data(), enc.size());
}