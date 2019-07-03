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
#include "Corner.h"
class BaseRegion :
	public BaseObject
{
public:
	BaseRegion();
	virtual ~BaseRegion();
public:
	vector<Corner> corner_list;
	RegionType region_type;

};

