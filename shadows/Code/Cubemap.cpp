#include "Cubemap.h"


Cubemap::Cubemap(void)
{
	for(int i = 0; i < 6; i++)
	{
		cams[i] = NULL;	
	}

	for(int i = 0; i < 6; i++)
	{
		DCMRTV[i] = NULL;	
	}

	cubeTex = NULL;
	DCMSRV = NULL;
	DCMDSV = NULL;
	depthTex = NULL;

}

//create all the textures and stuff here
Cubemap::Cubemap(int cubeSize, D3DXVECTOR3 position, ID3D11Device* device)
{
	this->cubeSize = cubeSize;
	
	//create the Cubemap texture
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = this->cubeSize;
	texDesc.Height = this->cubeSize;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 6;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	device->CreateTexture2D(&texDesc,0,&cubeTex);
	
	//create the RTVs
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;

	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.MipSlice = 0;
	rtvDesc.Texture2DArray.ArraySize = 1;

	for(int i = 0; i < 6; ++i)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		device->CreateRenderTargetView(cubeTex,&rtvDesc,&DCMRTV[i]);
	}

	//create the shader resource view

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;

	device->CreateShaderResourceView(cubeTex, &srvDesc, &DCMSRV);
	
	//create the depththingy
	D3D11_TEXTURE2D_DESC depthTexDesc;
	depthTexDesc.Width = cubeSize;
	depthTexDesc.Height = cubeSize;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	device->CreateTexture2D(&depthTexDesc,0,&depthTex);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;

	dsvDesc.Format = depthTexDesc.Format;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	device->CreateDepthStencilView(depthTex,&dsvDesc,&DCMDSV);


	//viewport
	cubeMapViewport.TopLeftX = 0.0f;
	cubeMapViewport.TopLeftY = 0.0f;
	cubeMapViewport.Width = (float)cubeSize;
	cubeMapViewport.Height = (float)cubeSize;
	cubeMapViewport.MinDepth = 0.0f;
	cubeMapViewport.MaxDepth = 1.0f;

	//create cameras

	float x,y,z;

	D3DXVECTOR3 centerPos = position;
	D3DXVECTOR3 look[6] = 
	{
		D3DXVECTOR3(1.0f, 0, 0), 
		D3DXVECTOR3(-1.0f, 0, 0), 
		D3DXVECTOR3(0, 1.0f, 0), 
		D3DXVECTOR3(0, -1.0f, 0), 
		D3DXVECTOR3(0, 0, 1.0f), 
		D3DXVECTOR3(0, 0, -1.0f), 
	};

	D3DXVECTOR3 up[6] = 
	{
		D3DXVECTOR3(0.0f, 1.0f, 0.0f),
		D3DXVECTOR3(0.0f, 1.0f, 0.0f),
		D3DXVECTOR3(0.0f, 0.0f, -1.0f),
		D3DXVECTOR3(0.0f, 0.0f, 1.0f),
		D3DXVECTOR3(0.0f, 1.0f, 0.0f),
		D3DXVECTOR3(0.0f, 1.0f, 0.0f),
	};

	D3DXVECTOR3 right[6] = 
	{
		D3DXVECTOR3(0.0f, 0.0f, -1.0f),
		D3DXVECTOR3(0.0f, 0.0f, 1.0f),
		D3DXVECTOR3(1.0f, 0.0f, 0.0f),
		D3DXVECTOR3(1.0f, 0.0f, 0.0f),
		D3DXVECTOR3(1.0f, 0.0f, 0.0f),
		D3DXVECTOR3(-1.0f, 0.0f, 0.0f),
	};


	for(int i = 0; i < 6; ++i)
	{
		cams[i] = new Camera(centerPos,look[i],right[i],up[i]);
		cams[i]->setLens(0.5f*PI,1.0f,0.1f,1000.0f);
		cams[i]->updateViewMatrix();
	}
	
}

D3D11_VIEWPORT Cubemap::getViewport()
{
	return this->cubeMapViewport;
}

ID3D11ShaderResourceView* Cubemap::getCubemap()
{
	return this->DCMSRV;
}	

ID3D11DepthStencilView* Cubemap::getStencilView()
{
	return this->DCMDSV;
}

ID3D11RenderTargetView* Cubemap::getRT(int index)
{
	return this->DCMRTV[index];
}

Camera* Cubemap::getCam(int index)
{
	return this->cams[index];
}

void Cubemap::updateCameraPos(D3DXVECTOR3 pos)
{
	for(int i = 0; i < 6; i++)
	{
		cams[i]->setPos(pos);
		cams[i]->updateViewMatrix();
	}
}

Cubemap::~Cubemap(void)
{
	SAFE_RELEASE(this->DCMDSV);
	SAFE_RELEASE(this->DCMSRV);
	for(int i = 0; i < 6; i++)
	{
		SAFE_RELEASE(this->DCMRTV[i]);	
	}
	
	for(int i = 0; i < 6; i++)
	{
		SAFE_DELETE(this->cams[i]);	
	}
}
