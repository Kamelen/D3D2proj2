#pragma once
#include "stdafx.h"
#include "Camera.h"

using namespace std;

class Cubemap
{
public:
	Cubemap(void);
	Cubemap(int cubeSize, D3DXVECTOR3 position, ID3D11Device* device); //create cameras

	~Cubemap(void);

	void updateCubemap(); // update cameras if needed and render the 6 different viewpoint
	ID3D11ShaderResourceView* getCubemap(); // gets the cubemap so that we can send it to the GPU
	ID3D11DepthStencilView* getStencilView();
	int getCubeSize(); // BLAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

	D3D11_VIEWPORT getViewport();
	ID3D11RenderTargetView* getRT(int index);
	Camera* getCam(int index);

	void updateCameraPos(D3DXVECTOR3 pos);

private:
	int cubeSize; // both for x and y
	D3DXVECTOR3 pos;
	
	Camera* cams[6]; // array of 6 cameras

	ID3D11Texture2D* cubeTex;
	ID3D11RenderTargetView* DCMRTV[6];

	ID3D11ShaderResourceView* DCMSRV;
	
	ID3D11DepthStencilView* DCMDSV;
	ID3D11Texture2D* depthTex;

	D3D11_VIEWPORT cubeMapViewport;

};

