/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:户型门
*  简要描述:
*
*  当前版本:
*  作者:常远
*  创建日期:2018-08-29
*  说明:
*
*  当前版本:V1.0.1
*  作者:常远
*  创建日期:2018-12-04
*  说明:新增门的布局类型
**************************************************/
#pragma once
#include "Opening.h"

class Door :
	public Opening
{
public:
	Door();
	virtual ~Door();
	// 加载BIM数据
	void LoadBimDoor(Json::Value data);
public:
	DoorType door_type;
	DoorLayoutType door_layout_type;

};

