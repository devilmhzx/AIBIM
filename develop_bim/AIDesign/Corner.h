/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:ǽ��
*  ��Ҫ����:
*
*  ��ǰ�汾:
*  ����:��Զ
*  ��������:2018-08-29
*  ˵��:
**************************************************/

#pragma once
#include "BaseObject.h"
#include "Point3f.h"

class Corner :public BaseObject
{
public:
	Corner();
	virtual ~Corner();

public:

	Point3f point;
	string room_no;
	void CopyCorner(Corner *corner);

};

