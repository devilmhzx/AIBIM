/*******************************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:�����е�����
*  ��Ҫ����:
*
*  ��������:2018/12/25
*  ��   ��:��Զ
*  ˵   ��:
*
******************************************************************/
#pragma once
#include "BaseObject.h"
class RoomRegion :
	public BaseObject
{
public:
	RoomRegion();
	virtual ~RoomRegion();

	int getRegionId();
	void setRegionId(int regionId);
	void setIsVirtual(bool isVirtual);
	bool getIsVirtual();
	string getRegionName();
	void setRegionName(string regionName);
	void setRegionRoomNo(string roomNo);
	string getRegionRoomNo();


	// ����BIM����
	void LoadBimData(Json::Value data);

public:
	vector<Point3f> point_list; // �������ĵ㼯��
	vector<Line> wall_line_list; // ��������ǽ���б�
	vector<Line> virtual_line_list; // �������������б�
private:
	int region_id; // ������
	string region_name; // ��������
	bool is_virtual; // �Ƿ�����������
	string room_no; // ������
	
};

