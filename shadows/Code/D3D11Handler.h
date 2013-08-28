#ifndef D3D11HANDLER_H
#define D3D11HANDLER_H

#include "stdafx.h"
#include "WinHandler.h"
#include "Shader.h"

class D3D11Handler : public WinHandler
{
	protected:
		D3D11_VIEWPORT           viewPort;
		ID3D11RenderTargetView  *backBufferRTV;
		ID3D11Texture2D         *backBufferDS; // ???
		ID3D11DepthStencilView  *backBufferDSV; //????

		ID3D11Texture2D          **RTs;
		ID3D11RenderTargetView   **RTVs;
		ID3D11DepthStencilView   *DSV;

		int nrOfTargets;
		int nrOfShaders;
		std::vector<Shader*>     shaders;
		Shader*					shader;

	protected:
		ID3D11ShaderResourceView **SRVs;
		ID3D11ShaderResourceView *depthStencil;
		IDXGISwapChain*         swapChain;	
		ID3D11Device*			device;
		ID3D11DeviceContext*	deviceContext;
		char* FeatureLevelToString(D3D_FEATURE_LEVEL featureLevel);

	public:
		D3D11Handler();
		~D3D11Handler();
		HRESULT InitDirect3D();
		HRESULT initResources(int screenWidth, int screenHeight);
		Shader* setPass(int nr);
		void clearAndBindTarget();
		ID3D11ShaderResourceView* debugGetSRV(int id);
		void resetRT();
};

#endif