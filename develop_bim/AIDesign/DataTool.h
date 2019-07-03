/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:DataTool.h
*  简要描述:数据处理共通方法
*
*  当前版本:
*  作者:常远
*  创建日期:2018-12-27
*  说明:
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
	// 判断此模型是否没有规则也能发布局
	static bool IsMoldelLayoutWithoutRule (Model *p_model,int space_id);
};

