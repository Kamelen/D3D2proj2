#pragma once
#include "stdafx.h"
#include <WindowsX.h>

class WinHandler
{
	public:
		WinHandler(void);
		~WinHandler(void);

		HRESULT initWindow(HINSTANCE hInstance, int cmdShow);
		static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
		virtual void mouseOnMove(WPARAM btnState, int x, int y){};

	protected:
		HINSTANCE   hInst;
		HWND        hWnd;



};


