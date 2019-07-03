/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:房间区域
*  简要描述:
*
*  当前版本:
*  作者:常远
*  创建日期:2018-12-04
*  说明:
**************************************************/
#include "RoomRegion.h"



RoomRegion::RoomRegion()
{
	this->region_name = "";
	this->region_id = 0;
}


RoomRegion::~RoomRegion()
{
	this->region_name = "";
	this->region_id = 0;
}

int RoomRegion::getRegionId()
{
	return this->region_id;
}
void RoomRegion::setRegionId(int regionId)
{
	this->region_id = regionId;
}
void RoomRegion::setIsVirtual(bool isVirtual)
{
	this->is_virtual = isVirtual;
}
bool RoomRegion::getIsVirtual()
{
	return this->is_virtual;
}
string RoomRegion::getRegionName()
{
	return this->region_name;
}
void RoomRegion::setRegionName(string regionName)
{
	this->region_name = regionName;
}
void RoomRegion::setRegionRoomNo(string roomNo)
{
	this->room_no = roomNo;
}
string RoomRegion::getRegionRoomNo()
{
	return this->room_no;
}

// 加载BIM数据
void RoomRegion::LoadBimData(Json::Value data)
{
	// 初始化编号
	if (data.isMember("objectId") && data["objectId"].isString())
	{
		string objectId = data["objectId"].asString();
		this->SetNo(objectId);
	}

	// 初始化标志
	if (data.isMember("propertyFlag") && data["propertyFlag"].isInt())
	{
		int property_flag = data["propertyFlag"].asInt();
		this->SetPropertyFlag(property_flag);
	}

	// 初始化注册类
	if (data.isMember("RegisterClass") && data["RegisterClass"].isString())
	{
		string register_class = data["RegisterClass"].asString();
		this->SetRegisterClass(register_class);
	}

	// 初始化object名称
	if (data.isMember("objectName") && data["objectName"].isString())
	{
		string obj_name = data["objectName"].asString();
		obj_name = ComUtil::JsonStringToUTF_8(obj_name);
		this->SetObjName(obj_name);
	}


	// 初始化名称
	if (data.isMember("RegionName") && data["RegionName"].isString())
	{
		string obj_name = data["RegionName"].asString();
		obj_name = ComUtil::JsonStringToUTF_8(obj_name);
		this->region_name = obj_name;
	}

	// 初始化所在房间的no
	if (data.isMember("SpaceObjectId") && data["SpaceObjectId"].isString())
	{
		string obj_name = data["SpaceObjectId"].asString();
		obj_name = ComUtil::JsonStringToUTF_8(obj_name);
		this->room_no = obj_name;
	}


	// 初始化region_id
	if (data.isMember("RegionId") && data["RegionId"].isInt())
	{
		int region_id = data["RegionId"].asInt();
		this->region_id = region_id;
	}
	else
	{
		this->region_id = 0;
	}

	// 初始化是否虚拟区域
	this->is_virtual = false;
	if (data.isMember("is_virtual") && data["is_virtual"].isInt())
	{
		int tmp_obj = data["is_virtual"].asInt();
		if (tmp_obj == 0)
		{
			this->is_virtual = false;
		}
		else
		{
			this->is_virtual = true;
		}
	}


	// 初始化房间点的集合
	if (data.isMember("PointList") && data["PointList"].isArray())
	{
		Json::Value points = data["PointList"];
		for (unsigned int i = 0; i < points.size(); i++)
		{
			if (points[i].isString())
			{
				string str_point = points[i].asString();
				Point3f temp_point = ComUtil::StringToPoint3f(str_point);
				this->point_list.push_back(temp_point);
			}
		}
	}
	// 初始化房间实线的集合
	if (data.isMember("WallList") && data["WallList"].isArray())
	{
		Json::Value wallLines = data["WallList"];
		for (unsigned int i = 0; i < wallLines.size(); i++)
		{
			if (wallLines[i].isObject())
			{
				Json::Value wallLine = wallLines[i];
				Line tmpLine = Line();
				tmpLine.is_virtual = false;
				if (wallLine.isMember("WallLineID") && wallLine["WallLineID"].isString())
				{
					tmpLine.no = ComUtil::JsonStringToUTF_8(wallLine["WallLineID"].asString());
				}
				if (wallLine.isMember("WallLineStart") && wallLine["WallLineStart"].isString())
				{
					tmpLine.start = ComUtil::StringToPoint3f(wallLine["WallLineStart"].asString());
				}
				if (wallLine.isMember("WallLineEnd") && wallLine["WallLineEnd"].isString())
				{
					tmpLine.end = ComUtil::StringToPoint3f(wallLine["WallLineEnd"].asString());
				}
				this->wall_line_list.push_back(tmpLine);
			}
		}
	}

	// 初始化房间虚线的集合
	if (data.isMember("VirtualLineList") && data["VirtualLineList"].isArray())
	{
		Json::Value virtualLines = data["VirtualLineList"];
		for (unsigned int i = 0; i < virtualLines.size(); i++)
		{
			if (virtualLines[i].isObject())
			{
				Json::Value virtualLine = virtualLines[i];
				Line tmpLine = Line();
				tmpLine.is_virtual = true;
				if (virtualLine.isMember("LineStart") && virtualLine["LineStart"].isString())
				{
					tmpLine.start = ComUtil::StringToPoint3f(virtualLine["LineStart"].asString());
				}
				if (virtualLine.isMember("LineEnd") && virtualLine["LineEnd"].isString())
				{
					tmpLine.end = ComUtil::StringToPoint3f(virtualLine["LineEnd"].asString());
				}
				this->virtual_line_list.push_back(tmpLine);
			}
		}
	}


}