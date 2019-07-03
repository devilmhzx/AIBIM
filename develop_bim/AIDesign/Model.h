/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:����е�ģ��
*  ��Ҫ����:
*
*  ��ǰ�汾:
*  ����:��Զ
*  ��������:2018-08-29
*  ˵��:
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
	// װ��λ��
	void ConvertByModel(Model *src_model);

public:
	// ҵ������
	bool is_delete;
	// ��������
	string room_no;
	int id; 
	int product_id; // ҵ����Ҫ��Ʒid
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
	string rule_description;// �����������м����

	/*****************************************************************
	* ���������Ҿߵ�λ ���谴��˳ʱ��˳�� 
	*
	* ���ߣ�jirenze
	*****************************************************************/
	vector<Point3f> model_point;
};

