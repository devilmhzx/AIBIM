/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:���͵Ķ�
*  ��Ҫ����:
*
*  ��ǰ�汾:
*  ����:��Զ
*  ��������:2018-08-29
*  ˵��:
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

