
/*******************************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:�����еķ���
*  ��Ҫ����:
*
*  ��������:2018/08/29
*  ��   ��:��Զ
*  ˵   ��:
*
******************************************************************/
#pragma once
#include "BaseObject.h"
#include "Point3f.h"
#include "RoomRegion.h"


class Room :
	public BaseObject
{
public:
	Room();
	// ���������ʼ������
	Room(RoomRegion region);
	virtual ~Room();

	void setName(string name);
	string getName();
	bool getIsOutroom();
	int getSpaceId();
	void setSpaceId(int space_id);
	vector<Point3f> getPointList();

	/*Room operator=(const Room& src_room);*/
	// ����BIM����
	void LoadBimData(Json::Value data);
	// �Ƿ��ڷ�����
	bool PointInRoom(Point3f point);
	vector<Point3f> point_list;

private:
	string name;
	int space_id;
	bool is_outroom; 
};

