#pragma once
#include <wrl/client.h>
#include <dxgi.h>
#include <d3d11.h>
using namespace Microsoft::WRL;
class D3D11HRI
{
public:
	D3D11HRI() = default;
	~D3D11HRI() = default;
public:
	static bool InitializeHRI(HWND hWnd,unsigned Height,unsigned Width);
	static ComPtr<ID3D11Device>			Device;
	static ComPtr<ID3D11DeviceContext>		Context;
	static ComPtr<IDXGISwapChain>			SwapChain;
	static ComPtr<ID3D11RenderTargetView>	RenderTarget;
	static ComPtr<ID3D11DepthStencilView>	DepthStencilView;
    static  ComPtr<ID3D11RasterizerState>		LineState;
    static ComPtr<ID3D11RasterizerState>		TraingleState;
	static ComPtr<ID3D11BlendState>			OnAlpha;
};

 