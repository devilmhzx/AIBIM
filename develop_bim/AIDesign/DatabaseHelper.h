/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:DatabaseHelper.h
*  简要描述:数据库相关操作
*
*  当前版本:
*  作者:常远
*  创建日期:2018-08-29
*  说明:
**************************************************/

#pragma once
#include "CommonSettings.h"
#include "ComUtil.h"
#include "Three\sqlite3\sqlite3.h"  
#include "Model.h"
#include <mutex>


struct SampleRoom
{
	string no;
	int		room_type;
	string design_no;
	string      data;
	string data_path;
};

struct RoomFunction
{
	int id;
	int		room_type;
	vector<int> model_inner_list;
	int      score;
};

// 规则结构体
struct RegualrtionsStruct
{
	string  regualrtion; // 规则内容
	string  no; // 规则编号
};

class DatabaseHelper
{
public:
	DatabaseHelper();
	~DatabaseHelper();

	//static std::mutex   db_mutex;
	//static DatabaseHelper* initance();

	// 初始化数据库
	bool initDB();

	// 通过模型编号检索规则
	vector<string> searchRegualrtionsByModel(const int modelId);

	// 通过房间类型检索房间特征值
	vector<SampleRoom> searchSampleRoomByType(const int roomType);

	// 检索所有的模板数据
	vector<SampleRoom> searchAllSampleRoom();

	// 插入样板间数据
	bool insertSampleRoom(SampleRoom sample);


	// 通过模型编号和样板间编号检索规则，返回的结果为规则列表
	vector<string> searchRegualrtionsByModelandSample(const int modelId,const string sampleNo);

	// 通过模型编号和样板间编号检索规则，返回的结果为规则结构体列表
	vector<RegualrtionsStruct> searchRegualrtionsStructByModelandSample(const int modelId, const string sampleNo);

	// 通过模型编号检索模型详细信息
	Model searchModelInfoById(const int modelId);

	// 检索所有模型信息
	vector<Model>  searchAllModelInfo();

	// 通过房间类型检索房间所需模型
	vector<RoomFunction> searchRoomFunctionByType(const int roomType);

	// 通过规则编号检索规则描述
	string searchRegualrtionsDescriptionByNo(const string no);

	// 通过模型和房间类型获得规则描述
	string searchRegualrtionsDescriptionByModel(const int space_id,const Model *p_model);

private:
	sqlite3 *db;
	string db_name;

	// 创建数据库
	bool create();
    // 打开数据库
	bool open();
	// 关闭数据库
	void close();
	// 创建模型表
	bool createModelTable();
	// 创建样板间表
	bool createSampleRoomTable();
	// 创建规则表
	bool createRegularTable();
	// 创建功能齐全表
	bool createRoomFunctionTable();
	// 表是否存在
	bool isExitsTable(string tableName);

	static DatabaseHelper * p;
};

