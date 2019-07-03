/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:���ʹ���
*  ��Ҫ����:
*
*  ��ǰ�汾:
*  ����:��Զ
*  ��������:2018-08-29
*  ˵��:
**************************************************/
#pragma once
#include "Opening.h"
#include "Door.h"
class Window :
	public Opening
{
public:
	Window();
	virtual ~Window();
	Window(Door *p_door);

	// ����BIM����
	void LoadBimWindow(Json::Value data);
public:
	int height_to_floor;
	WindonType window_type;
};

