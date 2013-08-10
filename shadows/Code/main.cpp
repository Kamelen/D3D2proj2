#include "stdafx.h"
#include "WinHandler.h"
#include "program.h"


int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	program* prgm = new program;
	MSG msg = {0};
	__int64 cntsPerSec = 0;
	__int64 prevTimeStamp = 0;
	__int64 currTimeStamp = 0;
	float dt = 0.0f;

	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);


	prgm->initiate(hInstance, nCmdShow);

	// Main message loop
	while(WM_QUIT != msg.message)
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			currTimeStamp = 0;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
			dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;

			//main logic and draw calls
			prgm->run(dt);

			prevTimeStamp = currTimeStamp;
		}
	}


	delete prgm;
	return 0;
}
