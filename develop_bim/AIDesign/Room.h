
/*******************************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:户型中的房间
*  简要描述:
*
*  创建日期:2018/08/29
*  作   者:常远
*  说   明:
*
******************************************************************/
#pragma once
#include "BaseObject.h"
#include "Point3f.h"
#include "RoomRegion.h"


class Room :
	public BaseObject
{
public:
	Room();
	// 根据区域初始化房间
	Room(RoomRegion region);
	virtual ~Room();

	void setName(string name);
	string getName();
	bool getIsOutroom();
	int getSpaceId();
	void setSpaceId(int space_id);
	vector<Point3f> getPointList();

	/*Room operator=(const Room& src_room);*/
	// 加载BIM数据
	void LoadBimData(Json::Value data);
	// 是否在房间内
	bool PointInRoom(Point3f point);
	vector<Point3f> point_list;

private:
	string name;
	int space_id;
	bool is_outroom; 
};

