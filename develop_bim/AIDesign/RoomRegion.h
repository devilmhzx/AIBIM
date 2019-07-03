/*******************************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:房间中的区域
*  简要描述:
*
*  创建日期:2018/12/25
*  作   者:常远
*  说   明:
*
******************************************************************/
#pragma once
#include "BaseObject.h"
class RoomRegion :
	public BaseObject
{
public:
	RoomRegion();
	virtual ~RoomRegion();

	int getRegionId();
	void setRegionId(int regionId);
	void setIsVirtual(bool isVirtual);
	bool getIsVirtual();
	string getRegionName();
	void setRegionName(string regionName);
	void setRegionRoomNo(string roomNo);
	string getRegionRoomNo();


	// 加载BIM数据
	void LoadBimData(Json::Value data);

public:
	vector<Point3f> point_list; // 组成区域的点集合
	vector<Line> wall_line_list; // 组成区域的墙线列表
	vector<Line> virtual_line_list; // 组成区域的虚线列表
private:
	int region_id; // 区域编号
	string region_name; // 区域名称
	bool is_virtual; // 是否是虚拟区域
	string room_no; // 房间编号
	
};

