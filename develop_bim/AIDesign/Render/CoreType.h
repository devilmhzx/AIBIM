#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>
using namespace DirectX;
 
/** 墙线 */
struct  FMetisLine
{
	FMetisLine() = default;
	FMetisLine(XMFLOAT3   InPoint1, XMFLOAT3   InPoint2)
	{
		Point1= InPoint1;
		Point2= InPoint2;
	}
    XMFLOAT3 Point1;
    XMFLOAT3 Point2;
};

/** 构件基类 */
struct   FMetisRect
{
	XMFLOAT3	LeftTop;
	XMFLOAT3	LeftBottom;
	XMFLOAT3	RightTop;
	XMFLOAT3	RightBottom;
	float			    Rotate;
	XMFLOAT2	Translation;
	XMFLOAT4	Color;
	float			    Width;
	float			    Height;
	float			    Depth;
};
/** 布局基类 */
enum   FMetisLayoutType
{
	UNDEFINE,
	SUBWALL,
	//FIXED
	WINDOWS,
	DOOR,
	//UNFIXED
	TV,
	BED,
	CUPBOARD,
	CHAIR,
	DESK
};
struct  FMetisLayout : public FMetisRect
{
	FMetisLayout(float InWidth, float InDepth, float InZValue) {

	}
	int				                Priority;
	std::string 		            Name;
	FMetisLayoutType		Type;
	std::vector<int>		    AttachLayout;
};

/** 闭合区域 */
class   FMetisRegion 
{
public:
	FMetisRegion() = default;
	FMetisRegion(const std::vector<XMFLOAT3>& InPoints);
	//墙线列表
	std::vector<FMetisLine> Lines;
	std::string GetRegionName();
	void AddLayout(FMetisLayout* InNewLayout);
	void AddComponent(FMetisLayout *InNewComponent);
	//所包含的家具布局
	std::vector<FMetisRect> Layouts;
	//所包含的门窗组件
	std::vector<FMetisRect> Components;
	//区域名称，由区域索引和区域类型来确定
	std::string RegionName;
};