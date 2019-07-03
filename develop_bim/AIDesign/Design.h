
/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:Design.h
*  ��Ҫ����:���������
*
*  ��ǰ�汾:
*  ����:��Զ
*  ��������:2018-08-29
*  ˵��:
**************************************************/
#pragma once
#include "Model.h"
#include "ComStruct.h"
#include"DatabaseHelper.h"

class Design
{
public:
	Design();
	virtual ~Design();

	// ����BIM�����������
	void LoadBimData(Json::Value data);

	// ����App���������������
	void LoadAppSingleData(Json::Value data);

	// ����Bim���������������
	void LoadBimSingleData(Json::Value data);

	// ����App�����������
	void LoadAppData(Json::Value data);

	// ���л������������ΪBim����
	Json::Value DumpBimData(int index);

	// ͨ��������ģ���б�
	vector<Model> GetModelsByRoomNo(string room_no);

	// ͨ��ģ�ͱ�Ų���ģ��
	Model* GetModelByNo(string model_no);

	// ͨ��ģ��id����ģ��
	Model* GetModelById(int model_id);
	// ��ȥ�ظ��Ĳ���
	void RomoveSameLayout();

public:

	vector<Model> model_list;
	int version;
	Json::Value  hydropower_model;
	Json::Value  construction_model;
	Json::Value  file_list;
	Json::Value  hard_room_space;

	// ��������
	vector<ModelLayout> layout_list;
	// �ռ�id���м����
	int space_id;
private:
	// �������ݿ��ʼ��ģ��
	void InitModel();
};

