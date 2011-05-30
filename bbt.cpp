// bbt.cpp : 定义控制台应用程序的入口点。
//
//#include "stdafx.h"
#include "InstanceMonitor.h"
#include "Shellapi.h"

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <locale>
#include <tchar.h>

#ifdef UNITTEST
#include "gtest/gtest.h"
#endif

using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _UNICODE
#define _tmain wmain
#else
#define _tmain main
#endif


#define TRAY_NOTIFY (WM_USER + 1)

// 唯一的应用程序对象

//CWinApp theApp;
CInstanceMonitor theApp;

// Global variables

// The main window class name.
static TCHAR szWindowClass[] = _T("win32app");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Win32 BlueEvent Monitor");

HINSTANCE hInst;
NOTIFYICONDATA nid;
HANDLE     hThread;
DWORD      threadId;


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
#ifdef UNITTEST
  std::cout << "Running main() from gtest_main.cc\n";

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();	
#else
	theApp.AppStart();
#endif
	return 0;
}


UINT MainThread(LPVOID param){
	return _tmain(0,NULL,NULL);
}

void TerminateApp()
{
	theApp.Shutdown();
	TerminateThread(hThread,0);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    TCHAR greeting[] = _T("Hello, World!");

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        // Here your application is laid out.
        // For this introduction, we just print out "Hello, World!"
        // in the top left corner.
        TextOut(hdc,
            5, 5,
            greeting, _tcslen(greeting));
        // End application specific layout section.

        EndPaint(hWnd, &ps);
        break;

	case WM_CLOSE:
		if(IDOK == MessageBox(NULL, L"Are you sure to quit Health Monitor?", L"Quit?", MB_OKCANCEL)) {
			ShowWindow(hWnd,SW_HIDE);
			Shell_NotifyIcon(NIM_DELETE, &nid);
			TerminateApp();
			PostQuitMessage(0);
		}
		break;


    case WM_DESTROY:
		TerminateApp();
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
        break;

	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED) {
			ShowWindow(hWnd, SW_HIDE);
		}
		break;

	case TRAY_NOTIFY:
		switch(lParam)
		{
		case WM_LBUTTONDBLCLK:
			//MessageBox(NULL, L"Tray icon double clicked!", L"clicked", MB_OK);
			ShowWindow(hWnd,SW_SHOW);
			ShowWindow(hWnd,SW_RESTORE);
			break;
		case WM_RBUTTONUP:
			if(IDOK == MessageBox(NULL, L"Are you sure to quit Health Monitor?", L"Quit?", MB_OKCANCEL)) {
				ShowWindow(hWnd,SW_HIDE);
				Shell_NotifyIcon(NIM_DELETE, &nid);
				TerminateApp();

				PostQuitMessage(0);

				return 0;
			};
			break;

		default:
			   return DefWindowProc(hWnd, message, wParam, lParam);
		};
		break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpszCmd, int nCmdShow)
{
	locale loc("");

	wofstream logFile( "bbt.log");
	logFile.rdbuf()->pubsetbuf(0,0);
	wstreambuf *outbuf = wcout.rdbuf(logFile.rdbuf());
	wstreambuf *errbuf = wcerr.rdbuf(logFile.rdbuf()); 

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Win32 Guided Tour"),
            NULL);

        return 1;
    }

	static TCHAR szWindowClass[] = _T("win32app");
	static TCHAR szTitle[] = _T("Win32 BlueEvent Monitor");

	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// 500, 100: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		500, 100,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Win32 BlueEvent Monitor"),
			NULL);

		return 1;
	}
	ShowWindow(hWnd,nCmdShow);

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	//ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);

	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = 100;
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uCallbackMessage = TRAY_NOTIFY;
	nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcscpy_s(nid.szTip, L"Tray Icon");
	nid.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	 
	Shell_NotifyIcon(NIM_ADD, &nid);

	hThread = ::CreateThread (
    0, // Security attributes
    0, // Stack size
	(LPTHREAD_START_ROUTINE)(MainThread),
    (LPVOID)0,
    0/*CREATE_SUSPENDED*/,
    &threadId);

	MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	TerminateThread(hThread,0);

	wcout.rdbuf(outbuf);
	wcerr.rdbuf(errbuf); 
    return (int) msg.wParam;
}
