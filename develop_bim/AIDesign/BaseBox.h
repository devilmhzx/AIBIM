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

