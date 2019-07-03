/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:户型的洞
*  简要描述:
*
*  当前版本:
*  作者:常远
*  创建日期:2018-08-29
*  说明:
**************************************************/
#pragma once
#include "BaseObject.h"
#include "Point3f.h"

class Opening :
	public BaseObject
{
public:
	Opening();
	virtual ~Opening();

public:
	string name;
	Point3f pos;
	int opening_type;
	Point3f direction;
	float length;
	float width;
	float height;
	bool right_open;
	Point3f start_point;
	Point3f end_point;
	bool is_light;
};

