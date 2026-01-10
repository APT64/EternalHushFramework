section .text
global _nt32call

_nt32call:
	mov ecx, [esp + 12]
	not ecx
    add ecx, 1
    lea edx, [esp + ecx * 4]

	mov ecx, [esp]
	mov [edx], ecx

	mov [edx - 4], esi
	mov [edx - 8], edi

	mov eax, [esp + 4]
    mov ecx, [esp + 12]
    lea esi, [esp + 16]
    lea edi, [edx + 4]
    rep movsd

    mov esi, [esp + 8]
    mov esp, edx
    

    lea ecx, [esi+0xA]
    push ecx
    lea ecx, [esi+0xF]

    mov esi, [edx - 4]
    mov edi, [edx - 8]

    mov edx, esp
    ;lea ecx, [esi+0x5]
    jmp ecx