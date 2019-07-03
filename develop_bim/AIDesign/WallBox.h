/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:户型中的墙标
*  简要描述:
*
*  当前版本:
*  作者:常远
*  创建日期:2018-09-05
*  说明:
**************************************************/
#pragma once
#include "BaseBox.h"
#include "CommonSettings.h"

class WallBox :
	public BaseBox
{
public:
	WallBox();
	virtual ~WallBox();

	// 解析BIM数据
	void LoadBimData(Json::Value data);
public:
	WallBoxType wall_box_type;
	string wall_no;
};

