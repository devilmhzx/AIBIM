/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:功能齐全
*  简要描述:
*
*  当前版本:V1.0.0
*  作者:jirenze
*  创建日期:2018-11-06
*  说明:检索功能是否齐全，现已完成构件的检索
**************************************************/
#pragma once
#include "../DatabaseHelper.h"
#include "../AICase.h"

struct LocalFunction
{
	string room_no;
	int inner_type;
	int room_type;
	int score;
	int id;
};

class MetisCompleteFeature
{
public:
	MetisCompleteFeature():global_room_type(99),full_score(40) {};
	
	MetisCompleteFeature(const shared_ptr<AICase> &aicase);

	~MetisCompleteFeature() {};

	/** 提取数据库的数据 */
	void PickBaseData();

	/** 提取方案中全局数据 */
	void PickGlobalData();

	/** 提取方案中空间数据 */
	void PickLocalData();
	 
	/** 总分 */
	int CalculateTotalScore();

	/** 传递数据 */
	Json::Value TransferData();

private:
	/** 数据库全局 */
	vector<RoomFunction> database_globals;

	/** 方案中的全局构件组别 */
	vector<int> program_global_modeltypes;

	/** 全局扣分项 */
	vector<RoomFunction> global_marks;

	/** 空间扣分项 */
	vector<LocalFunction> local_marks;

	/** ai_case */
	const shared_ptr<AICase> ai_case;

	/** 全局数据库roomtype */
	const int global_room_type;

	/**满分为40分*/
	const int full_score;
};
