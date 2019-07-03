/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:DataTool.cpp
*  ��Ҫ����:���ݴ���ͨ����
*
*  ��ǰ�汾:
*  ����:��Զ
*  ��������:2018-12-27
*  ˵��:
**************************************************/
#include "DataTool.h"



DataTool::DataTool()
{
}


DataTool::~DataTool()
{
}

// �жϴ�ģ���Ƿ�û�й���Ҳ�ܷ�����
bool DataTool::IsMoldelLayoutWithoutRule(Model *p_model, int space_id)
{
	bool result_flag = false;
	// �������������ڴ���ģ�������ʲ�����輸����λɳ�������Ҹ�λɳ��
	/** �޸Ĺ���ID @������ */
	if (space_id == KETING)
	{
		switch (p_model->id)
		{
			case ModelGroup::GROUP_MAIN_SOFA:
				result_flag = true;
				break;
			case ModelGroup::GROUP_ASSIST_SOFA_L:
				result_flag = true;
				break;
			case ModelGroup::GROUP_ASSIST_SOFA_R:
				result_flag = true;
				break;
			case ModelGroup::GROUP_TEA_TABLE:
				result_flag = true;
				break;
			default:
				break;
		}
	}

	return result_flag;
}