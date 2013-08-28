#include "D3D11Handler.h"

D3D11Handler::D3D11Handler()
:WinHandler()
{
	this->swapChain     = NULL;
	this->backBufferRTV = NULL;
	this->backBufferDS  = NULL;
	this->backBufferDSV = NULL;
	this->RTs           = NULL;
	this->RTVs          = NULL;
	this->SRVs          = NULL;
	this->DSV          = NULL;
	this->swapChain     = NULL;
	this->device        = NULL;
	this->deviceContext = NULL;
	this->nrOfTargets   = 3;
	this->nrOfShaders   = 3;
}

D3D11Handler::~D3D11Handler()
{
	if(this->swapChain)
	{
		this->swapChain->SetFullscreenState(false, NULL);
		this->swapChain->Release();
		this->swapChain = NULL;
	}

	SAFE_RELEASE(this->backBufferRTV);
	SAFE_RELEASE(this->backBufferDS);
	SAFE_RELEASE(this->backBufferDSV);

	for(int i = 0; i < this->nrOfTargets; i++)
	{
		SAFE_RELEASE(this->RTs[i]);
	}
	SAFE_DELETE(this->RTs);

	for(int i = 0; i < this->nrOfTargets; i++)
	{
		SAFE_RELEASE(this->RTVs[i]);
	}
	SAFE_DELETE(this->RTVs);

	for(int i = 0; i < this->nrOfTargets; i++)
	{
		SAFE_RELEASE(this->SRVs[i]);
	}
	SAFE_DELETE(this->SRVs);

	if(this->shader)
	{
		delete this->shader;
	}
	
	SAFE_RELEASE(this->DSV);
	SAFE_RELEASE(this->device);
	SAFE_RELEASE(this->deviceContext);

}

HRESULT D3D11Handler::initResources(int screenWidth, int screenHeight)
{
	D3D11_TEXTURE2D_DESC texDesc;	
	texDesc.Width				= screenWidth;
	texDesc.Height				= screenHeight;
	texDesc.MipLevels			= 1;
	texDesc.ArraySize			= 1;
	texDesc.Format				= DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;
	texDesc.Usage				= D3D11_USAGE_DEFAULT;
	texDesc.BindFlags			= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags		= 0;
	texDesc.MiscFlags			= 0;

	this->RTs	        = new ID3D11Texture2D *[this->nrOfTargets + 1];
	this->RTVs		    = new ID3D11RenderTargetView *[this->nrOfTargets + 1];
	this->SRVs		    = new ID3D11ShaderResourceView *[this->nrOfTargets + 1];
	
	for(int i = 0; i < this->nrOfTargets + 1; i++)
	{
		if(i == nrOfTargets)
		{
			texDesc.Format = DXGI_FORMAT_R32_FLOAT;

		}
		if(FAILED(this->device->CreateTexture2D(&texDesc, NULL, &this->RTs[i]))) return false;

		if(FAILED(this->device->CreateRenderTargetView(this->RTs[i], NULL, &this->RTVs[i]))) return false;

		if(FAILED(this->device->CreateShaderResourceView(this->RTs[i], NULL, &this->SRVs[i]))) return false;
	}

	return true;
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

	hr = this->device->CreateRenderTargetView( pBackBuffer, NULL, &this->backBufferRTV );
	pBackBuffer->Release();
	if( FAILED(hr) )
		return hr;


	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = screenWidth;
	descDepth.Height = screenHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = this->device->CreateTexture2D( &descDepth, NULL, &this->backBufferDS );
	if( FAILED(hr) )
		return hr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = this->device->CreateDepthStencilView( this->backBufferDS, &descDSV, &this->backBufferDSV);
	if( FAILED(hr) )
		return hr;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	SRVDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels			= 1;
	SRVDesc.Texture2D.MostDetailedMip	= 0;
	if(FAILED(this->device->CreateShaderResourceView(this->backBufferDS, &SRVDesc, &this->depthStencil))) return false;

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

	this->shaders.push_back(new Shader());
	if(FAILED(this->shaders[0]->Init(this->device, this->deviceContext, "../Shaders/Geometry.fx", inputDesc, 3)))
	{
		return E_FAIL;
	}

	D3D11_INPUT_ELEMENT_DESC lightInputDesc[] = {
		{"POSITION" , 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 0 , D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"LIGHTPOS" , 0 , DXGI_FORMAT_R32G32B32_FLOAT, 1 , 0 , D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"LIGHTCOLOR" , 0 , DXGI_FORMAT_R32G32B32_FLOAT, 1 , 12 , D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"LIGHTRADIUS" , 0 , DXGI_FORMAT_R32_FLOAT, 1 , 24 , D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};
	
	this->shaders.push_back(new Shader());
	if(FAILED(this->shaders[1]->Init(this->device, this->deviceContext, "../Shaders/PointLight.fx", lightInputDesc, 4)))
	{
		return E_FAIL;
	}
	

	this->shaders.push_back(new Shader());
	if(FAILED(this->shaders[2]->Init(this->device, this->deviceContext, "../Shaders/FullScreenQuad.fx", inputDesc, 3)))
	{
		return E_FAIL;
	}

		D3D11_INPUT_ELEMENT_DESC inputDesc2[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	shader = new Shader();
	if(FAILED(this->shader->Init(this->device, this->deviceContext, "../Shaders/BasicShadows.fx", inputDesc2, 3)))
	{
		return E_FAIL;
	}

	initResources(screenWidth, screenHeight); 

	return S_OK;
}
void D3D11Handler::clearAndBindTarget()
{
	static float depthClear[4] = { 1.0f , 1.0f, 1.0f, 1.0f };
	static float colorClear[4] = { 0.0f , 0.0f, 0.0f, 1.0f };
	static float normalClear[4] = { 0.5f , 0.5f, 0.5f, 1.0f };

	this->deviceContext->ClearRenderTargetView(this->backBufferRTV, colorClear);
	this->deviceContext->ClearDepthStencilView(this->backBufferDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);



	this->deviceContext->ClearRenderTargetView(this->RTVs[0], colorClear);
	this->deviceContext->ClearRenderTargetView(this->RTVs[1], normalClear);
	this->deviceContext->ClearRenderTargetView(this->RTVs[2], colorClear);
	this->deviceContext->ClearRenderTargetView(this->RTVs[3], depthClear);
}

Shader* D3D11Handler::setPass(int nr)
{
	switch(nr)
	{
		case 0:
			this->deviceContext->OMSetRenderTargets(this->nrOfTargets + 1, this->RTVs, this->backBufferDSV);
			return this->shaders.at(nr);
		break;

		case 1:
			this->deviceContext->OMSetRenderTargets(1, &this->RTVs[2] , NULL);
			return this->shaders.at(nr);
			//this->deviceContext->OMSetRenderTargets(1, this->RTVs[3], NULL);
			/*
				set normalmap, diffuse och depthmap
			*/
		break;

		case 2:
			this->deviceContext->OMSetRenderTargets(1, &this->backBufferRTV, NULL);
			return this->shaders.at(nr);
			/*
				set lightmap, diffuse, normal map;
			*/
		break;
	}
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
	this->deviceContext->OMSetRenderTargets( 1, &this->backBufferRTV, this->backBufferDSV );

	this->deviceContext->RSSetViewports( 1, &viewPort );
}