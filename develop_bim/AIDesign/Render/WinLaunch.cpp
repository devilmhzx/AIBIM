#include "WinLaunch.h"



WinLaunch::WinLaunch()
{
}


WinLaunch::~WinLaunch()
{
}

HWND WinLaunch::hWnd = NULL;

HRESULT WinLaunch::InitializeWindows()
{
            WNDCLASSEX wcex;
            wcex.cbSize = sizeof(WNDCLASSEX);
            wcex.style = CS_HREDRAW | CS_VREDRAW;
            wcex.lpfnWndProc = WndProc;
            wcex.cbClsExtra = 0;
            wcex.cbWndExtra = 0;
            wcex.hInstance = ::GetModuleHandle(NULL);
            wcex.hIcon = NULL;
            wcex.hCursor = NULL;
            wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            wcex.lpszMenuName = nullptr;
            wcex.lpszClassName =  "MetisRender";
            wcex.hIconSm = NULL;
            if (!RegisterClassEx(&wcex))
                        return E_FAIL;
            RECT rc = { 0, 0, 800, 600 };
            AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
            hWnd = CreateWindow( "MetisRender",  "MetisRender",
                        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, wcex.hInstance,
                        nullptr);
            if (!hWnd)
                        return E_FAIL;

            ShowWindow(hWnd, SW_SHOW);
            return S_OK;
}
 
 LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
            switch (message) {
                         case WM_DESTROY:
                                    PostQuitMessage(0);
                                    break;
                        default:
                                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            return DefWindowProc(hWnd, message, wParam, lParam);
}