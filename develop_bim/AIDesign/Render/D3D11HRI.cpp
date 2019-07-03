#include "D3D11HRI.h"
#pragma comment(lib,"d3d11.lib") 
#pragma comment(lib,"dxgi.lib") 
ComPtr<ID3D11Device>                    D3D11HRI::Device = ComPtr<ID3D11Device>(NULL);
ComPtr<ID3D11DeviceContext>		D3D11HRI::Context = ComPtr<ID3D11DeviceContext>();
ComPtr<IDXGISwapChain>			    D3D11HRI::SwapChain = ComPtr<IDXGISwapChain>();
ComPtr<ID3D11RenderTargetView>	D3D11HRI::RenderTarget = ComPtr<ID3D11RenderTargetView>();
ComPtr<ID3D11DepthStencilView>	D3D11HRI::DepthStencilView = ComPtr<ID3D11DepthStencilView>();
   ComPtr<ID3D11RasterizerState>	D3D11HRI::LineState= ComPtr<ID3D11RasterizerState>();
  ComPtr<ID3D11RasterizerState>		D3D11HRI::TraingleState= ComPtr<ID3D11RasterizerState>();
  ComPtr<ID3D11BlendState>			D3D11HRI::OnAlpha = ComPtr<ID3D11BlendState>();
bool D3D11HRI::InitializeHRI(HWND hWnd, unsigned Height, unsigned Width) {

	ComPtr<IDXGIFactory>	Factory;
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)Factory.GetAddressOf());
	if (!Factory.Get())
		return false;
	ComPtr<IDXGIAdapter>	Adapter;
	unsigned Index = 0;
	unsigned order = -1;
	unsigned	MaxMemory = 0;
	while (Factory->EnumAdapters(Index, Adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC  pDesc;
		Adapter->GetDesc(&pDesc);
		if (pDesc.DedicatedVideoMemory > MaxMemory){
			MaxMemory = (unsigned)pDesc.DedicatedVideoMemory;
			order = Index;
		}
		Index++;
		Adapter.Reset();
	}
	Factory->EnumAdapters(order, Adapter.GetAddressOf());
	//Device
	D3D_FEATURE_LEVEL featureLevels[] = {

		D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL	curLevel;
	UINT Flag = D3D11_CREATE_DEVICE_DEBUG;
	if (D3D11CreateDevice(
		Adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN,0 , Flag,
		featureLevels, 1, D3D11_SDK_VERSION,
		D3D11HRI::Device.GetAddressOf(),
		&curLevel, D3D11HRI::Context.GetAddressOf()) != S_OK)
		return false;
	//SwapChain
	DXGI_SWAP_CHAIN_DESC scDesc = { 0 };		
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		
	scDesc.BufferDesc.Width = Width;			
	scDesc.BufferDesc.Height = Height;
	scDesc.BufferDesc.RefreshRate.Numerator = 60;		
	scDesc.BufferDesc.RefreshRate.Denominator = 1;
	scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;			//固定参数
	scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	//固定参数
	scDesc.BufferCount = 1;				
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		//Usage为Render Target Output
	scDesc.Flags = 0;
	scDesc.OutputWindow = hWnd;		
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality =0;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;	
	scDesc.Windowed = true;
	
	if (Factory->CreateSwapChain(Device.Get(), &scDesc, SwapChain.GetAddressOf()) != S_OK)
		return false;
	Factory.Reset();
	Adapter.Reset();
 
	ComPtr<ID3D11Texture2D>	Texture;
	SwapChain->GetBuffer(0,__uuidof(ID3D11Texture2D),(void**)Texture.GetAddressOf());


	Device->CreateRenderTargetView(Texture.Get(), NULL, RenderTarget.GetAddressOf());


	ComPtr<ID3D11Texture2D> Texure2D;
	D3D11_TEXTURE2D_DESC DescRes = { 0 };
	DescRes.ArraySize = 1;
	DescRes.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DescRes.CPUAccessFlags = 0;
	DescRes.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DescRes.Height = Height;
	DescRes.Width = Width;
	DescRes.MipLevels = 1;
	DescRes.MiscFlags = 0;
	DescRes.SampleDesc.Count = 1;
	DescRes.SampleDesc.Quality = 0;
	DescRes.Usage = D3D11_USAGE_DEFAULT;
	
	Device->CreateTexture2D(&DescRes,nullptr,Texure2D.GetAddressOf());

	D3D11_DEPTH_STENCIL_VIEW_DESC Desc;
	Desc.Flags = 0;
	Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	Desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	Desc.Texture2D.MipSlice = 0;
	Device->CreateDepthStencilView(Texure2D.Get(),&Desc, DepthStencilView.GetAddressOf());

    D3D11_RASTERIZER_DESC rDesc = {};
    rDesc.CullMode = D3D11_CULL_NONE;
    rDesc.FillMode = D3D11_FILL_SOLID;
    rDesc.DepthClipEnable = true;
    Device->CreateRasterizerState(&rDesc, LineState.GetAddressOf());

	D3D11_BLEND_DESC bDesc;
	bDesc.AlphaToCoverageEnable = false;
	bDesc.IndependentBlendEnable = false;
	bDesc.RenderTarget[0].BlendEnable = true;
	bDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	Device->CreateBlendState(&bDesc, OnAlpha.GetAddressOf());
	return true;
}
 