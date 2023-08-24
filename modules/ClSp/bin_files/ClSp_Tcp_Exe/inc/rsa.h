#include <Windows.h>

NTSTATUS encrypt_data(BCRYPT_KEY_HANDLE hKey, PUCHAR data, ULONG size, PUCHAR& encrypted_data, ULONG& encrypted_size);
NTSTATUS import_pubkey(BCRYPT_ALG_HANDLE hProvider, PBYTE lpData, ULONG keySize, BCRYPT_KEY_HANDLE* hKey);