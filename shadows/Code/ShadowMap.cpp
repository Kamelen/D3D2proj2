#include "ShadowMap.h"


ShadowMap::ShadowMap(void)
{
	width = 0;
	height = 0;

	depthMapSRV = NULL;
	depthMapDSV = NULL;
}

ShadowMap::ShadowMap(ID3D11Device* device, ID3D11DeviceContext* immediateContext, UINT width, UINT height)
{
	this->width = width;
	this->height = height;
	this->depthMapSRV = NULL;
	this->depthMapDSV = NULL;


	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	shadowShader = new Shader();
	this->shadowShader->Init(device, immediateContext, "../Shaders/shadowMapping.fx", inputDesc, 1);


	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;
	viewPort.Width = static_cast<float>(width);
	viewPort.Height = static_cast<float>(height);
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* depthMap = 0;
	device->CreateTexture2D(&texDesc,0,&depthMap);
	
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	
	device->CreateDepthStencilView(depthMap, &dsvDesc, &depthMapDSV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(depthMap,&srvDesc,&depthMapSRV);

	depthMap->Release();
}

ShadowMap::~ShadowMap(void)
{
}


ID3D11ShaderResourceView* ShadowMap::DepthMapSRV()
{
	return this->depthMapSRV;
}

void ShadowMap::outputMerger(ID3D11DeviceContext* deviceContext)
{
	deviceContext->RSSetViewports(1, &viewPort);

	ID3D11RenderTargetView* renderTargets[1] = {0};
	deviceContext->OMSetRenderTargets(1,renderTargets,depthMapDSV);

	deviceContext->ClearDepthStencilView(depthMapDSV, D3D11_CLEAR_DEPTH,1.0f,0);
}

Shader* ShadowMap::getShadowShader()
{
	return this->shadowShader;
}


