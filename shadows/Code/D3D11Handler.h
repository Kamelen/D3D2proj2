#ifndef D3D11HANDLER_H
#define D3D11HANDLER_H

#include "stdafx.h"
#include "WinHandler.h"
#include "Shader.h"

class D3D11Handler : public WinHandler
{
	private:


	protected:
		IDXGISwapChain*         swapChain;	
		ID3D11RenderTargetView* renderTargetView;	
		ID3D11Texture2D*        depthStencil; 
		ID3D11DepthStencilView* depthStencilView;
		ID3D11Device*			device;
		ID3D11DeviceContext*	deviceContext;
		Shader*					shader;
		D3D11_VIEWPORT          viewPort;
		char* FeatureLevelToString(D3D_FEATURE_LEVEL featureLevel);

	public:
		D3D11Handler();
		~D3D11Handler();
		HRESULT InitDirect3D();
		void resetRT();
};

#endif