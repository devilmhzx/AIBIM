/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:户型基类
*  简要描述:
*
*  当前版本:
*  作者:常远
*  创建日期:2018-08-29
*  说明:
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

	string  m_no;   // 32位唯一标识符
	unsigned int  m_property_flag;  //  属性标识
	string m_register_class; // 注册类型
	string m_obj_name;       // 类型名称

};

