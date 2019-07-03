/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:�� ���� �̵�
*  ��Ҫ����:
*
*  ��ǰ�汾:V1.0.1
*  ����:jirenze
*  ��������:2019-03-29
*  ˵��:
**************************************************/
#pragma once
#include "BaseBox.h"
#include "FloorBox.h"
class AirPillarBaseJ : public BaseBox
{
public:
	AirPillarBaseJ();
	virtual ~AirPillarBaseJ();

	// ����BIM����
	void LoadBimAirPillar(Json::Value data);

public:
	AirPillarType airpillar_type;
	/** ����� */
	string room_no;
	/********************************************
	*   [0]top_left        [2]top_right
	*
	*				 
	*
	*
	*	[1]bottom_left    [3]bottom_right
	********************************************/
	Point3f top_left;
	Point3f bottom_left;
	Point3f top_right;
	Point3f bottom_right;

	string  m_no;                   // 32λΨһ��ʶ��
	unsigned int  m_property_flag;  //  ���Ա�ʶ
	string m_register_class;        // ע������
	string m_obj_name;              // ��������

	FloorBox GetNewFloorBox(AirPillarBaseJ &in_airpillar);
};