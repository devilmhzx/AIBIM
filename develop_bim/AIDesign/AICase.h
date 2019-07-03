/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:设计数据
*  简要描述:
*
*  当前版本:
*  作者:常远
*  创建日期:2018-09-05
*  说明:
**************************************************/

#pragma once
#include "Houselayout.h"
#include "Design.h"
#include "SingleCase.h"
#include "ComStruct.h"
#include <algorithm>

class AICase
{
public:
	AICase();
	virtual ~AICase();

	// 初始化户型与设计的关系
	void Init();

	// 初始化单房间数据
	void InitSingleCase();

	// 单房间数据转化为整个设计数据
	void SyncAICaseBySingle();

	// 单房间数据转化为整个设计数据
	void SyncSingleToDesign();

	// 单房间数据转化为整个设计数据
	void SyncSingleToDesign(SingleCase *p_single_case);

	// 根据模型id 查找房间对象
	Room* GetRoomByModelId(int model_id);

	// 根据模型id 查找所在的房间的门列表
	vector<Door>  GetDoorsByModelId(int model_id);

	// 初始化房间模型布局信息
	bool InitModelLayout(SingleCase *p_single_case);

	// 通过参考模型列表获得有效的模型列表
	vector<unsigned> GetEffectiveModelList(vector<unsigned> referenced_list);

	// 通过门的编号获得门所在的房间列表
	vector<string>GetRoomsByDoorNo(string door_no);

	// 设置阳光门
	void SetSunDoor();

	// 设置进光口
	void SetSunnyOpening();
	

public:
	Houselayout house;
	Design design;
	vector<SingleCase> single_case_list;

	string json_file_name;
};

