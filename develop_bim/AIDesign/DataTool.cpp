/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:DataTool.cpp
*  简要描述:数据处理共通方法
*
*  当前版本:
*  作者:常远
*  创建日期:2018-12-27
*  说明:
**************************************************/
#include "DataTool.h"



DataTool::DataTool()
{
}


DataTool::~DataTool()
{
}

// 判断此模型是否没有规则也能发布局
bool DataTool::IsMoldelLayoutWithoutRule(Model *p_model, int space_id)
{
	bool result_flag = false;
	// 客厅布局中由于存在模型组概念，故不处理茶几，主位沙发、左右辅位沙发
	/** 修改构件ID @纪仁泽 */
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