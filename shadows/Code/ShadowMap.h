#pragma once
#include "stdafx.h"
#include "Shader.h"

using namespace std;

class ShadowMap
{
public:
	ShadowMap(void);
	ShadowMap(ID3D11Device* device, ID3D11DeviceContext* immediateContext, UINT width, UINT height);
	~ShadowMap(void);

	ID3D11ShaderResourceView* DepthMapSRV();

	void outputMerger(ID3D11DeviceContext* deviceContext);

	Shader* getShadowShader();

private:
	UINT width;
	UINT height;

	ID3D11ShaderResourceView* depthMapSRV;
	ID3D11DepthStencilView* depthMapDSV;

	Shader* shadowShader;

	D3D11_VIEWPORT viewPort;

};

