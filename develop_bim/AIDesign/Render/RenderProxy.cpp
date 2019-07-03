#include "RenderProxy.h"
#include "D3D11HRI.h"
#include <d3dcompiler.h>
#include <fstream>
 #pragma comment(lib,"d3dcompiler.lib") 
XMFLOAT3 XMF3Max(XMFLOAT3 Left, XMFLOAT3 Right) {
            return XMFLOAT3(max(Left.x, Right.x), max(Left.y, Right.y), max(Left.z, Right.z));
}
XMFLOAT3 XMF3Min(XMFLOAT3 Left, XMFLOAT3 Right) {
            return XMFLOAT3(min(Left.x, Right.x), min(Left.y, Right.y), min(Left.z, Right.z));
}
XMFLOAT3 operator+(XMFLOAT3 Left, XMFLOAT3 Right) {
            return XMFLOAT3(Left.x+Right.x,  Left.y+ Right.y, Left.z+Right.z);
}

XMFLOAT3 operator-(XMFLOAT3 Left, XMFLOAT3 Right) {
            return XMFLOAT3(Left.x - Right.x, Left.y - Right.y, Left.z - Right.z);
}

XMFLOAT3 operator*(XMFLOAT3 Left, XMFLOAT3 Right) {
            return XMFLOAT3(Left.x * Right.x, Left.y * Right.y, Left.z * Right.z);
}

XMFLOAT3 operator/(XMFLOAT3 Left, XMFLOAT3 Right) {
            return XMFLOAT3(Left.x / Right.x, Left.y / Right.y, Left.z / Right.z);
}
XMFLOAT3 operator*(XMFLOAT3 Left, float Right) {
            return XMFLOAT3(Left.x * Right, Left.y * Right, Left.z * Right);
}
XMFLOAT3 operator/(XMFLOAT3 Left, float Right) {
            return XMFLOAT3(Left.x / Right, Left.y / Right, Left.z / Right);
}

XMFLOAT3 operator+(XMFLOAT3 Left, float Right) {
            return XMFLOAT3(Left.x + Right, Left.y +Right, Left.z+ Right);
}
XMFLOAT3 operator-(XMFLOAT3 Left, float Right) {
            return XMFLOAT3(Left.x - Right, Left.y - Right, Left.z- Right);
}
void RenderObject::CreateWallBuffer(FMetisLine*Lines) {
            auto &Line = *Lines;
             XMFLOAT3 vertex[2];
             vertex[0] = XMFLOAT3(Line.Point1.x, Line.Point1.y, 20.f);
             vertex[1] = XMFLOAT3(Line.Point2.x, Line.Point2.y, 20.f);
 
            D3D11_BUFFER_DESC Desc;
            Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            Desc.ByteWidth = sizeof(XMFLOAT3) * 2;
            Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            Desc.MiscFlags = 0;
            Desc.StructureByteStride = sizeof(XMFLOAT3);
            Desc.Usage = D3D11_USAGE_DYNAMIC;
            D3D11_SUBRESOURCE_DATA  Data;
            Data.pSysMem = vertex;
            D3D11HRI::Device->CreateBuffer(&Desc, &Data, VertexBuffer.GetAddressOf());   

            auto Color =XMFLOAT4(1.f, 1.f, 1.f, 1.f);
            Desc.ByteWidth = sizeof(XMFLOAT4);
            Desc.StructureByteStride = Desc.ByteWidth;
            Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            Data.pSysMem = &Color;
            D3D11HRI::Device->CreateBuffer(&Desc, &Data, Constant.GetAddressOf());
}

void RenderObject::CreateRectBuffer(FMetisRect* Rects) {
            auto &Rect = *Rects;
            XMFLOAT3 vertex[6];
            vertex[0] = XMFLOAT3(Rect.LeftTop.x, Rect.LeftTop.y, 10.f);
            vertex[1] = XMFLOAT3(Rect.RightTop.x, Rect.RightTop.y, 10.f);
            vertex[2] = XMFLOAT3(Rect.RightBottom.x, Rect.RightBottom.y, 10.f);

            vertex[3] = XMFLOAT3(Rect.LeftBottom.x, Rect.LeftBottom.y, 10.f);
            vertex[4] = XMFLOAT3(Rect.LeftTop.x, Rect.LeftTop.y, 10.f);
            vertex[5] = XMFLOAT3(Rect.RightBottom.x, Rect.RightBottom.y, 10.f);
            
            D3D11_BUFFER_DESC Desc;
            Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            Desc.ByteWidth = sizeof(XMFLOAT3) * 6;
            Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            Desc.MiscFlags = 0;
            Desc.StructureByteStride = sizeof(XMFLOAT3);
            Desc.Usage = D3D11_USAGE_DYNAMIC;
            D3D11_SUBRESOURCE_DATA  Data;
            Data.pSysMem = vertex;
            D3D11HRI::Device->CreateBuffer(&Desc, &Data, VertexBuffer.GetAddressOf());

            Desc.ByteWidth = sizeof(XMFLOAT4);
            Desc.StructureByteStride = Desc.ByteWidth;
            Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            Data.pSysMem = &Rect.Color;
            D3D11HRI::Device->CreateBuffer(&Desc, &Data, Constant.GetAddressOf());
}

void RenderObject::UpdateWallBuffer(FMetisLine*Lines) {
            auto &Line = *Lines;
            XMFLOAT3 vertex[2];
            vertex[0] = XMFLOAT3(Line.Point1.x, Line.Point1.y, 20.f);
            vertex[1] = XMFLOAT3(Line.Point2.x, Line.Point2.y, 20.f);
            D3D11_MAPPED_SUBRESOURCE Res;
            D3D11HRI::Context->Map(VertexBuffer.Get(), 0, D3D11_MAP_WRITE, 0, &Res);
            memcpy(Res.pData, vertex, sizeof(XMFLOAT3) * 2);
            D3D11HRI::Context->Unmap(VertexBuffer.Get(), 0);
}

void RenderObject::UpdateRectBuffer(FMetisRect* Rects) {
            auto &Rect = *Rects;
            XMFLOAT3 vertex[6];
            vertex[0] = XMFLOAT3(Rect.LeftTop.x, Rect.LeftTop.y, 20.f);
            vertex[1] = XMFLOAT3(Rect.RightTop.x, Rect.RightTop.y, 20.f);
            vertex[2] = XMFLOAT3(Rect.RightBottom.x, Rect.RightBottom.y, 20.f);

            vertex[3] = XMFLOAT3(Rect.LeftBottom.x, Rect.LeftBottom.y, 20.f);
            vertex[4] = XMFLOAT3(Rect.LeftTop.x, Rect.LeftTop.y, 20.f);
            vertex[5] = XMFLOAT3(Rect.RightBottom.x, Rect.RightBottom.y, 20.f);
            D3D11_MAPPED_SUBRESOURCE Res;
            D3D11HRI::Context->Map(VertexBuffer.Get(), 0, D3D11_MAP_WRITE, 0, &Res);
            memcpy(Res.pData, vertex, sizeof(XMFLOAT3) *6);
            D3D11HRI::Context->Unmap(VertexBuffer.Get(), 0);

            D3D11HRI::Context->Map(Constant.Get(), 0, D3D11_MAP_WRITE, 0, &Res);
            memcpy(Res.pData, vertex, sizeof(XMFLOAT4));
            D3D11HRI::Context->Unmap(Constant.Get(), 0);

}
void RenderProxy::InitializeProxy() {
            D3D11_BUFFER_DESC Desc;
            Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            Desc.ByteWidth = sizeof(XMMATRIX);
            Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            Desc.MiscFlags = 0;
            Desc.StructureByteStride = sizeof(XMMATRIX);
            Desc.Usage = D3D11_USAGE_DYNAMIC;
            D3D11HRI::Device->CreateBuffer(&Desc, nullptr, CBuffer.GetAddressOf());
}

void RenderProxy::RegisterProxy(FMetisRegion*Src, FMetisRegion*Dest=nullptr) {
			if(!VSO.Get())
				CompileVS();
			if (!PSO.Get())
				CompilePS();
            UpdateResurce(Src, Dest);
}   

void RenderProxy::UpdateResurce(FMetisRegion*Src, FMetisRegion*Dest) {

	XMFLOAT3 Maxs(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	XMFLOAT3 Mins(FLT_MAX, FLT_MAX, FLT_MAX);
	if (Src)
		for (auto Line : Src->Lines) {
			Maxs = XMF3Max(Maxs, Line.Point1);
			Maxs = XMF3Max(Maxs, Line.Point2);
			Mins = XMF3Min(Mins, Line.Point1);
			Mins = XMF3Min(Mins, Line.Point2);
		}
	if (Dest)
		for (auto Line : Dest->Lines) {
			Maxs = XMF3Max(Maxs, Line.Point1);
			Maxs = XMF3Max(Maxs, Line.Point2);
			Mins = XMF3Min(Mins, Line.Point1);
			Mins = XMF3Min(Mins, Line.Point2);
		}
 
	auto sss = (Maxs + Mins) / 2;
	auto Size = Maxs - Mins;
	//view proj
	XMFLOAT3  Pos = XMFLOAT3(sss.x, sss.y, 0);
	XMVECTOR VPos = XMLoadFloat3(&Pos);
	XMFLOAT3  Look = XMFLOAT3(sss.x, sss.y, 100);
	XMVECTOR VLook = XMLoadFloat3(&Look);
	XMFLOAT3  Up = XMFLOAT3(0, 1, 0);
	XMVECTOR VUp = XMLoadFloat3(&Up);
	auto View = XMMatrixLookAtLH(VPos, VLook, VUp);
	auto maxf = max(Size.x, Size.y);
	//-----------------------------------------------------------------
	this->ViewProj = View * XMMatrixOrthographicLH(maxf+1000, maxf + 1000, 1.f, 10000.f);
	ViewProj = XMMatrixTranspose(ViewProj);
	D3D11_MAPPED_SUBRESOURCE Res;
	D3D11HRI::Context->Map(CBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &Res);
	memcpy(Res.pData, &ViewProj, sizeof(ViewProj));
	D3D11HRI::Context->Unmap(CBuffer.Get(), 0);

	ViewPort.Height = 800;
	ViewPort.Width = 600;
	ViewPort.TopLeftX = 0;
	ViewPort.TopLeftY = 0;
	ViewPort.MinDepth = 0.f;
	ViewPort.MaxDepth = 1.f;
            //Resurce
            UnfixedObj.clear();
            for (auto &t : FixedObj) {
                        t.Show = false;
            }
            for (auto &t : Wall) {
                        t.Show = false;
            }
			for (auto &t : Grid) {
				t.Show = false;
			}
            if (Src&&!Dest) {
                        unsigned Wallsize = 0;
                        unsigned ComponetSize = 0;
						unsigned Layoutsize = 0;
                        for (unsigned i = 0; i < Src->Lines.size(); ++i) {
                                    if (Wall.size()==Wallsize) {
                                                //Create new 
                                                RenderObject  Object;
                                                Object.CreateWallBuffer(&Src->Lines[i]);
                                                Object.Show = true;
                                                Wall.push_back(Object);
                                    }
                                    else {
                                                //update 
                                                Wall[Wallsize].UpdateWallBuffer(&Src->Lines[i]);
                                                Wall[Wallsize].Show = true;
                                    }
                                    ++Wallsize;
                        }
                        for (unsigned i = 0; i < Src->Layouts.size(); ++i) {     
							if (Layoutsize == FixedObj.size()) {
								RenderObject  Object;
								Object.CreateRectBuffer(&Src->Layouts[i]);
								Object.Show = true;

								//UnfixedObj.insert(std::make_pair(Src->Layouts[i].Name, Object));
								FixedObj.push_back(Object);
							}
							else {
								FixedObj[ComponetSize].UpdateRectBuffer(&Src->Layouts[i]);
								FixedObj[ComponetSize].Show = true;
							}
							Layoutsize ++;
                        }
                        for (unsigned i = 0; i < Src->Components.size(); ++i) {
                                    if (FixedObj.size() == ComponetSize) {
                                                //Create new 
                                                RenderObject  Object;
                                                Object.CreateRectBuffer(&Src->Components[i]);
                                                Object.Show = true;
                                                FixedObj.push_back(Object);
                                    }
                                    else {
                                                //update 
                                                FixedObj[ComponetSize].UpdateRectBuffer(&Src->Components[i]);
                                                FixedObj[ComponetSize].Show = true;
                                    }
                                    ++ComponetSize;
                        }
            }
            if (Src&&Dest) {
                        unsigned Wallsize = 0;
                        unsigned ComponetSize = 0;
                        for (unsigned i = 0; i < Src->Lines.size(); ++i) {
                                    if (Wall.size() == Wallsize) {
                                                //Create new 
                                                RenderObject  Object;
                                                Object.CreateWallBuffer(&Src->Lines[i]);
                                                Object.Show = true;
                                                Wall.push_back(Object);
                                    }
                                    else {
                                                //update 
                                                Wall[Wallsize].UpdateWallBuffer(&Src->Lines[i]);
                                                Wall[Wallsize].Show = true;
                                    }
                                    ++Wallsize;
                        }        
                        for (unsigned i = 0; i < Src->Layouts.size(); ++i) {
                                    if (FixedObj.size() == ComponetSize) {
                                                //Create new 
                                                RenderObject  Object;
                                                Object.CreateRectBuffer(&Src->Layouts[i]);
                                                Object.Show = true;
                                                FixedObj.push_back(Object);
                                    }
                                    else {
                                                //update 
                                                FixedObj[ComponetSize].UpdateRectBuffer(&Src->Layouts[i]);
                                                FixedObj[ComponetSize].Show = true;
                                    }
                                    ++ComponetSize;
                        }
                        for (unsigned i = 0; i < Src->Components.size(); ++i) {
                                    if (FixedObj.size() == ComponetSize) {
                                                //Create new 
                                                RenderObject  Object;
                                                Object.CreateRectBuffer(&Src->Components[i]);
                                                Object.Show = true;
                                                FixedObj.push_back(Object);
                                    }
                                    else {
                                                //update 
                                                FixedObj[ComponetSize].UpdateRectBuffer(&Src->Components[i]);
                                                FixedObj[ComponetSize].Show = true;
                                    }
                                    ++ComponetSize;
                        }
                        for (unsigned i = 0; i < Dest->Lines.size(); ++i) {
                                    if (Wall.size() == Wallsize) {
                                                //Create new 
                                                RenderObject  Object;
                                                Object.CreateWallBuffer(&Dest->Lines[i]);
                                                Object.Show = true;
                                                Wall.push_back(Object);
                                    }
                                    else {
                                                //update 
                                                Wall[Wallsize].UpdateWallBuffer(&Dest->Lines[i]);
                                                Wall[Wallsize].Show = true;
                                    }
                                    ++Wallsize;
                        }
                        for (unsigned i = 0; i < Dest->Layouts.size(); ++i) {
                                    RenderObject  Object;
                                    Object.CreateRectBuffer(&Dest->Layouts[i]);
                                    Object.Show = true;
                                  //  UnfixedObj.insert(std::make_pair(Dest->Layouts[i].Name, Object));
									FixedObj.push_back(Object);
                        }
                        for (unsigned i = 0; i < Dest->Components.size(); ++i) {
                                    if (FixedObj.size() == ComponetSize) {
                                                //Create new 
                                                RenderObject  Object;
                                                Object.CreateRectBuffer(&Dest->Components[i]);
                                                Object.Show = true;
                                                FixedObj.push_back(Object);
                                    }
                                    else {
                                                //update 
                                                FixedObj[ComponetSize].UpdateRectBuffer(&Dest->Components[i]);
                                                FixedObj[ComponetSize].Show = true;
                                    }
                                    ++ComponetSize;
                        }

            }
}

void RenderProxy::RegisterGrid(FMetisRect*MetisRect, unsigned Size) {
	unsigned i = 0;
	for (i; i < Size; ++i)
	{
		if (i == Grid.size()) {
			//Create new 
			RenderObject  Object;
			Object.CreateRectBuffer(&MetisRect[i]);
			Object.Show = true;
			Grid.push_back(Object);
		}
		else {
			//update 
			Grid[i].UpdateRectBuffer(&MetisRect[i]);
			Grid[i].Show = true;
		}
	}
 
}

void RenderProxy::CompileVS() {
	std::fstream File;
	File.open("C:\\Users\\40451\\Desktop\\AiDesign\\x64\\Debug\\SOV.cso",std::ios::binary|std::ios::in);
	char max[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, max);
	if (!File.is_open())
		return;
	File.seekg(0, std::ios::end);
	auto Length = File.tellg();
	File.seekg(0, std::ios::beg);
	char* Filebuffer = new char[(size_t)Length];
	File.read(Filebuffer, Length);
	auto hr=D3D11HRI::Device->CreateVertexShader(Filebuffer, (size_t)Length, NULL, VSO.GetAddressOf());
	D3D11_INPUT_ELEMENT_DESC Desc[]{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	D3D11HRI::Device->CreateInputLayout(Desc, 1, Filebuffer, (size_t)Length, Layout.GetAddressOf());
	delete[] Filebuffer;
	File.close();
}

void RenderProxy::CompilePS() {
	std::fstream File;
	File.open("C:\\Users\\40451\\Desktop\\AiDesign\\x64\\Debug\\SOP.cso", std::ios::binary | std::ios::in);
	File.seekg(0, std::ios::end);
	auto Length = File.tellg();
	File.seekg(0, std::ios::beg);
	char* Filebuffer = new char[(size_t)Length];
	File.read(Filebuffer, Length);
	D3D11HRI::Device->CreatePixelShader(Filebuffer, (size_t)Length, NULL, PSO.GetAddressOf());
	delete[] Filebuffer;
	File.close();
}
 
void RenderProxy::Render() {
	auto Context = D3D11HRI::Context;
	Context->OMSetBlendState(nullptr, nullptr, 0xff);
	 Context->ClearDepthStencilView(D3D11HRI::DepthStencilView.Get(),D3D11_CLEAR_DEPTH| D3D11_CLEAR_STENCIL,1.f,0);
     XMFLOAT4 Color = { 0,0,0,1 };
     Context->ClearRenderTargetView(D3D11HRI::RenderTarget.Get(),(float*)&Color.x); 
    
	 Context->OMSetRenderTargets(1, D3D11HRI::RenderTarget.GetAddressOf(), D3D11HRI::DepthStencilView.Get());

	 Context->RSSetState(D3D11HRI::LineState.Get());
	 Context->RSSetViewports(1,&ViewPort);
     Context->IASetInputLayout(Layout.Get());
      Context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
     Context->VSSetShader(VSO.Get(), 0, 0);
     Context->PSSetShader(PSO.Get(), 0, 0);
     const unsigned Offset = 0;
     const unsigned Stride = sizeof(XMFLOAT3);
     //Wall
     for (unsigned i = 0; i < Wall.size(); ++i) {
                 if (Wall[i].Show) {
                             Context->IASetVertexBuffers(0, 1, Wall[i].VertexBuffer.GetAddressOf(), &Stride, &Offset);
                             Context->VSSetConstantBuffers(0, 1, CBuffer.GetAddressOf());
                             Context->PSSetConstantBuffers(0, 1, Wall[i].Constant.GetAddressOf());
                             Context->Draw(2, 0);
                 }
     }
     //Layout
	 Context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	 Context->OMSetBlendState(D3D11HRI::OnAlpha.Get(), nullptr, 0xff);
	 //Context->RSSetState(D3D11HRI::TraingleState.Get());
     for (auto Obj: UnfixedObj) {
                 if (Obj.second.Show) {
                             Context->IASetVertexBuffers(0, 1, Obj.second.VertexBuffer.GetAddressOf(), &Stride, &Offset);
                             Context->VSSetConstantBuffers(0, 1, CBuffer.GetAddressOf() );
                             Context->PSSetConstantBuffers(0, 1, Obj.second.Constant.GetAddressOf());
                             Context->Draw(6, 0);
                 }
     }
     //Component
     for (unsigned i = 0; i < FixedObj.size(); ++i) {
                 if (FixedObj[i].Show) {
						
                             Context->IASetVertexBuffers(0, 1, FixedObj[i].VertexBuffer.GetAddressOf(), &Stride, &Offset);
                             Context->VSSetConstantBuffers(0, 1, CBuffer.GetAddressOf());
                             Context->PSSetConstantBuffers(0, 1, FixedObj[i].Constant.GetAddressOf());
                             Context->Draw(6, 0);
                 }
     }
     //Grid
     for (unsigned i = 0; i < Grid.size(); ++i) {
		 
		 if (Grid[i].Show) {
			 Context->IASetVertexBuffers(0, 1, Grid[i].VertexBuffer.GetAddressOf(), &Stride, &Offset);
			 Context->VSSetConstantBuffers(0, 1, CBuffer.GetAddressOf());
			 Context->PSSetConstantBuffers(0, 1, Grid[i].Constant.GetAddressOf());
			 Context->Draw(6, 0);
		 }
		 else
			 int a = 1;
     }

     D3D11HRI::SwapChain->Present(0, 0);
 
}

void RenderProxy::UpdateLayoutFromName(std::string Name, FMetisRect*MetisRect) {
            MapType::iterator Sign = UnfixedObj.find(Name);
            if (Sign == UnfixedObj.end()) {
                        return;
            }
            Sign->second.UpdateRectBuffer(MetisRect);
}

void RenderProxy::UpdateGrid(unsigned Index, FMetisRect*MetisRect) {

}
 