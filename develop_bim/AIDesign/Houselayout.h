/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:户型数据
*  简要描述:
*
*  当前版本:
*  作者:常远
*  创建日期:2018-08-29
*  说明:
**************************************************/
#pragma once

#include "Corner.h"
#include "Wall.h"
#include "Window.h"
#include "Door.h"
#include "Room.h"
#include "AirPillarBaseJ.h"
#include "FloorBox.h"
#include "WallBox.h"
#include "RoomRegion.h"
#include "ComponentObject.h"
#include "PackPipeJ.h"

class Houselayout
{
public:
	Houselayout();
	virtual ~Houselayout();

	// 解析BIM整个户型数据
	void LoadBimData(Json::Value data);
	// 解析BIM单房间数据
	void LoadBimSingleData(Json::Value data);
	// 解析APP单房间数据
	void LoadAppSingleData(Json::Value data);
	// 解析APP整个户型数据
	void LoadAppData(Json::Value data);
	// 获得房间编号获得房间对象
	Room* GetRoomByRoomNo(string room_no);
	// 获得房间的墙列表
	vector<Wall> GetWallsByRoomNo(string room_no);
	// 获得房间的门列表
	vector<Door> GetDoorsByRoomNo(string room_no);
	// 获得房间的窗列表
	vector<Window> GetWindowsByRoomNo(string room_no);
	// 获得房间的点列表
	vector<Corner> GetCornersByRoomNo(string room_no);
	// 获得房间的地标
	vector<FloorBox> GetFloorBoxesByRoomNo(string room_no);
	// 获得房间的区域
	vector<RoomRegion> GetRoomRegionsByRoomNo(string room_no);
	// 获得门编号获得门对象
	Door* GetDoorByDoorNo(string door_no);

	// 根据空间id获得空间列表
	vector<Room*> GetRoomsBySpaceId(int space_id);
	// 得到入户门的墙体
	Wall *GetWallByEntranceDoor();
	// 得到入户门
	Door *GetEntranceDoor();
	// 计算墙体的朝向向量
	Point3f CalculateWallNormal(Wall*wall);


public:
	int version;
	Door entrance_door;
	vector<Room> room_list;
	vector<Door> door_list;
	vector<Window> window_list;
	vector<Corner> corner_list;
	vector<Wall> wall_list;
	vector <WallBox> wall_box_list;
	vector<FloorBox> floor_box_list;
	vector<RoomRegion> region_list;

	

private: 

	// 初始化点和墙体数据
	void InitCorner();
	// 初始化门窗数据
	void InitOpening();
	// 把门放到墙上面
	void InitDoorOnWall(Door *door);
	// 把窗户放到墙上面
	void InitWindowOnWall(Window *window);
	// 初始化Box,把Box与墙体产生关系
	void InitBox();
	// 地标与房间关系
	void InitFloorBox(FloorBox* box);
};

