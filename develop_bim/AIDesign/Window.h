/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:户型窗户
*  简要描述:
*
*  当前版本:
*  作者:常远
*  创建日期:2018-08-29
*  说明:
**************************************************/
#pragma once
#include "Opening.h"
#include "Door.h"
class Window :
	public Opening
{
public:
	Window();
	virtual ~Window();
	Window(Door *p_door);

	// 加载BIM数据
	void LoadBimWindow(Json::Value data);
public:
	int height_to_floor;
	WindonType window_type;
};

