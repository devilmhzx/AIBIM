/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:户型墙体
*  简要描述:
*
*  当前版本:V1.0.0
*  作者:常远
*  创建日期:2018-08-29
*  说明:
*
*  当前版本:V1.0.1
*  作者:常远
*  创建日期:2018-12-03
*  说明:添加墙体是否是虚拟的属性
**************************************************/
#pragma once
#include "BaseObject.h"
#include "Corner.h"
#include "Opening.h"
#include "Window.h"
#include "Door.h"


class Wall :public BaseObject
{
public:
	Wall();
	Wall(Corner* start_corner, Corner* end_corner);
	virtual ~Wall();
public:
	Corner start_corner;
	Corner end_corner;
	string room_no;
	bool is_virtual;
	vector<Window> window_list;
	vector<Door> door_list;

private:
	WallType wall_type;
	float height;
	bool is_bearing;
};

