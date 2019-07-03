#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>
using namespace DirectX;
 
/** ǽ�� */
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

/** �������� */
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
/** ���ֻ��� */
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

/** �պ����� */
class   FMetisRegion 
{
public:
	FMetisRegion() = default;
	FMetisRegion(const std::vector<XMFLOAT3>& InPoints);
	//ǽ���б�
	std::vector<FMetisLine> Lines;
	std::string GetRegionName();
	void AddLayout(FMetisLayout* InNewLayout);
	void AddComponent(FMetisLayout *InNewComponent);
	//�������ļҾ߲���
	std::vector<FMetisRect> Layouts;
	//���������Ŵ����
	std::vector<FMetisRect> Components;
	//�������ƣ�����������������������ȷ��
	std::string RegionName;
};