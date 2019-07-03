/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:梁 柱子 烟道
*  简要描述:
*
*  当前版本:V1.0.1
*  作者:jirenze
*  创建日期:2019-03-29
*  说明:
**************************************************/
#pragma once
#include "BaseBox.h"
#include "FloorBox.h"
class AirPillarBaseJ : public BaseBox
{
public:
	AirPillarBaseJ();
	virtual ~AirPillarBaseJ();

	// 解析BIM数据
	void LoadBimAirPillar(Json::Value data);

public:
	AirPillarType airpillar_type;
	/** 长宽高 */
	string room_no;
	/********************************************
	*   [0]top_left        [2]top_right
	*
	*				 
	*
	*
	*	[1]bottom_left    [3]bottom_right
	********************************************/
	Point3f top_left;
	Point3f bottom_left;
	Point3f top_right;
	Point3f bottom_right;

	string  m_no;                   // 32位唯一标识符
	unsigned int  m_property_flag;  //  属性标识
	string m_register_class;        // 注册类型
	string m_obj_name;              // 类型名称

	FloorBox GetNewFloorBox(AirPillarBaseJ &in_airpillar);
};