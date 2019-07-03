#pragma once
#include<d3d11.h>
#include "CoreType.h"
#include <wrl/client.h>
#include <unordered_map>
using namespace Microsoft::WRL;
XMFLOAT3 XMF3Max(XMFLOAT3 Left, XMFLOAT3 Right);
XMFLOAT3 XMF3Min(XMFLOAT3 Left, XMFLOAT3 Right);
XMFLOAT3 operator+(XMFLOAT3 Left, XMFLOAT3 Right);
XMFLOAT3 operator-(XMFLOAT3 Left, XMFLOAT3 Right);
XMFLOAT3 operator*(XMFLOAT3 Left, XMFLOAT3 Right);
XMFLOAT3 operator/(XMFLOAT3 Left, XMFLOAT3 Right);
XMFLOAT3 operator/(XMFLOAT3 Left, float Right);
XMFLOAT3 operator*(XMFLOAT3 Left, float Right);
XMFLOAT3 operator+(XMFLOAT3 Left, float Right);
XMFLOAT3 operator-(XMFLOAT3 Left, float Right);
 
 
class RenderObject
{
public:
	friend class RenderProxy;
	RenderObject() = default;
	~RenderObject() = default;
    void CreateWallBuffer(FMetisLine *Line);
    void CreateRectBuffer(FMetisRect *Rect);

    void UpdateWallBuffer(FMetisLine *Line);
    void UpdateRectBuffer(FMetisRect *Rect);
private:
	ComPtr<ID3D11Buffer>  VertexBuffer;
	ComPtr<ID3D11Buffer>  Constant;
    bool                                 Show;
};
 
 
struct IRenderProxy
{
	XMFLOAT4	Color;
	XMMATRIX	Transform;
};
struct FBOX
{
       XMFLOAT3	Max;
       XMFLOAT3	Min;
};
class RenderProxy:public IRenderProxy
{
public:
            RenderProxy() {
                        
             }
	~RenderProxy() = default;
    void InitializeProxy();
	        void RegisterProxy(FMetisRegion*Src, FMetisRegion*Dest);
            void RegisterGrid(FMetisRect*MetisRect,unsigned Size);
            void UpdateLayoutFromName(std::string Name, FMetisRect*MetisRect);
            void UpdateGrid(unsigned Index, FMetisRect*MetisRect);
            void Render();
private:
            void UpdateResurce(FMetisRegion*Src, FMetisRegion*Dest);
	        void CompileVS();
	        void CompilePS();          
private:
	ComPtr<ID3D11VertexShader>		VSO=nullptr;
	ComPtr<ID3D11PixelShader>		PSO = nullptr;
 
	D3D11_VIEWPORT					             ViewPort;
	XMMATRIX						                     ViewProj;
	ComPtr<ID3D11InputLayout>		     Layout;
    ComPtr<ID3D11Buffer>                      CBuffer;
private:
	std::unordered_map<std::string, RenderObject>           UnfixedObj;
    using MapType = std::unordered_map<std::string, RenderObject>;
    std::vector<RenderObject>                                             Wall;
    std::vector<RenderObject>                                             FixedObj;
    std::vector<RenderObject>                                             Grid;
};

