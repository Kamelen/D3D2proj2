////--------------------------------------------------------------------------------------
//// File: TemplateMain.cpp
////
//// BTH-D3D-Template
////
//// Copyright (c) Stefan Petersson 2011. All rights reserved.
////--------------------------------------------------------------------------------------
//#include "stdafx.h"
//
//#include "Shader.h"
//#include "Buffer.h"
//
////--------------------------------------------------------------------------------------
//// Global Variables
////--------------------------------------------------------------------------------------
//HINSTANCE				g_hInst					= NULL;  
//HWND					g_hWnd					= NULL;
//
//IDXGISwapChain*         g_SwapChain				= NULL;
//ID3D11RenderTargetView* g_RenderTargetView		= NULL;
//ID3D11Texture2D*        g_DepthStencil			= NULL;
//ID3D11DepthStencilView* g_DepthStencilView		= NULL;
//ID3D11Device*			g_Device				= NULL;
//ID3D11DeviceContext*	g_DeviceContext			= NULL;
//
//Shader*					g_Shader				= NULL;
//Buffer*					g_VertexBuffer			= NULL;
//
////--------------------------------------------------------------------------------------
//// Forward declarations
////--------------------------------------------------------------------------------------
//HRESULT             InitWindow( HINSTANCE hInstance, int nCmdShow );
//LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
//HRESULT				Render(float deltaTime);
//HRESULT				Update(float deltaTime);
//HRESULT				InitDirect3D();
//char*				FeatureLevelToString(D3D_FEATURE_LEVEL featureLevel);
//
//
////--------------------------------------------------------------------------------------
//// Entry point to the program. Initializes everything and goes into a message processing 
//// loop. Idle time is used to render the scene.
////--------------------------------------------------------------------------------------
//int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
//{
//	if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
//		return 0;
//
//	if( FAILED( InitDirect3D() ) )
//		return 0;
//
//	__int64 cntsPerSec = 0;
//	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
//	float secsPerCnt = 1.0f / (float)cntsPerSec;
//
//	__int64 prevTimeStamp = 0;
//	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);
//
//	// Main message loop
//	MSG msg = {0};
//	while(WM_QUIT != msg.message)
//	{
//		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
//		{
//			TranslateMessage( &msg );
//			DispatchMessage( &msg );
//		}
//		else
//		{
//			__int64 currTimeStamp = 0;
//			QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
//			float dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;
//
//			//render
//			Update(dt);
//			Render(dt);
//
//			prevTimeStamp = currTimeStamp;
//		}
//	}
//
//	return (int) msg.wParam;
//}
//
////--------------------------------------------------------------------------------------
//// Register class and create window
////--------------------------------------------------------------------------------------
//HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
//{
//	// Register class
//	WNDCLASSEX wcex;
//	wcex.cbSize = sizeof(WNDCLASSEX); 
//	wcex.style          = CS_HREDRAW | CS_VREDRAW;
//	wcex.lpfnWndProc    = WndProc;
//	wcex.cbClsExtra     = 0;
//	wcex.cbWndExtra     = 0;
//	wcex.hInstance      = hInstance;
//	wcex.hIcon          = 0;
//	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
//	wcex.lpszMenuName   = NULL;
//	wcex.lpszClassName  = "BTH_D3D_Template";
//	wcex.hIconSm        = 0;
//	if( !RegisterClassEx(&wcex) )
//		return E_FAIL;
//
//	// Adjust and create window
//	g_hInst = hInstance; 
//	RECT rc = { 0, 0, 1024, 768 };
//	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
//	
//	if(!(g_hWnd = CreateWindow(
//							"BTH_D3D_Template",
//							"BTH - Direct3D 11.0 Template",
//							WS_OVERLAPPEDWINDOW,
//							CW_USEDEFAULT,
//							CW_USEDEFAULT,
//							rc.right - rc.left,
//							rc.bottom - rc.top,
//							NULL,
//							NULL,
//							hInstance,
//							NULL)))
//	{
//		return E_FAIL;
//	}
//
//	ShowWindow( g_hWnd, nCmdShow );
//
//	return S_OK;
//}
//
////--------------------------------------------------------------------------------------
//// Create Direct3D device and swap chain
////--------------------------------------------------------------------------------------
//HRESULT InitDirect3D()
//{
//	HRESULT hr = S_OK;;
//
//	RECT rc;
//	GetClientRect( g_hWnd, &rc );
//	int screenWidth = rc.right - rc.left;
//	int screenHeight = rc.bottom - rc.top;
//
//	UINT createDeviceFlags = 0;
//#ifdef _DEBUG
//	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
//#endif
//
//	D3D_DRIVER_TYPE driverType;
//
//	D3D_DRIVER_TYPE driverTypes[] = 
//	{
//		D3D_DRIVER_TYPE_HARDWARE,
//		D3D_DRIVER_TYPE_REFERENCE,
//	};
//	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]); //same as ARRAYSIZE(x) macro
//
//	DXGI_SWAP_CHAIN_DESC sd;
//	ZeroMemory( &sd, sizeof(sd) );
//	sd.BufferCount = 1;
//	sd.BufferDesc.Width = screenWidth;
//	sd.BufferDesc.Height = screenHeight;
//	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	sd.BufferDesc.RefreshRate.Numerator = 60;
//	sd.BufferDesc.RefreshRate.Denominator = 1;
//	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//	sd.OutputWindow = g_hWnd;
//	sd.SampleDesc.Count = 1;
//	sd.SampleDesc.Quality = 0;
//	sd.Windowed = TRUE;
//
//	D3D_FEATURE_LEVEL featureLevelsToTry[] = {
//		D3D_FEATURE_LEVEL_11_0,
//		D3D_FEATURE_LEVEL_10_1,
//		D3D_FEATURE_LEVEL_10_0
//	};
//	D3D_FEATURE_LEVEL initiatedFeatureLevel;
//
//	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
//	{
//		driverType = driverTypes[driverTypeIndex];
//		hr = D3D11CreateDeviceAndSwapChain(
//			NULL,
//			driverType,
//			NULL,
//			createDeviceFlags,
//			featureLevelsToTry,
//			ARRAYSIZE(featureLevelsToTry),
//			D3D11_SDK_VERSION,
//			&sd,
//			&g_SwapChain,
//			&g_Device,
//			&initiatedFeatureLevel,
//			&g_DeviceContext);
//
//		if( SUCCEEDED( hr ) )
//		{
//			char title[256];
//			sprintf_s(
//				title,
//				sizeof(title),
//				"BTH - Direct3D 11.0 Template | Direct3D 11.0 device initiated with Direct3D %s feature level",
//				FeatureLevelToString(initiatedFeatureLevel)
//			);
//			SetWindowText(g_hWnd, title);
//
//			break;
//		}
//	}
//	if( FAILED(hr) )
//		return hr;
//
//	// Create a render target view
//	ID3D11Texture2D* pBackBuffer;
//	hr = g_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&pBackBuffer );
//	if( FAILED(hr) )
//		return hr;
//
//	hr = g_Device->CreateRenderTargetView( pBackBuffer, NULL, &g_RenderTargetView );
//	pBackBuffer->Release();
//	if( FAILED(hr) )
//		return hr;
//
//
//	// Create depth stencil texture
//	D3D11_TEXTURE2D_DESC descDepth;
//	descDepth.Width = screenWidth;
//	descDepth.Height = screenHeight;
//	descDepth.MipLevels = 1;
//	descDepth.ArraySize = 1;
//	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
//	descDepth.SampleDesc.Count = 1;
//	descDepth.SampleDesc.Quality = 0;
//	descDepth.Usage = D3D11_USAGE_DEFAULT;
//	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
//	descDepth.CPUAccessFlags = 0;
//	descDepth.MiscFlags = 0;
//	hr = g_Device->CreateTexture2D( &descDepth, NULL, &g_DepthStencil );
//	if( FAILED(hr) )
//		return hr;
//
//	// Create the depth stencil view
//	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
//	ZeroMemory(&descDSV, sizeof(descDSV));
//	descDSV.Format = descDepth.Format;
//	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//	descDSV.Texture2D.MipSlice = 0;
//	hr = g_Device->CreateDepthStencilView( g_DepthStencil, &descDSV, &g_DepthStencilView );
//	if( FAILED(hr) )
//		return hr;
//
//
//	g_DeviceContext->OMSetRenderTargets( 1, &g_RenderTargetView, g_DepthStencilView );
//
//	// Setup the viewport
//	D3D11_VIEWPORT vp;
//	vp.Width = (float)screenWidth;
//	vp.Height = (float)screenHeight;
//	vp.MinDepth = 0.0f;
//	vp.MaxDepth = 1.0f;
//	vp.TopLeftX = 0;
//	vp.TopLeftY = 0;
//	g_DeviceContext->RSSetViewports( 1, &vp );
//
//
//	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//
//	g_Shader = new Shader();
//	if(FAILED(g_Shader->Init(g_Device, g_DeviceContext, "../Shaders/Basic.fx", inputDesc, 1)))
//	{
//		return E_FAIL;
//	}
//
//	struct Vertex
//	{
//		D3DXVECTOR3 pos;
//	};
//
//	Vertex mesh[] = {
//		D3DXVECTOR3(1,0,0),
//		D3DXVECTOR3(-1,0,0),
//		D3DXVECTOR3(0,1,0)
//	};
//
//	BUFFER_INIT_DESC bufferDesc;
//	bufferDesc.ElementSize = sizeof(Vertex);
//	bufferDesc.InitData = mesh;
//	bufferDesc.NumElements = 3;
//	bufferDesc.Type = VERTEX_BUFFER;
//	bufferDesc.Usage = BUFFER_DEFAULT;
//
//	g_VertexBuffer = new Buffer();
//	if(FAILED(g_VertexBuffer->Init(g_Device, g_DeviceContext, bufferDesc)))
//	{
//		return E_FAIL;
//	}
//
//	return S_OK;
//}
//
//HRESULT Update(float deltaTime)
//{
//	return S_OK;
//}
//
//HRESULT Render(float deltaTime)
//{
//	//clear render target
//	static float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 0.0f };
//	g_DeviceContext->ClearRenderTargetView( g_RenderTargetView, ClearColor );
//
//	//clear depth info
//	g_DeviceContext->ClearDepthStencilView( g_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
//
//	//set topology
//	g_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//
//	//calculate WVP matrix
//	static float rotY = 0.0f;
//	rotY += deltaTime;
//	D3DXMATRIX world, view, proj, wvp;
//	D3DXMatrixRotationY(&world, rotY);
//	D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(0,0,-5), &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,1,0));
//	D3DXMatrixPerspectiveFovLH(&proj, (float)D3DX_PI * 0.45f, 1024.0f / 768.0f, 1.0f, 100.0f);
//	wvp = world * view * proj;
//
//	//draw triangle
//	g_VertexBuffer->Apply();
//	g_Shader->SetMatrix("gWVP", wvp);
//	g_Shader->Apply(0);
//	g_DeviceContext->Draw(3,0);
//
//	if(FAILED(g_SwapChain->Present( 0, 0 )))
//		return E_FAIL;
//
//	return S_OK;
//}
//
////--------------------------------------------------------------------------------------
//// Called every time the application receives a message
////--------------------------------------------------------------------------------------
//LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
//{
//	PAINTSTRUCT ps;
//	HDC hdc;
//
//	switch (message) 
//	{
//	case WM_PAINT:
//		hdc = BeginPaint(hWnd, &ps);
//		EndPaint(hWnd, &ps);
//		break;
//
//	case WM_DESTROY:
//		PostQuitMessage(0);
//		break;
//
//	case WM_KEYDOWN:
//
//		switch(wParam)
//		{
//			case VK_ESCAPE:
//				PostQuitMessage(0);
//				break;
//		}
//		break;
//
//	default:
//		return DefWindowProc(hWnd, message, wParam, lParam);
//	}
//
//	return 0;
//}
//
//char* FeatureLevelToString(D3D_FEATURE_LEVEL featureLevel)
//{
//	if(featureLevel == D3D_FEATURE_LEVEL_11_0)
//		return "11.0";
//	if(featureLevel == D3D_FEATURE_LEVEL_10_1)
//		return "10.1";
//	if(featureLevel == D3D_FEATURE_LEVEL_10_0)
//		return "10.0";
//
//	return "Unknown";
//}