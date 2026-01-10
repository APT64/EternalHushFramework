#include <Windows.h>
#include <vector>
#include <WbemCli.h>
#include <stdio.h>
#include <cutils.h>
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "propsys.lib")
#include "resp_builder.hpp"
#include <propvarutil.h>

class WmiLib {
public:
	HRESULT InitializeWmi();
	HRESULT ConnectServer(PWCHAR resource_name, PWCHAR user, PWCHAR password, PWCHAR authority);
	HRESULT ExecuteQuery(PWCHAR query);
	HRESULT CallMethod(BSTR ClassName, BSTR MethodName);
	VOID ReleaseWmi();
	std::vector<UCHAR> PackResult(std::vector<PWCHAR> filter, DWORD* row_count);
	std::vector<UCHAR> PackMethodResult(std::vector<PWCHAR> filter, DWORD* row_count);
	~WmiLib();
	bool released = false;
private:
	HRESULT hRes;
	IWbemLocator* pLocator = NULL;
	IEnumWbemClassObject* pEnumerator = NULL;
	IWbemServices* pService = NULL;
	IWbemClassObject* clsObj = NULL;
	int numElems;
	ResponseBuilder builder;

	IWbemClassObject* pMethod = NULL;
	IWbemClassObject* pInInst = NULL;
	IWbemClassObject* pClass = NULL;
	BSTR iMethodName = NULL;

};
#define _HRES char abc[256]; sprintf(abc, "WMILIB hres %llx", hRes); OutputDebugStringA(abc);
#define HRES
HRESULT WmiLib::InitializeWmi() {
	hRes = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pLocator));
	HRES
	return hRes;
}

HRESULT WmiLib::ConnectServer(PWCHAR resource_name, PWCHAR user, PWCHAR password, PWCHAR authority) {
	if (FAILED(hRes = pLocator->ConnectServer(BSTR(resource_name), user, password, NULL, WBEM_FLAG_CONNECT_USE_MAX_WAIT, authority, NULL, &pService)))
	{
		pLocator->Release();
	}
	HRES
	return hRes;
}

HRESULT WmiLib::ExecuteQuery(PWCHAR query) {
	hRes = CoSetProxyBlanket(pService, 
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE);


	if (FAILED(hRes = pService->ExecQuery(BSTR(L"WQL"), BSTR(query), WBEM_FLAG_FORWARD_ONLY, NULL, &pEnumerator)))
	{
		pLocator->Release();
		pService->Release();
	}
	HRES
	return hRes;
}

std::vector<UCHAR> WmiLib::PackResult(std::vector<PWCHAR> filter, DWORD* row_count) {
	hRes = CoSetProxyBlanket(pEnumerator,
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE);
	while ((hRes = pEnumerator->Next(WBEM_INFINITE, 1, &clsObj, (ULONG*)&numElems)) != WBEM_S_FALSE)
	{

		if (FAILED(hRes))
			break;

		VARIANT vRet;
		for (int i = 0; i < filter.size(); i++)
		{
			VariantInit(&vRet);
			hRes = clsObj->Get(filter[i], 0, &vRet, NULL, NULL);
			if (FAILED(hRes))
			{
				SetLastError(hRes);
				return builder.build();
			}
			builder.add_short(vRet.vt);
			if (vRet.vt == VT_INT ||
				vRet.vt == VT_UINT ||
				vRet.vt == VT_HRESULT ||
				vRet.vt == VT_PTR ||
				vRet.vt == VT_INT_PTR ||
				vRet.vt == VT_UINT_PTR ||
				vRet.vt == VT_DECIMAL ||
				vRet.vt == VT_UI1 ||
				vRet.vt == VT_UI2 ||
				vRet.vt == VT_UI4 ||
				vRet.vt == VT_UI8 ||
				vRet.vt == VT_I1 ||
				vRet.vt == VT_I2 ||
				vRet.vt == VT_I4 ||
				vRet.vt == VT_I8 ||
				vRet.vt == VT_R4 ||
				vRet.vt == VT_R8 ||
				vRet.vt == VT_ERROR
				)
			{
				builder.add_long(vRet.ullVal);
			}
			else if (
				vRet.vt == VT_BSTR ||
				vRet.vt == VT_LPWSTR
				)
			{
				builder.add_bstrarg((char*)vRet.bstrVal, _wcslen(vRet.bstrVal) * 2 );
			}
			else if (vRet.vt == VT_LPSTR)
			{
				builder.add_bstrarg((char*)vRet.bstrVal, _strlen((char*)vRet.bstrVal ));
			}
			else if (vRet.vt == VT_BOOL)
			{
				BOOL b = 0;
				VariantToBoolean(vRet, &b);
				builder.add_byte(b);
			}
			VariantClear(&vRet);
		}
		(*row_count)++;
		clsObj->Release();
	}
	HRES
	SetLastError(0);
	if (hRes != NULL && hRes != 1) SetLastError(hRes);
	return builder.build();
}

std::vector<UCHAR> WmiLib::PackMethodResult(std::vector<PWCHAR> filter, DWORD* row_count) {
	hRes = CoSetProxyBlanket(pEnumerator,
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE);
	while ((hRes = pEnumerator->Next(WBEM_INFINITE, 1, &clsObj, (ULONG*)&numElems)) != WBEM_S_FALSE)
	{

		if (FAILED(hRes))
			break;
		if (numElems == 0) break;
	

		VARIANT vRet, vPath;
		for (int i = 0; i < filter.size(); i++)
		{
			VariantInit(&vRet);
			VariantInit(&vPath);

			hRes = clsObj->Get(L"__PATH", 0, &vPath, NULL, NULL);
			if (FAILED(hRes))
			{
				SetLastError(hRes);
				builder.add_short(VT_ERROR);
				builder.add_long(NULL);
				continue;
			}

			hRes = pService->ExecMethod(vPath.bstrVal, iMethodName, 0, NULL, NULL, &pMethod, NULL);
			if (FAILED(hRes))
			{
				SetLastError(hRes);
				builder.add_short(VT_ERROR);
				builder.add_long(NULL);
				continue;
			}

			hRes = pMethod->Get(filter[i], 0, &vRet, NULL, NULL);
			if (FAILED(hRes))
			{
				SetLastError(hRes);
				builder.add_short(VT_ERROR);
				builder.add_long(NULL);
				continue;
			}
			builder.add_short(vRet.vt);
			if (vRet.vt == VT_INT ||
				vRet.vt == VT_UINT ||
				vRet.vt == VT_HRESULT ||
				vRet.vt == VT_PTR ||
				vRet.vt == VT_INT_PTR ||
				vRet.vt == VT_UINT_PTR ||
				vRet.vt == VT_DECIMAL ||
				vRet.vt == VT_UI1 ||
				vRet.vt == VT_UI2 ||
				vRet.vt == VT_UI4 ||
				vRet.vt == VT_UI8 ||
				vRet.vt == VT_I1 ||
				vRet.vt == VT_I2 ||
				vRet.vt == VT_I4 ||
				vRet.vt == VT_I8 ||
				vRet.vt == VT_R4 ||
				vRet.vt == VT_R8 ||
				vRet.vt == VT_ERROR ||
				vRet.vt == VT_NULL
				)
			{
				builder.add_long(vRet.ullVal);
			}
			else if (
				vRet.vt == VT_BSTR ||
				vRet.vt == VT_LPWSTR
				)
			{
				builder.add_bstrarg((char*)vRet.bstrVal, _wcslen(vRet.bstrVal) * 2);
			}
			else if (vRet.vt == VT_LPSTR)
			{
				builder.add_bstrarg((char*)vRet.bstrVal, _strlen((char*)vRet.bstrVal));
			}
			else if (vRet.vt == VT_BOOL)
			{
				BOOL b = 0;
				VariantToBoolean(vRet, &b);
				builder.add_byte(b);
			}
			VariantClear(&vRet);
			VariantClear(&vPath);
		}
		(*row_count)++;
		clsObj->Release();
	}
	HRES
		SetLastError(0);
	//if (hRes != NULL && hRes != 1) SetLastError(hRes);
	return builder.build();
}

HRESULT WmiLib::CallMethod(BSTR ClassName, BSTR MethodName) {
	hRes = CoSetProxyBlanket(pService,
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE);
	hRes = pService->GetObject(ClassName, 0, NULL, &pClass, NULL);
	if (FAILED(hRes))
	{
		pService->Release();
		pLocator->Release();
		return hRes;
	}

	// get the desired method
	// in our case, we only need pmethodGetOwner since GetOwner really only has output
	hRes = pClass->GetMethod(MethodName, 0, NULL, &pMethod);
	if (FAILED(hRes))
	{
		pService->Release();
		pLocator->Release();
		return hRes;
	}

	// spawn the instance of the method
	hRes = pMethod->SpawnInstance(0, &pInInst);
	if (FAILED(hRes))
	{
		pService->Release();
		pLocator->Release();
		return hRes;
	}
	HRES
	iMethodName = SysAllocString(MethodName);
	return hRes;
}

VOID WmiLib::ReleaseWmi() {
	this->~WmiLib();
}

WmiLib::~WmiLib() {
	released = true;
	if (pEnumerator)
	{
		pEnumerator->Release();
		pEnumerator = NULL;
	}
	if (pService)
	{
		pService->Release();
		pService = NULL;
	}
	if (pLocator)
	{
		pLocator->Release();
		pLocator = NULL;
	}
	if (pMethod) {
		pMethod->Release();
		pMethod = NULL;
	}
	if (pClass) {
		pClass->Release();
		pClass = NULL;
	}
	if (pInInst) {
		pInInst->Release();
		pInInst = NULL;
	}
}