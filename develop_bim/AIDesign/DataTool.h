/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:DataTool.h
*  ��Ҫ����:���ݴ���ͨ����
*
*  ��ǰ�汾:
*  ����:��Զ
*  ��������:2018-12-27
*  ˵��:
**************************************************/
#pragma once
#include"CommonSettings.h"
#include "Model.h"
class DataTool
{
public:
	DataTool();
	~DataTool();
public:
	// �жϴ�ģ���Ƿ�û�й���Ҳ�ܷ�����
	static bool IsMoldelLayoutWithoutRule (Model *p_model,int space_id);
};

