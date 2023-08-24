#include <Windows.h>
#pragma comment (lib, "bcrypt.lib")
#define NT_SUCCESS(Status)          (((NTSTATUS)(Status)) >= 0)
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
