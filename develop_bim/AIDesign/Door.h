/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:������
*  ��Ҫ����:
*
*  ��ǰ�汾:
*  ����:��Զ
*  ��������:2018-08-29
*  ˵��:
*
*  ��ǰ�汾:V1.0.1
*  ����:��Զ
*  ��������:2018-12-04
*  ˵��:�����ŵĲ�������
**************************************************/
#pragma once
#include "Opening.h"

class Door :
	public Opening
{
public:
	Door();
	virtual ~Door();
	// ����BIM����
	void LoadBimDoor(Json::Value data);
public:
	DoorType door_type;
	DoorLayoutType door_layout_type;

};

