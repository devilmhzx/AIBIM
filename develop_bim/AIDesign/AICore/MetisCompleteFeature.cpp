#include "MetisCompleteFeature.h"

MetisCompleteFeature::MetisCompleteFeature(const shared_ptr<AICase> &aicase)
	:ai_case(aicase), global_room_type(99), full_score(40)
{

}

void MetisCompleteFeature::PickBaseData()
{
	/** �õ����ݿ�ȫ�ַ������е�roomfunctionֵ */

	auto p_database_helper = std::make_shared<DatabaseHelper>();
	database_globals = p_database_helper->searchRoomFunctionByType(global_room_type);

	/** �õ������е�ȫ����������roomtypes */
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
	/** ���ݿ�ȫ�ֹ������ */
	for (auto& database_global : database_globals)
	{
		/** ���ݿ�û���ҵ������еĹ��������۷� */
		bool mark = true;

		/** �ҵ�����һ������ */
		for (auto& model_inner : database_global.model_inner_list)
		{
			/** ����������ȫ�ֵĹ������ */
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

		/** δ�ҵ�Ϊ�۷��� */
		if (mark)
		{
			global_marks.push_back(database_global);
		}
	}
}

void MetisCompleteFeature::PickLocalData()
{
	/** �ռ���� */
	for (auto& singlecase : ai_case->single_case_list)
	{
		/** �õ���Ӧ�ռ����ݿ� */
		// j���з������Ƶ�װ��
		int change_room_id = ComUtil::ChangeRoomId(singlecase.single_room.getSpaceId());

		shared_ptr<DatabaseHelper> p_db = std::make_shared<DatabaseHelper>();
		vector<RoomFunction> temp_database_locals = p_db->searchRoomFunctionByType(change_room_id);

		LocalFunction temp_local_function;
		temp_local_function.room_no = singlecase.single_room.GetNo();
		temp_local_function.room_type = singlecase.single_room.getSpaceId();

		/** �����ռ����ݿ�ͷ����ռ� */
		for (auto& database_local : temp_database_locals)
		{
			/** ���ݿ�û���ҵ������еĹ��������۷� */
			bool mark = true;

			temp_local_function.score = database_local.score;
			temp_local_function.id = database_local.id;
			/** �ҵ�����һ������ */
			for (auto& model_inner : database_local.model_inner_list)
			{
				temp_local_function.inner_type = model_inner;
				/** ���������оֲ��Ĺ������ */
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
			/** δ�ҵ�Ϊ�۷��� */
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
	/** ����ȫ�ֺͿռ�ķ��� */
	for (auto& global_mark : global_marks)
	{
		global_total_score += global_mark.score;
	}

	for (auto& local_mark : local_marks)
	{
		local_total_socre += local_mark.score;
	}

	/** �۵ķ������ڵ���40����Ϊ0 */
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
	/** ���㹦����ȫ���� */
	PickBaseData();
	PickGlobalData();
	PickLocalData();

	/** ����Json���� */
	Json::Value room_function;
	room_function["version"] = 10001;
	//room_function["totalScore"] = CalculateTotalScore();

	/** ȫ�ֺͿռ����� */
	Json::Value global;
	Json::Value global_list;
	Json::Value local;
	Json::Value local_list;

	/** ȫ�����ݴ��� */
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

	/** �ռ����ݴ��� */
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

