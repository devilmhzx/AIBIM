/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  Author:fengxiaoyi
*  Create Date:2018-11.10
*  Description:卫生间设计
**************************************************/
#pragma once
#include "../SingleCase.h"
#include <set>
/* 定义模型的内部类型 */


#define		Commode_Extend    5.f
#define     DynanmicLayoutStep 10.f
#define     Commode_Dis 50
#define     Toilet_Area 4
#define     Toilet_Area 4
#define	    Sprinkler_Extend    20.f

/**花洒和淋浴房替换*/
//#define		Model_Sprinkler_Id	    30012
//#define		Model_Bathroom_Id	    30006
//#define		Model_Bathroom_Width	    80
//#define		Model_Bathroom_Length	    80

#define		Model_Inner_Type	    -1

struct BoundingBox
{
	Point3f Max;
	Point3f	Min;
	Point3f Direction;
	Model	model;
	Wall*	Wall;
	int		Inner_type;
	Point3f location;
	bool flag;
};

struct CornerData
{
	Corner corner;
	float distance;
};

bool operator<(const BoundingBox&left, const BoundingBox&right);

class ToiletAutomaticlDesign
{
public:
	ToiletAutomaticlDesign();
	~ToiletAutomaticlDesign();
public:
	 


	using  ToiletDesign = std::vector<std::set<BoundingBox>>;
	using  BoundingBoxSet= std::set<BoundingBox>;
public:
	bool AutoDesign(SingleCase*single_case);
private:
	// 初始化门的布局
	void InitLayoutDoor();
	//布局马桶
	void LayoutCommode();
	// 获得距离门较远的墙角
	vector<CornerData> GetFastCornerDataWithDoor();
	// 获得距离门较近的墙角
	vector<CornerData> GetNearCornerDataWithDoor();
	//布局花洒
	void LayoutSprinkler();
	//布局浴房
	void LayoutNormalBathroom();
	//布局一字浴房
	void LayoutBathroomWithLineShape();
	//布局浴柜
	void LayoutCabinet();
	//布局浴缸
	void LayoutBathtub();
	// 检测布局是否与门进行了碰撞
	bool IsCollisionDoor(BoundingBox box);
	// 扩展布局进行有效布局的组合
	void ExpandLayout();
	// 生成含浴缸的布局
	void ExpandBathtubLayout();
	// 判定浴缸在布局中是否合适
	bool CheckBathtubLayout(BoundingBoxSet&single_layout,BoundingBox bathtub);
	//布局洗衣机
	void LayoutWasher(ToiletDesign &ref);
	//沿着墙体自动布局洗衣机
	bool DynamicLayoutWasher(BoundingBoxSet&single_layout,Wall*wall,Model model);
	//沿着角落自动布局洗衣机
	bool DynamicLayoutWasherByCorner(BoundingBoxSet&single_layout, Corner*corner, Model model);
	// 生成结果
	void FillModelToSingleCase ();
	//
	void FillModelToModelLayout(ModelLayout&layout, BoundingBoxSet&Set);
	// 查找可布局的模型
	Model FindLayoutModel(int inner_type);
	// 通过墙体计算门的布局
	BoundingBox CalculateDoorByWall(Door*door, Wall*wall);
	// 通过墙体的点计算马桶的box
	BoundingBox CalculateCommodeByWall(Point3f stool,Model model, Wall*wall);
	// 通过墙体的点计算花洒的box
	BoundingBox CalculateSprinklerByWall(Corner*corner, Model model, Wall*wall);
	// 通过墙体的点计算淋浴房的box
	BoundingBox CalculateNormalBathroomByWall(Corner*corner, Model model, Wall*wall);
	// 通过墙体的点计算一字型淋浴房的box
	BoundingBox CalculateBathroomWithLineShape(Corner*corner, Model model, Wall*wall);
	// 通过墙角计算浴室柜的box
	BoundingBox CalculateCabinetByCorner(Point3f corner_point, Point3f end, Model model, Wall*wall);
	// 通过位置坐标计算浴室柜的box
	BoundingBox CalculateCabinetByPoint(Point3f beg, Point3f end, Model model, Wall*wall);
	// 通过墙体的点计算浴缸的box
	BoundingBox CalculateBathtubByWall(Corner *corner, Model model, Wall*wall);
	//单独布局洗衣机
	void AloneLayoutWasher(Model model);
	// 计算墙体的方向向量
	Point3f CalculateWallNormal(Wall*wall);
	// 对点数据按照距离排序
	vector<CornerData> SortCornerData(vector<CornerData> src_data, int sort_type);
	// 两个布局是否碰撞
	bool IsCollisionBoundingBox(BoundingBox src_box, BoundingBox des_box);
	// 得到一套虚拟的布局方案,为了组装使用
	BoundingBox GetVirtualBox(int inner_type);
	// 复制一套新的布局
	set<BoundingBox> CopyBoundingBoxSet(BoundingBoxSet src_set);
private:
	 
	std::vector<BoundingBox>		door_layout;  // 门的布局
	std::vector<BoundingBox>		commode_layout; // 马桶布局
	std::vector<BoundingBox>		sprinkler_layout; // 花洒布局
	std::vector<BoundingBox>		bathroom_layout; // 淋浴房布局
	std::vector<BoundingBox>		cabinet_layout;  // 浴室柜布局
	std::vector<BoundingBox>		bathtub_layout;  // 浴缸的布局

	ToiletDesign								case_layout;
	SingleCase*								m_single_case;
};

