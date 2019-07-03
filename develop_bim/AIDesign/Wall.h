/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:����ǽ��
*  ��Ҫ����:
*
*  ��ǰ�汾:V1.0.0
*  ����:��Զ
*  ��������:2018-08-29
*  ˵��:
*
*  ��ǰ�汾:V1.0.1
*  ����:��Զ
*  ��������:2018-12-03
*  ˵��:���ǽ���Ƿ������������
**************************************************/
#pragma once
#include "BaseObject.h"
#include "Corner.h"
#include "Opening.h"
#include "Window.h"
#include "Door.h"


class Wall :public BaseObject
{
public:
	Wall();
	Wall(Corner* start_corner, Corner* end_corner);
	virtual ~Wall();
public:
	Corner start_corner;
	Corner end_corner;
	string room_no;
	bool is_virtual;
	vector<Window> window_list;
	vector<Door> door_list;

private:
	WallType wall_type;
	float height;
	bool is_bearing;
};

