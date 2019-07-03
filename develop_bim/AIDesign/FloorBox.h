/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:户型中的地标
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

class FloorBox :
	public BaseBox
{
public:
	FloorBox();
	~FloorBox();

	// 解析BIM数据
	void LoadBimData(Json::Value data);

	// 解析BIM数据测试
	void LoadBimDataTest(Json::Value data);
public:
	FloorBoxType floor_box_type;
	string room_no;

};

