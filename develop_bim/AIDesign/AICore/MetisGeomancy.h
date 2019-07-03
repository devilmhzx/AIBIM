/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:风水测试
*  简要描述:
*
*  当前版本:V1.0.0
*  作者:jirenze、niuzongyao
*  创建日期:2018-11-07
*  说明:
**************************************************/
#pragma once
#include "../AICase.h"
#include "../Point3f.h"
#include "../Door.h"


/** 门的类型*/
enum RoomDoorType
{
	NOMEN=0,  // 没有类型
	RUHUMEN = 1,  // 入户门
	WOSHIMEN = 2,    // 卧室门
	CHUFANGMEN = 3, // 厨房门
	WEISHENGJIANMEN = 4     // 卫生间门
};

struct GeomancyData
{
	int id;  // 编号
	int count; // 个数
	int score; // 单位分数
};
struct DoorData
{
	string room_no;
	int room_door_type;
	Door door;
	string link_room_no;
};

class MetisGeomancy
{
public:

	MetisGeomancy() {};
	MetisGeomancy(const shared_ptr<AICase> &ai_case);
	~MetisGeomancy() {};
	Json::Value GetGeomancyJson();

private:
	// 寻找有效的门
	void PickEffectiveDoors();
	//判断两个门的是否相交
	bool IsDoorIntersect(DoorData src_door, DoorData des_door);
	// 通过房间类型和门类型获得房间门类型
	int GetRoomDoorType(int space_id, int door_type);
	// 初始化门相关数据
	void InitDoors();
	// 入户门对卫生间门10
	GeomancyData EntranceToToilet();
	// 入户门对厨房门11
	GeomancyData EntranceToKitchen();
	// 卧室门对卫生间门12
	GeomancyData BedroomToToilet();
	// 卧室门对厨房门13
	GeomancyData BedroomToKitchen();
	// 卧室门对入户门14
	GeomancyData BedroomToEntrance();
	// 卫生间门对厨房门15
	GeomancyData ToiletToKitchen();
	// 室内尖角16
	GeomancyData CheckSharpAngle();
	// 客厅沙发处出发，垂直方向不应看到房间门，否则有直捣黄龙之意 17
	GeomancyData CheckSoftToDoor();
	// 餐桌不可只对大门，否则容易被煞气冲散、把财气带走，扣2分；18
	GeomancyData CheckTableToDoor();
	// 卧室床头不对着房门；否则阴气较重，不利于主人健康，扣3分；19
	GeomancyData CheckBedHeadToDoor();
	// 镜子不能直对着枕头位，否则容易犯煞，扣3分；20
	GeomancyData CheckBedHeadToMirror();
	// 书桌不能面朝窗户，否则容易望空，扣1分；21
	GeomancyData CheckDeskToWindow();
	// 书桌不能正对大门，否则容易受干扰，扣1分；22
	GeomancyData CheckDeskToDoor();
	// 书桌背后应靠墙，否则没有靠山，扣1分23
	GeomancyData CheckDeskToWall();
	// 风水扣分结构体json化
	Json::Value GeomancyDataToJson(GeomancyData data);
	//获得分数列表
	Json::Value GetGeomancyScoreList();
private:
	// 有效的门
	vector<DoorData> effective_doors;
	// 卧室门
	vector<DoorData> bedroom_doors;
	// 入户门
	DoorData entrance_door;
	// 厨房门
	vector<DoorData> kitchen_doors;
	// 卫生间门
	vector<DoorData> toilet_doors;
	// 设计数据
	const shared_ptr<AICase> ai_case;
};
