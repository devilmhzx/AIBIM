
/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:Design.h
*  简要描述:设计数据类
*
*  当前版本:
*  作者:常远
*  创建日期:2018-08-29
*  说明:
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

	// 解析BIM整个设计数据
	void LoadBimData(Json::Value data);

	// 解析App单个房间设计数据
	void LoadAppSingleData(Json::Value data);

	// 解析Bim单个房间设计数据
	void LoadBimSingleData(Json::Value data);

	// 解析App整个设计数据
	void LoadAppData(Json::Value data);

	// 序列化整个设计数据为Bim数据
	Json::Value DumpBimData(int index);

	// 通过房间获得模型列表
	vector<Model> GetModelsByRoomNo(string room_no);

	// 通过模型编号查找模型
	Model* GetModelByNo(string model_no);

	// 通过模型id查找模型
	Model* GetModelById(int model_id);
	// 除去重复的布局
	void RomoveSameLayout();

public:

	vector<Model> model_list;
	int version;
	Json::Value  hydropower_model;
	Json::Value  construction_model;
	Json::Value  file_list;
	Json::Value  hard_room_space;

	// 布局数组
	vector<ModelLayout> layout_list;
	// 空间id，中间变量
	int space_id;
private:
	// 根据数据库初始化模型
	void InitModel();
};

