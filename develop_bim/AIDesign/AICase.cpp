#include "AICase.h"
#include "DataTool.h"
#include "CaseMatch/MetisRegionMatcher.h"


AICase::AICase()
{
}


AICase::~AICase()
{
}

// 初始化户型与设计的关系
void AICase::Init()
{
	for (vector<Room>::iterator room = house.room_list.begin(); room != house.room_list.end(); room++)
	{
		string room_no = room->GetNo();
		// 初始化模型关系
		for (vector<Model>::iterator model = design.model_list.begin(); model != design.model_list.end(); model++)
		{
			if (room->PointInRoom(model->location))
			{
				model->room_no = room_no;
			}
		}
	}
}

// 初始化单房间数据
void AICase::InitSingleCase()
{
	// 初始化户型数据
	for (vector<Room>::iterator room = house.room_list.begin(); room != house.room_list.end(); room++)
	{
		SingleCase single_case = SingleCase();

		single_case.json_file_name = this->json_file_name;
		// 初始化硬装
		single_case.single_room = *room;
		string room_no = room->GetNo();
		single_case.wall_list = house.GetWallsByRoomNo(room_no);
		single_case.door_list = house.GetDoorsByRoomNo(room_no);
		single_case.window_list = house.GetWindowsByRoomNo(room_no);
		single_case.corner_list = house.GetCornersByRoomNo(room_no);
		single_case.floor_box_list = house.GetFloorBoxesByRoomNo(room_no);
		// 初始化区域
		single_case.region_list = house.GetRoomRegionsByRoomNo(room_no);
		// 初始化软装模型
		single_case.model_list = design.GetModelsByRoomNo(room_no);
		
		single_case_list.push_back(single_case);
	}
}

// 单房间数据转化为整个设计数据
void AICase::SyncAICaseBySingle() 
{
	// 不进行户型数据的处理，只进行设计数据处理

	for (vector<SingleCase>::iterator single_case = single_case_list.begin(); single_case != single_case_list.end(); single_case++)
	{
		// 处理模型坐标位置文件
		for (vector<Model>::iterator model = single_case->model_list.begin(); model != single_case->model_list.end(); model++)
		{
			Model *p_model = design.GetModelByNo(model->GetNo());
			if(p_model == NULL)
			{
				continue;
			}
			// 模型转标位置转换
			p_model->ConvertByModel(&(*model));
			p_model->is_delete = false;
		}
	}
}

// 单房间数据转化为整个设计数据
void AICase::SyncSingleToDesign()
{
	// 不进行户型数据的处理，只进行设计数据处理

	for (vector<SingleCase>::iterator single_case = single_case_list.begin(); single_case != single_case_list.end(); single_case++)
	{
		for (int i = 0; i < single_case->layout_list.size(); i++)
		{
			ModelLayout layout = single_case->layout_list[i];
			design.layout_list.push_back(layout);
		}
	}
}

// 单房间数据转化为整个设计数据
void AICase::SyncSingleToDesign(SingleCase *p_single_case)
{
	// 不进行户型数据的处理，只进行设计数据处理
	for (int i = 0; i < p_single_case->layout_list.size(); i++)
	{
		ModelLayout layout = p_single_case->layout_list[i];
		design.layout_list.push_back(layout);
	}
}

// 根据模型id 查找房间对象
Room* AICase::GetRoomByModelId(int model_id)
{
	Model * model = this->design.GetModelById(model_id);
	if (model != NULL)
	{
		return (this->house.GetRoomByRoomNo(model->room_no));
	}
	else
	{
		return NULL;
	}
}

// 根据模型id 查找所在的房间的门列表
vector<Door>  AICase::GetDoorsByModelId(int model_id)
{
	Room * p_room = GetRoomByModelId(model_id);
	if (p_room == NULL)
	{
		vector<Door> temp_list;
		return temp_list;
	}
	else
	{
		return this->house.GetDoorsByRoomNo(p_room->GetNo());
	}
}

// 临时配合排序使用
bool CompareModel(const Model &a, const Model &b)
{
	 return a.weight < b.weight;
}

bool AICase::InitModelLayout(SingleCase *p_single_case)
{
	// 初始化模型信息
	shared_ptr<DatabaseHelper> p_db = std::make_shared<DatabaseHelper>();
	if (!p_db)
		return false;

	if (!p_single_case)
		return false;

	// 查找模板
	string home_no = "";
	MetisRegionMatcher Matcher;
	if (Matcher.FindHighSimilarityOnServerTemplate(p_single_case, 0 ))
	{
		SampleRoom home = Matcher.GetHighSimilarityTemplate();
		home_no = home.no;
	}

	for(vector<Model>::iterator model = p_single_case->model_list.begin(); model != p_single_case->model_list.end();)
	{
		// 判断此模型是否没有规则也能发布局
		if (DataTool::IsMoldelLayoutWithoutRule(&(*model), p_single_case->single_room.getSpaceId()))
		{
			++model;
			continue;
		}
		// 模型长宽小于零则不进行布局
		if (model->width < 1 || model->length < 1)
		{
			model = p_single_case->model_list.erase(model);
			continue;
		}

		auto string_one_model = p_db->searchRegualrtionsByModelandSample(model->id, home_no);
		if (string_one_model.size() < 1)
		{
			model = p_single_case->model_list.erase(model);
			continue;
		}

		auto model_info = p_db->searchModelInfoById(model->id);
		// 初始化模型优先级
		model->weight = 99;
		if (model_info.id > 0)
		{
			if (model_info.weight > 0)
			{
				model->weight = model_info.weight;
			}
		}

		// 进行模型长宽变换TODO_CY
		/*if (model->id == 1001 || model->id == 1002)
		{
			int tmp = model->width;
			model->width = model->length;
			model->length = tmp;
		}*/

		// 如果是独立性的模型则不处理
		// 非依赖性模型
		model->layout_type = 0;
		for (auto string : string_one_model)
		{
			if (string.find("@") != -1)
			{
				// 依赖性模型
				model->layout_type = 1;
				break;
			}
		}
		/** 测试衣柜为依赖性家具 临时使用 @纪仁泽 */
		if (model->id == 1006)
		{
			model->layout_type = 1;
			break;
		}
		++model;

	}

	// 根据优先级进行布局排序
	sort(p_single_case->model_list.begin(), p_single_case->model_list.end(), CompareModel);

	return true;
}

// 通过参考模型列表获得有效的模型列表
vector<unsigned> AICase::GetEffectiveModelList(vector<unsigned> referenced_list)
{
	vector<unsigned> result;
	for (int i = 0; i < referenced_list.size();i++)
	{
		for (vector<Model>::iterator model = this->design.model_list.begin(); model != this->design.model_list.end(); model++)
		{
			if (model->id == referenced_list[i])
			{
				result.push_back(model->id);
				break;
			}
		}
	}

	return result;
}

// 通过门的编号获得门所在的房间列表
vector<string> AICase::GetRoomsByDoorNo(string door_no)
{
	vector<string> room_nos;
	// 遍历房间
	for (auto& single_case : single_case_list)
	{
		// 遍历房间中的门
		for (auto& door : single_case.door_list)
		{
			if (door.GetNo() == door_no)
			{
				room_nos.push_back(single_case.single_room.GetNo());
				break;
			}
		}
	}

	return room_nos;
}


// 设置阳光门
void AICase::SetSunDoor()
{
	// 遍历所有singlecase,只处理客厅
	for (SingleCase &single_case : single_case_list)
	{
		if (single_case.single_room.getSpaceId() == KETING)
		{
			// 遍历客厅的门
			for (auto &door : single_case.door_list)
			{
				if (door.door_type == D_SLIDING || door.door_type == D_PASS)
				{
					door.door_layout_type = D_SUN_LAYOUT;
					continue;
				}

				// 修改墙体上面存储的门的类型
				for (Wall &wall : single_case.wall_list)
				{
					for (Door &des_door : wall.door_list)
					{
						if (door.GetNo() == des_door.GetNo() && door.door_layout_type== D_SUN_LAYOUT)
						{
							des_door.door_layout_type = door.door_layout_type;
						}
					}
				}
			}
		}
	}
}

// 设置进光口
void AICase::SetSunnyOpening()
{
	for (SingleCase &single_case : single_case_list)
	{
		// 只处理客厅
		if (single_case.single_room.getSpaceId() != KETING)
		{
			continue;
		}
	    
		// 获得有效的窗体
		Window *p_max_window=NULL;
		float max_window_length = -9999.0f;
		for (Window &window : single_case.window_list)
		{
			if (window.length > max_window_length)
			{
				max_window_length = window.length;
				p_max_window = &window;
			}
		}

		// 获得有效的门
		Door *p_max_door= NULL;
		float max_door_length = -9999.0f;

		for (auto &door : single_case.door_list)
		{
			// 如果是入户门则不处理
			if (door.door_type == D_SECURITY)
			{
				continue;
			}
			// 如果此门不是联通门则不处理
			vector<string>room_nos = GetRoomsByDoorNo(door.GetNo());
			if (room_nos.size() != 2)
			{
				continue;
			}

			string no1 = room_nos[0];
			string no2 = room_nos[1];
			string tmp_room_no = no2;
			if (no2 == single_case.single_room.GetNo())
			{
				tmp_room_no = no1;
			}
			Room *p_room = house.GetRoomByRoomNo(tmp_room_no);
			if (p_room->getSpaceId() == YANGTAI)
			{
				if (door.length > max_door_length)
				{
					max_door_length = door.length;
					p_max_door = &door;
				}
				continue;
			}
		}

		// 从有效门和有效窗中选择一个最为进光口
		int light_flag = -1; // 进光口编制  1 ：代表窗户为进光口  2：代表门为进光口
		// 如果门窗都有效
		if (max_window_length > 0 && max_door_length > 0)
		{
			if (max_window_length > max_door_length)
			{
				p_max_window->is_light = true;
				light_flag = 1;
			}
			else
			{
				p_max_door->is_light = true;
				light_flag = 2;
			}
		}

		// 如果窗户有效
		if (max_window_length > 0 && max_door_length < 0)
		{
			p_max_window->is_light = true;
			light_flag = 1;
		}
		// 如果门有效
		if (max_window_length < 0 && max_door_length  >0)
		{
			p_max_door->is_light = true;
			light_flag = 2;
		}

		//  如果没有进光口
		if (light_flag < 0)
		{
			break;
		}

		//  如果窗户为进光口
		if (light_flag == 1)
		{
			// 修改墙体上面存储的窗户的类型
			for (Wall &wall : single_case.wall_list)
			{
				for (Window &des_window : wall.window_list)
				{
					if (p_max_window->GetNo() == des_window.GetNo() && p_max_window->is_light == true)
					{
						des_window.is_light = true;
					}
				}
			}
			break;
		}

		//  如果门为进光口
		if (light_flag == 2)
		{
			// 修改墙体上面存储的窗户的类型
			for (Wall &wall : single_case.wall_list)
			{
				for (Door &des_door : wall.door_list)
				{
					if (p_max_door->GetNo() == des_door.GetNo() && p_max_door->is_light == true)
					{
						des_door.is_light = true;
					}
				}
			}
			break;
		}
	}
}