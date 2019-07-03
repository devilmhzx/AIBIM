/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:��ͨ�ṹ��
*  ��Ҫ����:
*
*  ��ǰ�汾:
*  ����:��Զ
*  ��������:2018-09-05
*  ˵��:
**************************************************/
#include<vector>
#include "Model.h"

#pragma once

typedef struct layout
{
	vector<Model> model_list;
	float score;

}ModelLayout;

// ֱ�߽ṹ��
typedef struct line
{
	Point3f start_point;
	Point3f end_point;

}AiLine;

