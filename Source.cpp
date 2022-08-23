#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "dwmapi")

#include <windows.h>
#include <dwmapi.h>
#include "resource.h"

#define CLIENT_WIDTH 300
#define CLIENT_HEIGHT 128
#define FONT_SIZE (-75)
#define BKCOLOR RGB(134, 161, 110)

TCHAR szClassName[] = TEXT("clock");

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HGLOBAL hGlobal = NULL;
	static HANDLE hFontResource = NULL;
	static HFONT hFont = NULL;
	static SYSTEMTIME st = {};
	static WCHAR szTime[32] = {};
	static HDC hMemDC = 0;
	static HBITMAP hBitmap;
	static HBITMAP hOldBitmap;
	switch (msg)
	{
	case WM_CREATE:
		{
			HRSRC hRes = FindResource(0, MAKEINTRESOURCE(IDR_FONT1), RT_FONT);
			if (!hRes)
			{
				return -1;
			}
			DWORD dwSize = SizeofResource(0, hRes);
			if (!dwSize)
			{
				return -1;
			}
			hGlobal = LoadResource(0, hRes);
			if (!hGlobal)
			{
				return -1;
			}
			LPVOID pBuffer = LockResource(hGlobal);
			if (!pBuffer)
			{
				return -1;
			}
			DWORD dwFonts;
			hFontResource = AddFontMemResourceEx(pBuffer, dwSize, 0, &dwFonts);
			if (!hFontResource)
			{
				return -1;
			}
		}
		hFont = CreateFontW(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, L"DigitalNumber");
		SendMessage(hWnd, WM_TIMER, 0, 0);
		SetTimer(hWnd, 0x1234, 1, NULL);
		break;
	case WM_TIMER:
		{
			SYSTEMTIME st_ = {};
			double vt = 0.0, vt_ = 0.0;
			GetLocalTime(&st_);
			SystemTimeToVariantTime(&st, &vt);
			SystemTimeToVariantTime(&st_, &vt_);
			if (vt != vt_) {
				st = st_;
				wsprintfW(szTime, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
				// 描画
				RECT rect = {};
				GetClientRect(hWnd, &rect);
				// 背景塗りつぶし
				{
					HBRUSH hBrush = CreateSolidBrush(BKCOLOR);
					FillRect(hMemDC, &rect, hBrush);
					DeleteObject(hBrush);
				}
				// 文字描画
				{
					HFONT hOldFont = (HFONT)SelectObject(hMemDC, hFont);
					int nOldBkMode = SetBkMode(hMemDC, TRANSPARENT);
					COLORREF cOldColor = SetTextColor(hMemDC, RGB(124, 150, 101));
					DrawText(hMemDC, L"88:88:88", -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
					SetTextColor(hMemDC, RGB(38, 41, 52));
					DrawText(hMemDC, szTime, -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
					SetTextColor(hMemDC, cOldColor);
					SetBkMode(hMemDC, nOldBkMode);
					SelectObject(hMemDC, hOldFont);
				}
				InvalidateRect(hWnd, 0, 0);
			}
		}
		break;
	case WM_APP:
		if (hMemDC) {
			if (hOldBitmap) {
				SelectObject(hMemDC, hOldBitmap);
			}
			if (hBitmap) {
				DeleteObject(hBitmap);
				hBitmap = 0;
			}
			DeleteDC(hMemDC);
			hMemDC = 0;
		}
		break;
	case WM_SIZE:
		{
			SendMessage(hWnd, WM_APP, 0, 0);
			HDC hdc = GetDC(hWnd);
			if (hdc) {
				hMemDC = CreateCompatibleDC(hdc);
				if (hMemDC) {
					RECT rect;
					GetClientRect(hWnd, &rect);
					hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
					hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
				}
				ReleaseDC(hWnd, hdc);
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			RECT rect = {};
			GetClientRect(hWnd, &rect);
			BitBlt(hdc, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_NCHITTEST:
		wParam = DefWindowProc(hWnd, msg, wParam, lParam);
		if (wParam == HTCLIENT)
			return HTCAPTION;
		else
			return wParam;
	case WM_DESTROY:
		KillTimer(hWnd, 0x1234);
		SendMessage(hWnd, WM_APP, 0, 0);
		if (hFont)
			DeleteObject(hFont);
		if (hFontResource)
			RemoveFontMemResourceEx(hFontResource);
		if (hGlobal)
			FreeResource(hGlobal);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	MSG msg;
	HBRUSH hBrush = CreateSolidBrush(BKCOLOR);
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON1)),
		LoadCursor(0,IDC_ARROW),
		hBrush,
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	DWORD dwWindowStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
	RECT rect = {};
	SetRect(&rect, 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT);
	AdjustWindowRect(&rect, dwWindowStyle, 0);
	HWND hWnd = CreateWindowEx(
		WS_EX_TOPMOST,
		szClassName,
		TEXT("clock"),
		dwWindowStyle,
		CW_USEDEFAULT,
		0,
		rect.right - rect.left,
		rect.bottom - rect.top,
		0,
		0,
		hInstance,
		0
	);
	// change caption color for windows 11
	COLORREF bkColor = BKCOLOR;
	BOOL SET_CAPTION_COLOR = SUCCEEDED(DwmSetWindowAttribute(
		hWnd, 35/*DWMWINDOWATTRIBUTE::DWMWA_CAPTION_COLOR*/,
		&bkColor, sizeof(COLORREF)));
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteObject(hBrush);
	return (int)msg.wParam;
}
