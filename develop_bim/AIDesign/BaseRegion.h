/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:�����е�box
*  ��Ҫ����:
*
*  ��ǰ�汾:
*  ����:��Զ
*  ��������:2018-09-05
*  ˵��:
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

