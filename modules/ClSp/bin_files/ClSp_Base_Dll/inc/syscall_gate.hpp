#pragma once
#include <Windows.h>
#include <mem_ldr.h>
#include <syslib.hpp>
#include <vector>
#include <cmd_parser.hpp>

#define PACK_NTSTATUS_STATUS	  builder.add_byte(bResult); \
						  builder.add_int(status);

#define HANDLE_SYSCALL_RESULT if (NT_SUCCESS(status)) bResult = 1;
extern syslib* psyslib;

enum syscall_argtype
{
    ATypeByte,
    ATypeShort,
    ATypeInteger,
    ATypeLong,
    ATypeRawMemory
};
/*
class syscall_arg {
private:
	ULONGLONG private_obj_ptr = 0;
	BYTE argtype = 0;
	BYTE _retn = 0;
    LONGLONG _obj_size = 0;
	std::string raw_memory;
public:
	syscall_arg(CommandParser* user_data) {
	
		_retn = user_data->get_byte();
		argtype = user_data->get_byte();
		if (argtype == ATypeLong) private_obj_ptr = user_data->get_long();
		if (argtype == ATypeRawMemory){
			raw_memory = user_data->get_strarg();
			private_obj_ptr = (ULONGLONG)raw_memory.data();
            _obj_size = (LONGLONG)raw_memory.size();
		}
	}
    LONGLONG obj_size() {
        return _obj_size;
    }
    BOOL retn() {
        return _retn;
    }
    BYTE atype(){
        return argtype;
    }
	LPVOID ptr() {
        if (_retn) return (LPVOID)&private_obj_ptr;
		return (LPVOID)private_obj_ptr;
	}
	~syscall_arg() {
		if (raw_memory.size() != NULL) raw_memory.~basic_string();
		private_obj_ptr = 0;
		argtype = 0;
	}
};
#define SYSCALL_ARGUMENT(d) syscall_arg(d).ptr()

NTSTATUS UserSyscallGate(CommandParser* user_data, ResponseBuilder* builder) {
	NTSTATUS status = ERROR_SUCCESS;
	BOOL bResult = 0;
    std::vector<syscall_arg*> arglist;
	DWORD32 syscall_hash = user_data->get_int();
	DWORD32 syscall_argc = user_data->get_int();

    for (int i = 0; i < syscall_argc; i++) { arglist.push_back(new syscall_arg(user_data)); }
    switch (syscall_argc)
    {
    case 0:
        status = psyslib->nt_call_0arg(syscall_hash);
        goto End;
    case 1:
        status = psyslib->nt_call_1arg(syscall_hash, arglist.at(0)->ptr());
        goto End;
    case 2:
        status = psyslib->nt_call_2arg(syscall_hash, arglist.at(0)->ptr(), arglist.at(1)->ptr());
        goto End;
    case 3:
        status = psyslib->nt_call_3arg(syscall_hash, arglist.at(0)->ptr(), arglist.at(1)->ptr(), arglist.at(2)->ptr());
        goto End;
    case 4:
        status = psyslib->nt_call(syscall_hash, arglist.at(0)->ptr(), arglist.at(1)->ptr(), arglist.at(2)->ptr(), arglist.at(3)->ptr());
        goto End;
    case 5:
        status = psyslib->nt_call(syscall_hash, arglist.at(0)->ptr(), arglist.at(1)->ptr(), arglist.at(2)->ptr(), arglist.at(3)->ptr(), arglist.at(4)->ptr());
        goto End;
    case 6:
        status = psyslib->nt_call(syscall_hash, arglist.at(0)->ptr(), arglist.at(1)->ptr(), arglist.at(2)->ptr(), arglist.at(3)->ptr(), arglist.at(4)->ptr(), arglist.at(5)->ptr());
        goto End;
    case 7:
        status = psyslib->nt_call(syscall_hash, arglist.at(0)->ptr(), arglist.at(1)->ptr(), arglist.at(2)->ptr(), arglist.at(3)->ptr(), arglist.at(4)->ptr(), arglist.at(5)->ptr(), arglist.at(6)->ptr());
        goto End;
    case 8:
        status = psyslib->nt_call(syscall_hash, arglist.at(0)->ptr(), arglist.at(1)->ptr(), arglist.at(2)->ptr(), arglist.at(3)->ptr(), arglist.at(4)->ptr(), arglist.at(5)->ptr(), arglist.at(6)->ptr(), arglist.at(7)->ptr());
        goto End;
    case 9:
        status = psyslib->nt_call(syscall_hash, arglist.at(0)->ptr(), arglist.at(1)->ptr(), arglist.at(2)->ptr(), arglist.at(3)->ptr(), arglist.at(4)->ptr(), arglist.at(5)->ptr(), arglist.at(6)->ptr(), arglist.at(7)->ptr(), arglist.at(8)->ptr());
        goto End;
    case 10:
        status = psyslib->nt_call(syscall_hash, arglist.at(0)->ptr(), arglist.at(1)->ptr(), arglist.at(2)->ptr(), arglist.at(3)->ptr(), arglist.at(4)->ptr(), arglist.at(5)->ptr(), arglist.at(6)->ptr(), arglist.at(7)->ptr(), arglist.at(8)->ptr(), arglist.at(9)->ptr());
        goto End;
    case 11:
        status = psyslib->nt_call(syscall_hash, arglist.at(0)->ptr(), arglist.at(1)->ptr(), arglist.at(2)->ptr(), arglist.at(3)->ptr(), arglist.at(4)->ptr(), arglist.at(5)->ptr(), arglist.at(6)->ptr(), arglist.at(7)->ptr(), arglist.at(8)->ptr(), arglist.at(9)->ptr(), arglist.at(10)->ptr());
        goto End;
    case 12:
        status = psyslib->nt_call(syscall_hash, arglist.at(0)->ptr(), arglist.at(1)->ptr(), arglist.at(2)->ptr(), arglist.at(3)->ptr(), arglist.at(4)->ptr(), arglist.at(5)->ptr(), arglist.at(6)->ptr(), arglist.at(7)->ptr(), arglist.at(8)->ptr(), arglist.at(9)->ptr(), arglist.at(10)->ptr(), arglist.at(11)->ptr());
        goto End;
    case 13:
        status = psyslib->nt_call(syscall_hash, arglist.at(0)->ptr(), arglist.at(1)->ptr(), arglist.at(2)->ptr(), arglist.at(3)->ptr(), arglist.at(4)->ptr(), arglist.at(5)->ptr(), arglist.at(6)->ptr(), arglist.at(7)->ptr(), arglist.at(8)->ptr(), arglist.at(9)->ptr(), arglist.at(10)->ptr(), arglist.at(11)->ptr(), arglist.at(12)->ptr());
        goto End;
    case 14:
        status = psyslib->nt_call(syscall_hash, arglist.at(0)->ptr(), arglist.at(1)->ptr(), arglist.at(2)->ptr(), arglist.at(3)->ptr(), arglist.at(4)->ptr(), arglist.at(5)->ptr(), arglist.at(6)->ptr(), arglist.at(7)->ptr(), arglist.at(8)->ptr(), arglist.at(9)->ptr(), arglist.at(10)->ptr(), arglist.at(11)->ptr(), arglist.at(12)->ptr(), arglist.at(13)->ptr());
        goto End;
    case 15:
        status = psyslib->nt_call(syscall_hash, arglist.at(0)->ptr(), arglist.at(1)->ptr(), arglist.at(2)->ptr(), arglist.at(3)->ptr(), arglist.at(4)->ptr(), arglist.at(5)->ptr(), arglist.at(6)->ptr(), arglist.at(7)->ptr(), arglist.at(8)->ptr(), arglist.at(9)->ptr(), arglist.at(10)->ptr(), arglist.at(11)->ptr(), arglist.at(12)->ptr(), arglist.at(13)->ptr(), arglist.at(14)->ptr());
        goto End;
    case 16:
        status = psyslib->nt_call(syscall_hash, arglist.at(0)->ptr(), arglist.at(1)->ptr(), arglist.at(2)->ptr(), arglist.at(3)->ptr(), arglist.at(4)->ptr(), arglist.at(5)->ptr(), arglist.at(6)->ptr(), arglist.at(7)->ptr(), arglist.at(8)->ptr(), arglist.at(9)->ptr(), arglist.at(10)->ptr(), arglist.at(11)->ptr(), arglist.at(12)->ptr(), arglist.at(13)->ptr(), arglist.at(14)->ptr(), arglist.at(15)->ptr());
        goto End;
    case 17:
        status = psyslib->nt_call(syscall_hash, arglist.at(0)->ptr(), arglist.at(1)->ptr(), arglist.at(2)->ptr(), arglist.at(3)->ptr(), arglist.at(4)->ptr(), arglist.at(5)->ptr(), arglist.at(6)->ptr(), arglist.at(7)->ptr(), arglist.at(8)->ptr(), arglist.at(9)->ptr(), arglist.at(10)->ptr(), arglist.at(11)->ptr(), arglist.at(12)->ptr(), arglist.at(13)->ptr(), arglist.at(14)->ptr(), arglist.at(15)->ptr(), arglist.at(16)->ptr());
        goto End;
    case 18:
        status = psyslib->nt_call(syscall_hash, arglist.at(0)->ptr(), arglist.at(1)->ptr(), arglist.at(2)->ptr(), arglist.at(3)->ptr(), arglist.at(4)->ptr(), arglist.at(5)->ptr(), arglist.at(6)->ptr(), arglist.at(7)->ptr(), arglist.at(8)->ptr(), arglist.at(9)->ptr(), arglist.at(10)->ptr(), arglist.at(11)->ptr(), arglist.at(12)->ptr(), arglist.at(13)->ptr(), arglist.at(14)->ptr(), arglist.at(15)->ptr(), arglist.at(16)->ptr(), arglist.at(17)->ptr());
        goto End;
    default:
        status = ERROR_INVALID_USER_BUFFER;
        goto End;
    }

End:
	HANDLE_SYSCALL_RESULT
	builder->add_byte(bResult);
	builder->add_int(status);

    int ret_argc = 0;
    for (int i = 0; i < syscall_argc; i++) {
        if (arglist.at(i)->retn()) {
            ret_argc++;
        }
    }
    builder->add_int(ret_argc);
    for (int i = 0; i < syscall_argc; i++) { 
        if (arglist.at(i)->retn()) {
            if (arglist.at(i)->atype() == ATypeRawMemory) {
                builder->add_bstrarg((PCHAR)(*(PCHAR)arglist.at(i)->ptr()), arglist.at(i)->obj_size());
            }
            else if (arglist.at(i)->atype() == ATypeLong){
#ifdef _WIN64
                builder->add_long(*(PLONGLONG)(arglist.at(i)->ptr()));
#else
                builder->add_long(*(PLONG)(arglist.at(i)->ptr()));
#endif
            }
        }
        delete arglist.at(i);
    }
    arglist.~vector();
	return status;
}
*/

#define SYSCALL_ARGUMENT ((PVOID)user_data->get_long())
NTSTATUS UserSyscallGate(CommandParser* user_data, ResponseBuilder* builder) {
    NTSTATUS status = ERROR_SUCCESS;
    BOOL bResult = 0;
    DWORD32 syscall_hash = user_data->get_int();
    DWORD32 syscall_argc = user_data->get_int();

    switch (syscall_argc)
    {
    case 0:
        status = psyslib->nt_call_0arg(syscall_hash);
        goto End;
    case 1:
        status = psyslib->nt_call_1arg(syscall_hash, SYSCALL_ARGUMENT);
        goto End;
    case 2:
        status = psyslib->nt_call_2arg(syscall_hash, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT);
        goto End;
    case 3:
        status = psyslib->nt_call_3arg(syscall_hash, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT);
        goto End;
    case 4:
        status = psyslib->nt_call(syscall_hash, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT);
        goto End;
    case 5:
        status = psyslib->nt_call(syscall_hash, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT);
        goto End;
    case 6:
        status = psyslib->nt_call(syscall_hash, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT);
        goto End;
    case 7:
        status = psyslib->nt_call(syscall_hash, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT);
        goto End;
    case 8:
        status = psyslib->nt_call(syscall_hash, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT);
        goto End;
    case 9:
        status = psyslib->nt_call(syscall_hash, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT);
        goto End;
    case 10:
        status = psyslib->nt_call(syscall_hash, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT);
        goto End;
    case 11:
        status = psyslib->nt_call(syscall_hash, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT);
        goto End;
    case 12:
        status = psyslib->nt_call(syscall_hash, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT);
        goto End;
    case 13:
        status = psyslib->nt_call(syscall_hash, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT);
        goto End;
    case 14:
        status = psyslib->nt_call(syscall_hash, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT);
        goto End;
    case 15:
        status = psyslib->nt_call(syscall_hash, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT);
        goto End;
    case 16:
        status = psyslib->nt_call(syscall_hash, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT);
        goto End;
    case 17:
        status = psyslib->nt_call(syscall_hash, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT);
        goto End;
    case 18:
        status = psyslib->nt_call(syscall_hash, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT, SYSCALL_ARGUMENT);
        goto End;
    default:
        status = ERROR_INVALID_USER_BUFFER;
        goto End;
    }

End:
    HANDLE_SYSCALL_RESULT
    builder->add_byte(bResult);
    builder->add_int(status);

    return status;
}