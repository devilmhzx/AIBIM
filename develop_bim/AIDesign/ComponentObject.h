/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:��������
*  ��Ҫ����:

*  ��ǰ�汾:V1.0.1
*  ����:jirenze
*  ��������:2019-03-29
*  ˵��:Ϊ�˽���BIM�е�λ��ص�����
**************************************************/ 
#pragma once
#include "BaseBox.h"
#include "WallBox.h"
class ComponentObject : public BaseBox
{
public:
	ComponentObject();
	virtual ~ComponentObject();

	// ����BIM����
	void LoadBimComponentObject(Json::Value data);
public:
	/** ������� */
	ComponentJType componentJ_type;

	/** ��λ */
	string room_no;

	WallBox GetNewWallBox(ComponentObject & in_component_object);
private:

};

