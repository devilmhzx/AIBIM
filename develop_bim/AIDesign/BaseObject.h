/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:���ͻ���
*  ��Ҫ����:
*
*  ��ǰ�汾:
*  ����:��Զ
*  ��������:2018-08-29
*  ˵��:
**************************************************/

#pragma once
#include "CommonSettings.h"
#include "ComUtil.h"
#include "ComMath.h"
#include"string"
#include<vector>
#include <json.h>
#include <sstream>

using namespace std;

class BaseObject
{
public:
	BaseObject();
	virtual ~BaseObject();

	void SetNo(string no);
	string GetNo();

	void SetPropertyFlag(int property_flag);
	int GetPropertyFlag();

	void SetObjName(string obj_name);
	string GetObjName();

	void SetRegisterClass(string register_class);
	string GetRegisterClass();


private:

	string  m_no;   // 32λΨһ��ʶ��
	unsigned int  m_property_flag;  //  ���Ա�ʶ
	string m_register_class; // ע������
	string m_obj_name;       // ��������

};

