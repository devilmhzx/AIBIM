/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:�����еĵر�
*  ��Ҫ����:
*
*  ��ǰ�汾:
*  ����:��Զ
*  ��������:2018-09-05
*  ˵��:
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

	// ����BIM����
	void LoadBimData(Json::Value data);

	// ����BIM���ݲ���
	void LoadBimDataTest(Json::Value data);
public:
	FloorBoxType floor_box_type;
	string room_no;

};

