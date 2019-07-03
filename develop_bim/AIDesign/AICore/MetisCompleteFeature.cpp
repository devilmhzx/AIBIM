#include "MetisCompleteFeature.h"

MetisCompleteFeature::MetisCompleteFeature(const shared_ptr<AICase> &aicase)
	:ai_case(aicase), global_room_type(99), full_score(40)
{

}

void MetisCompleteFeature::PickBaseData()
{
	/** 得到数据库全局方案所有的roomfunction值 */

	auto p_database_helper = std::make_shared<DatabaseHelper>();
	database_globals = p_database_helper->searchRoomFunctionByType(global_room_type);

	/** 得到方案中的全部构件组别和roomtypes */
	for (auto&single_case : ai_case->single_case_list)
	{
		for (auto& model : single_case.model_list)
		{
			program_global_modeltypes.push_back(model.inner_type);
		}
	}
}

void MetisCompleteFeature::PickGlobalData()
{
	/** 数据库全局构件组别 */
	for (auto& database_global : database_globals)
	{
		/** 数据库没有找到方案中的构件组别，则扣分 */
		bool mark = true;

		/** 找到任意一个即可 */
		for (auto& model_inner : database_global.model_inner_list)
		{
			/** 检索方案中全局的构件组别 */
			for (auto& program_global_modeltype : program_global_modeltypes)
			{
				if (model_inner == program_global_modeltype)
				{
					mark = false;
					break;
				}
			}
			if (!mark)
			{
				break;
			}
		}

		/** 未找到为扣分项 */
		if (mark)
		{
			global_marks.push_back(database_global);
		}
	}
}

void MetisCompleteFeature::PickLocalData()
{
	/** 空间检索 */
	for (auto& singlecase : ai_case->single_case_list)
	{
		/** 得到相应空间数据库 */
		// j进行房间名称的装换
		int change_room_id = ComUtil::ChangeRoomId(singlecase.single_room.getSpaceId());

		shared_ptr<DatabaseHelper> p_db = std::make_shared<DatabaseHelper>();
		vector<RoomFunction> temp_database_locals = p_db->searchRoomFunctionByType(change_room_id);

		LocalFunction temp_local_function;
		temp_local_function.room_no = singlecase.single_room.GetNo();
		temp_local_function.room_type = singlecase.single_room.getSpaceId();

		/** 检索空间数据库和方案空间 */
		for (auto& database_local : temp_database_locals)
		{
			/** 数据库没有找到方案中的构件组别，则扣分 */
			bool mark = true;

			temp_local_function.score = database_local.score;
			temp_local_function.id = database_local.id;
			/** 找到任意一个即可 */
			for (auto& model_inner : database_local.model_inner_list)
			{
				temp_local_function.inner_type = model_inner;
				/** 检索方案中局部的构件组别 */
				for (auto& modeltype : singlecase.model_list)
				{
					if (model_inner == modeltype.inner_type)
					{
						mark = false;
						break;
					}
				}
				if (!mark)
				{
					break;
				}
			}
			/** 未找到为扣分项 */
			if (mark)
			{

				local_marks.push_back(temp_local_function);
			}
		}
		temp_database_locals.clear();
	}
}

int MetisCompleteFeature::CalculateTotalScore()
{
	int global_total_score = 0;
	int local_total_socre = 0;
	int total_score = 0;
	/** 计算全局和空间的分数 */
	for (auto& global_mark : global_marks)
	{
		global_total_score += global_mark.score;
	}

	for (auto& local_mark : local_marks)
	{
		local_total_socre += local_mark.score;
	}

	/** 扣的分数大于等于40，则为0 */
	if (global_total_score + local_total_socre >= full_score)
	{
		total_score = 0;
	}
	else
	{
		total_score = full_score - global_total_score - local_total_socre;
	}
	return total_score;
}

Json::Value MetisCompleteFeature::TransferData()
{
	/** 计算功能齐全数据 */
	PickBaseData();
	PickGlobalData();
	PickLocalData();

	/** 构造Json数据 */
	Json::Value room_function;
	room_function["version"] = 10001;
	//room_function["totalScore"] = CalculateTotalScore();

	/** 全局和空间数据 */
	Json::Value global;
	Json::Value global_list;
	Json::Value local;
	Json::Value local_list;

	/** 全局数据传递 */
	for (auto& global_mark : global_marks)
	{
		Json::Value g_list;
		if (global_mark.model_inner_list.size() > 0)
		{
			g_list["inner_type"] = global_mark.model_inner_list[0];
		}
		else
		{
			g_list["inner_type"] = 0;
		}
		g_list["score"] = global_mark.score;
		g_list["id"] = global_mark.id;
		global_list.append(g_list);
	}

	/** 空间数据传递 */
	for (auto& local_mark : local_marks)
	{
		Json::Value l_list;
		string s;
		l_list["room_no"] = local_mark.room_no;
		l_list["room_type"] = local_mark.room_type;
		l_list["inner_type"] = local_mark.inner_type;
		l_list["score"] = local_mark.score;
		l_list["id"] = local_mark.id;
		local_list.append(l_list);
	}

	if (global_list.size() > 0)
	{
		global["lostList"] = global_list;
	}
	else
	{
		global["lostList"].resize(0);
	}

	if (local_list.size() > 0)
	{
		local["lostList"] = local_list;
	}
	else
	{
		local["lostList"].resize(0);
	}

	room_function["global"] = global;
	room_function["local"] = local;
	return room_function;
}

