/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:�����е�ǽ��
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

class WallBox :
	public BaseBox
{
public:
	WallBox();
	virtual ~WallBox();

	// ����BIM����
	void LoadBimData(Json::Value data);
public:
	WallBoxType wall_box_type;
	string wall_no;
};

