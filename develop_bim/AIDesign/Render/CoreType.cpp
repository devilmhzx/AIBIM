#include "CoreType.h"

//FMetisRegion
FMetisRegion::FMetisRegion(const std::vector<XMFLOAT3>& InPoints)
{
 
	//计算墙线
	for (size_t i = 0; i < InPoints.size(); i++)
	{
		 XMFLOAT3	A;
         XMFLOAT3	B;
		//最后一个点
		if (i == InPoints.size() - 1)
		{
			A = InPoints[i];
			B = InPoints[0];
		}
		//非最后一个点
		else
		{
			A = InPoints[i];
			B = InPoints[i + 1];
		}
		FMetisLine  Line =  FMetisLine(A, B);
		Lines.push_back(Line);
	}
}

std::string FMetisRegion::GetRegionName()
{
	return RegionName;
}

void FMetisRegion::AddLayout( FMetisLayout  *InNewLayout)
{
	Layouts.push_back(*InNewLayout);
}

void FMetisRegion::AddComponent(FMetisLayout *InNewComponent)
{
	Components.push_back(*InNewComponent);
}


