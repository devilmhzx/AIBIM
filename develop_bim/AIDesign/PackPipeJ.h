/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称: 包立管
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

class PackPipeJ : public BaseBox
{
public:
	PackPipeJ() {};
	~PackPipeJ() {};

	// 解析BIM数据
	void LoadBimPackPipe(Json::Value data);

	/**  */
public:
	/** 长高 */
	float length;
	float height;
	string room_no;
	/********************************************
	*   [0]outside_0        [2]outside_1
	*
	*
	*
	*
	*	                    [3]outside_2
	********************************************/
	vector<Point3f> outsideVertices;
	string  m_no;                   // 32位唯一标识符
	unsigned int  m_property_flag;  //  属性标识
	string m_register_class;        // 注册类型
	string m_obj_name;              // 类型名称

	FloorBox GetNewFloorBox(PackPipeJ &in_airpillar);
private:
	/** 构造最大矩形 */
	void CreateBox(vector<Point3f>& in_Vertices);
};