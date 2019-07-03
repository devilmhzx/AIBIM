#pragma once
#include <Windows.h>
class WinLaunch
{
public:
            WinLaunch();
            ~WinLaunch();
            static HWND             hWnd;
            static HRESULT          InitializeWindows();
};
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
 