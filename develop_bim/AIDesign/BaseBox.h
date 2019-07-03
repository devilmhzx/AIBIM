/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:户型中的box
*  简要描述:
*
*  当前版本:
*  作者:常远
*  创建日期:2018-09-05
*  说明:
**************************************************/
#pragma once
#include "BaseObject.h"
class BaseBox :
	public BaseObject
{
public:
	BaseBox();
	virtual ~BaseBox();

public:
	float length;
	float width;
	float height;
	bool is_circle;
	Point3f center;
	int radio;
	vector<Point3f> point_list;
};

