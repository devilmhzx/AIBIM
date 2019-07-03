/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����: ������
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

class PackPipeJ : public BaseBox
{
public:
	PackPipeJ() {};
	~PackPipeJ() {};

	// ����BIM����
	void LoadBimPackPipe(Json::Value data);

	/**  */
public:
	/** ���� */
	float length;
	float height;
	string room_no;
	/********************************************
	*   [0]outside_0        [2]outside_1
	*
	*
	*
	*
	*	                    [3]outside_2
	********************************************/
	vector<Point3f> outsideVertices;
	string  m_no;                   // 32λΨһ��ʶ��
	unsigned int  m_property_flag;  //  ���Ա�ʶ
	string m_register_class;        // ע������
	string m_obj_name;              // ��������

	FloorBox GetNewFloorBox(PackPipeJ &in_airpillar);
private:
	/** ���������� */
	void CreateBox(vector<Point3f>& in_Vertices);
};