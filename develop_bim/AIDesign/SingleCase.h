/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:单房间设计数据
*  简要描述:
*
*  当前版本:
*  作者:常远
*  创建日期:2018-09-05
*  说明:
*
*  当前版本:V1.0.1
*  作者:常远
*  创建日期:2018-12-04
*  说明:添加入户门、新增区域转换成singlecase
**************************************************/
#pragma once
#include "Wall.h"
#include "Window.h"
#include "Door.h"
#include "Room.h"
#include "Model.h"
#include "AirPillarBaseJ.h"
#include "FloorBox.h"
#include "WallBox.h"
#include "ComStruct.h"
#include "CombinedGeneration.h"
#include "RoomRegion.h"
#include "ComponentObject.h"

// 点信息结构体
struct CornerInfo
{
	Corner corner;  // 点对象
	bool is_old; // 是否是旧点
};

class SingleCase
{
public:
	SingleCase();
	virtual ~SingleCase();

	// 初始化数据
	void Init(SingleCase &single);
	// 获得与墙角相关的墙体
	vector<Wall*> GetWallsByCorner(Corner *corner);
	// 墙体是否与门相对
	bool  IsWallFaceToDoor(Wall *wall);
	// 模拟生成卫生间马桶点位
	void CreateFloorboxOfStool();
	/*****************************************************************
	* 函数名：CreateCombinedModel
	* 函数描述：创建模型的组合
	*****************************************************************/
	void CreateCombinedModel();
	/*****************************************************************
	* 函数名：AnalyzeCombinedModel
	* 函数描述：解析模型的组合（布局后）
	*****************************************************************/
	void AnalyzeCombinedModel();
	// 区域转化为房间
	SingleCase GetNewSingleCaseByRegion(RoomRegion region);
	// 是否是区域布局
	bool IsRegionLayout();
	// 根据空间类型获得对应的region
	RoomRegion GetRegionById(int region_id);
	// 获得布局的区域
	RoomRegion GetLayoutRegion();
private:
	// 通过墙体获得有关的两个墙体
	vector<Wall*> SingleCase::GetRelatedWallListByWall(Wall *p_wall);
	// 计算墙体的朝向向量
	Point3f CalculateWallNormal(Wall*wall);
	// 通过坐标点获得corner,仅供区域转换使用
	Corner GetCornerByPoint(Point3f point);
	// 通过墙获得墙,仅供区域转换使用
	Wall GetWallByWall(Wall srcWall);
	// 寻找重合的墙体
	//vector<Wall> GetCoincidenceWalls(Point3f start_point, Point3f end_point);
	

public:

	Room single_room;
	// 入户门
	Door entrance_door;
	// 区域
	vector<RoomRegion> region_list;
	vector<Door> door_list;
	vector<Window> window_list;
	vector<Corner> corner_list;
	vector<Wall> wall_list;
	vector <WallBox> wall_box_list;
	vector<FloorBox>floor_box_list;
	vector<Model> model_list;
	// 组合列表
	vector<PackageStruct> package_list;
	// 布局数组
	vector<ModelLayout> layout_list;

	// json文件名称 临时使用
	string json_file_name;
};

