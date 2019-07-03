/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:共通结构体
*  简要描述:
*
*  当前版本:
*  作者:常远
*  创建日期:2018-09-05
*  说明:
**************************************************/
#include<vector>
#include "Model.h"

#pragma once

typedef struct layout
{
	vector<Model> model_list;
	float score;

}ModelLayout;

// 直线结构体
typedef struct line
{
	Point3f start_point;
	Point3f end_point;

}AiLine;

