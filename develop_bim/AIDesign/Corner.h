/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:墙点
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

class Corner :public BaseObject
{
public:
	Corner();
	virtual ~Corner();

public:

	Point3f point;
	string room_no;
	void CopyCorner(Corner *corner);

};

