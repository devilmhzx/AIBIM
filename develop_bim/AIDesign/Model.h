/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:设计中的模型
*  简要描述:
*
*  当前版本:
*  作者:常远
*  创建日期:2018-08-29
*  说明:
**************************************************/
#pragma once
#include "BaseObject.h"
#include "Point3f.h"


#define DEFAULT_ID -1
class Model :
	public BaseObject
{
public:
	Model();
	virtual ~Model();
	Model(Model* model);

	void LoadBimData(Json::Value data);
	Json::Value DumpBimData();
	// 装换位置
	void ConvertByModel(Model *src_model);

public:
	// 业务属性
	bool is_delete;
	// 数据属性
	string room_no;
	int id; 
	int product_id; // 业务需要商品id
	int layout_type;
	int inner_type;
	int weight;
	Point3f location;
	Point3f rotation;
	Point3f scale;
	int toggleMode;
	float length;
	float width;
	float height;
	float heiht_to_floor;
	string name;
	int direction;
	bool is_maker;
	string layout_rule;
	string rule_description;// 规则描述，中间变量

	/*****************************************************************
	* 变量名：家具点位 假设按照顺时针顺序 
	*
	* 作者：jirenze
	*****************************************************************/
	vector<Point3f> model_point;
};

