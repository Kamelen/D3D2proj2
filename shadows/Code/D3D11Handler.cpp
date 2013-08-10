#include "D3D11Handler.h"

D3D11Handler::D3D11Handler()
:WinHandler()
{
	this->swapChain = NULL;
	this->renderTargetView = NULL;
	this->depthStencil = NULL;
	this->depthStencilView = NULL;
	this->device = NULL;
	this->deviceContext = NULL;
}

D3D11Handler::~D3D11Handler()
{
	if(this->swapChain)
	{
		this->swapChain->SetFullscreenState(false, NULL);
		this->swapChain->Release();
		this->swapChain = NULL;
	}

	if(this->renderTargetView)
	{
		this->renderTargetView->Release();
		this->renderTargetView = NULL;
	}

	if(this->depthStencil)
	{
		this->depthStencil->Release();
		this->depthStencil = NULL;
	}

	if(this->depthStencilView)
	{
		this->depthStencilView->Release();
		this->depthStencilView = NULL;
	}

	if(this->device)
	{
		this->device->Release();
		this->device = NULL;
	}

	if(this->deviceContext)
	{
		this->deviceContext->Release();
		this->deviceContext = NULL;
	}

	if(this->shader)
	{
		delete this->shader;
	}
}


HRESULT D3D11Handler::InitDirect3D()
{
	HRESULT hr = S_OK;
	RECT rec;
	GetClientRect(this->hWnd, &rec);

	int screenWidth = rec.right - rec.left;
	int screenHeight = rec.bottom - rec.top;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	D3D_DRIVER_TYPE driverType;
	
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE
	};

	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = screenWidth;
	sd.BufferDesc.Height = screenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL featureLevelsToTry[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	D3D_FEATURE_LEVEL initiatedFeatureLevel;

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			driverType,
			NULL,
			createDeviceFlags,
			featureLevelsToTry,
			ARRAYSIZE(featureLevelsToTry),
			D3D11_SDK_VERSION,
			&sd,
			&this->swapChain,
			&this->device,
			&initiatedFeatureLevel,
			&this->deviceContext);

		if( SUCCEEDED( hr ) )
		{
			char title[256];
			sprintf_s(
				title,
				sizeof(title),
				"BTH - Direct3D 11.0 Template | Direct3D 11.0 device initiated with Direct3D %s feature level",
				FeatureLevelToString(initiatedFeatureLevel)
			);
			SetWindowText(this->hWnd, title);

			break;
		}
	}
	if( FAILED(hr) )
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer;
	hr = this->swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&pBackBuffer );
	if( FAILED(hr) )
		return hr;

	hr = this->device->CreateRenderTargetView( pBackBuffer, NULL, &renderTargetView );
	pBackBuffer->Release();
	if( FAILED(hr) )
		return hr;


	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = screenWidth;
	descDepth.Height = screenHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = this->device->CreateTexture2D( &descDepth, NULL, &this->depthStencil );
	if( FAILED(hr) )
		return hr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = this->device->CreateDepthStencilView( this->depthStencil, &descDSV, &this->depthStencilView );
	if( FAILED(hr) )
		return hr;


	this->deviceContext->OMSetRenderTargets( 1, &this->renderTargetView, this->depthStencilView );

	// Setup the viewport

	viewPort.Width = (float)screenWidth;
	viewPort.Height = (float)screenHeight;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	this->deviceContext->RSSetViewports( 1, &viewPort );


	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	shader = new Shader();
	if(FAILED(this->shader->Init(this->device, this->deviceContext, "../Shaders/BasicShadows.fx", inputDesc, 3)))
	{
		return E_FAIL;
	}

	return S_OK;
}


char* D3D11Handler::FeatureLevelToString(D3D_FEATURE_LEVEL featureLevel)
{
	if(featureLevel == D3D_FEATURE_LEVEL_11_0)
		return "11.0";
	if(featureLevel == D3D_FEATURE_LEVEL_10_1)
		return "10.1";
	if(featureLevel == D3D_FEATURE_LEVEL_10_0)
		return "10.0";

	return "Unknown";
}

void D3D11Handler::resetRT()
{
	this->deviceContext->OMSetRenderTargets( 1, &this->renderTargetView, this->depthStencilView );

	this->deviceContext->RSSetViewports( 1, &viewPort );
}