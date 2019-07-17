#include "MetisDynamicLayout.h"
#include "../Log/easylogging++.h"

void MetisDynamicLayout::CalculateIndependentExcellentRegion(shared_ptr<MetisRegionResolver>& region_resolver)
{
	/** 检测是否有独立性家具权重图 */
	if (region_resolver->independent_regulation_map.empty())
	{
		LOG(ERROR) << "independent not regulation_map:" << region_resolver->p_region->single_room.GetNo();
		return;
	}

	vector<SingleFurnitureAllRuleLayoutData> single_furniture_weight_data;

	/** 遍历需要摆放区域内独立性的所有家具 */
	for (auto &furntiure : region_resolver->independent_model_container)
	{
		/** 根据家具ID来检索到所有权重图 */
		vector<FurnitureWeight> temp_regulation_container = region_resolver->independent_regulation_map[furntiure.id];

		/** 处理独立性家具的权重图，获得单一规则下的适合的解 */
		for (auto &furniture_weight_map : temp_regulation_container)
		{
			PickIndependentSuitableResult(single_furniture_weight_data, furniture_weight_map, furntiure, region_resolver);
		}

		region_resolver->independent_furniture_all_rule_weight[furntiure.id] = single_furniture_weight_data;
		single_furniture_weight_data.clear();
	}
	int a = 0;
	/** 计算独立性家具的布局 */
	CalculateIndependentFurnitureSingleRule(region_resolver);
}

void MetisDynamicLayout::CalculateAttachExcellentRegion(shared_ptr<MetisRegionResolver>& region_resolver)
{
	if (region_resolver->attach_regulation_map.empty())
	{
		return;
	}

	// 依附性家具的布局信息 
	vector<SingleFurnitureAllRuleLayoutData> single_furniture_weight_data;

	/** 处理每个独立性布局产生的依附性家具的权重图 */
	for (auto&attach_furniture_weight_map : region_resolver->attach_regulation_map)
	{
		map<int, vector<SingleFurnitureAllRuleLayoutData>> single_layout_rule_layout;

		/** 遍历需要摆放区域内的依附性家具 */
		for (auto &furntiure : region_resolver->attach_model_container)
		{
			/** 得到家具的ID,来检索到权重图 */
			/** 获得对应家具的每一个权重图，找到对应的解 */
			for (auto &attach_furniture_weight : attach_furniture_weight_map[furntiure.id])
			{
				PickAttachSuitableResult(single_furniture_weight_data, attach_furniture_weight, furntiure, region_resolver);
			}

			single_layout_rule_layout.insert(make_pair(furntiure.id, single_furniture_weight_data));
			single_furniture_weight_data.clear();
		}

		region_resolver->attach_furniture_all_rule_weight.push_back(single_layout_rule_layout);
		single_layout_rule_layout.clear();
	}

	/** 匹配所有独立性家具方案 */
	MatchAttachFurniture(region_resolver);
}

void MetisDynamicLayout::PickIndependentSuitableResult(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_weight_data, FurnitureWeight& furniture_weight_map, Model& furniture, shared_ptr<MetisRegionResolver>& region_resolver)
{
	region_resolver->AddIndependentLayoutDataArray(single_furniture_weight_data);

	float min_score = -999999.0f;
	int min_score_index;
	TraveseRoomLeaf_Independent_0(single_furniture_weight_data, furniture_weight_map, furniture, region_resolver, min_score, min_score_index);
	TraveseRoomLeaf_Independent_270(single_furniture_weight_data, furniture_weight_map, furniture, region_resolver, min_score, min_score_index);

	SortExcellent(single_furniture_weight_data);
}

void MetisDynamicLayout::TraveseRoomLeaf_Independent_0(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_weight_data, FurnitureWeight& furniture_weight_map, Model &furniture, shared_ptr<MetisRegionResolver> &region_resolver, float &min_score, int &min_score_index)
{
	/** 家具的行列数量 */
	int furniture_row_num = static_cast<int>(((int)furniture.length % region_resolver->BoxSize == 0) ? furniture.length / region_resolver->BoxSize : furniture.length / region_resolver->BoxSize + 1);
	int furniture_rank_num = static_cast<int>(((int)furniture.width % region_resolver->BoxSize == 0) ? furniture.width / region_resolver->BoxSize : furniture.width / region_resolver->BoxSize + 1);

	/** 提供缩小覆盖区域的误差 */
	float error_range = 0.1f;

	// 无需整体遍历，只需要遍历部分即可
	int row_count = region_resolver->max_row - furniture_row_num + 1;
	int rank_count = region_resolver->max_rank - furniture_rank_num + 1;

	for (int row_index = 0; row_index < row_count; ++row_index)
	{
		for (int rank_index = 0; rank_index < rank_count; ++rank_index)
		{

			/********************************************
				* vector 存储点位顺序
				*   [0]top_left        [1]top_right
				*
				*
				*
				*
				*	[3]bottom_right    [2]bottom_right
			********************************************/
			//判断是否本身就是门点 或者 不在区域内 或者在地标区域内
			if (furniture_weight_map.regulation_map[row_index][rank_index]->bIndoor || !furniture_weight_map.regulation_map[row_index][rank_index]->bInside || furniture_weight_map.regulation_map[row_index][rank_index]->bInfloorbox)
			{
				continue;
			}
			/** ↑ 0° */
			vector<Point3f> furniture_points_0;
			/** 构造家具覆盖区域（误差内）*/
			furniture_points_0.push_back(Point3f(furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.x - error_range, furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.y + error_range, 0));
			furniture_points_0.push_back(Point3f(furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.x - error_range, furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.y + furniture.width - error_range, 0));
			furniture_points_0.push_back(Point3f(furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.x - furniture.length + error_range, furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.y + furniture.width - error_range, 0));
			furniture_points_0.push_back(Point3f(furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.x - furniture.length + error_range, furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.y + error_range, 0));

			if (ComMath::PolygonInPolygon(furniture_points_0, region_resolver->room_corner))
			{
				/** 初始化数据 */
				float temp_vaild_score_0 = 0.0f;
				float temp_vaild_score_180 = 0.0f;

				/** 计算旋转中心 */
				Point3f temp_translation_point = Point3f(furniture_points_0[0].x + error_range - furniture.length*0.5f, furniture_points_0[0].y - error_range + furniture.width*0.5f, 0);
				float temp_vaild_score = 0.0f;
				int temp_rotation_degree = 0;

				/** 家具覆盖区域的行列 */
				int row_count = furniture_row_num + row_index;
				int rank_count = furniture_rank_num + rank_index;

				/** 在门区域的格子数量 */
				int in_door_amount = 0;

				/** 判断是否在地表内 */
				bool in_floor_box = false;

				/** 计算区域内分数，0，180度计算 */
				for (int temp_row_index = row_index; temp_row_index < row_count; ++temp_row_index)
				{
					/** 占门区域超过8格终止循环 */
					if (in_door_amount > region_resolver->door_region_depth)
					{
						break;
					}
					if (in_floor_box == true)
					{
						break;
					}

					for (int temp_rank_index = rank_index; temp_rank_index < rank_count; ++temp_rank_index)
					{
						/** 判断格子是否在门区域内 */
						if (furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->bIndoor)
						{
							++in_door_amount;
						}

						/** 占门区域8格内则进行权重检索 */
						if (in_door_amount <= region_resolver->door_region_depth)
						{
							if (temp_row_index == row_index)
							{
								temp_vaild_score_0 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight*10.f;
								temp_vaild_score_180 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight;
							}
							else if (temp_row_index == row_count - 1)
							{
								temp_vaild_score_0 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight;
								temp_vaild_score_180 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight*10.f;
							}
							else
							{
								temp_vaild_score_180 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight;
								temp_vaild_score_0 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight;
							}
						}

						/** 超过8格终止循环 */
						if (in_door_amount > region_resolver->door_region_depth)
						{
							temp_vaild_score_0 = 0.0f;
							temp_vaild_score_180 = 0.0f;
							break;
						}

						/**  判断格子是否在地标区域内 */
						if (furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->bInfloorbox)
						{
							temp_vaild_score_0 = 0.0f;
							temp_vaild_score_180 = 0.0f;
							in_floor_box = true;
							break;
						}
					}
				}

				/** 将结果赋予临时变量 */
				temp_vaild_score = (temp_vaild_score_0 >= temp_vaild_score_180) ? temp_vaild_score_0 : temp_vaild_score_180;
				temp_rotation_degree = (temp_vaild_score_0 >= temp_vaild_score_180) ? 180 : 0;

				/** 如果分数大于所存分数，结果替换 */
				if (temp_vaild_score > min_score)
				{
					int rule_index = (int)single_furniture_weight_data.size() - 1;
					min_score_index = GetMinScoreIndex(single_furniture_weight_data[rule_index].vaild_scores);
					single_furniture_weight_data[rule_index].vaild_scores[min_score_index] = temp_vaild_score;
					single_furniture_weight_data[rule_index].translation_points[min_score_index] = temp_translation_point;
					single_furniture_weight_data[rule_index].rotation_degrees[min_score_index] = temp_rotation_degree;
					single_furniture_weight_data[rule_index].furniture_vertex_pointss[min_score_index] = furniture_points_0;
					single_furniture_weight_data[rule_index].model_ids[min_score_index] = furniture.id;
					single_furniture_weight_data[rule_index].widths[min_score_index] = furniture.width;
					single_furniture_weight_data[rule_index].rule_nos[min_score_index] = furniture_weight_map.rule_no;

					/** 将分数新的最小值赋给min_score */
					min_score_index = GetMinScoreIndex(single_furniture_weight_data[rule_index].vaild_scores);
					min_score = single_furniture_weight_data[rule_index].vaild_scores[min_score_index];
				}
			}

			furniture_points_0.clear();
		}
	}
}

void MetisDynamicLayout::TraveseRoomLeaf_Independent_270(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_weight_data, FurnitureWeight& furniture_weight_map, Model & furniture, shared_ptr<MetisRegionResolver>& region_resolver, float & min_score, int & min_score_index)
{
	/** 家具的行列数量 */
	int furniture_rank_num = static_cast<int>(((int)furniture.length % region_resolver->BoxSize == 0) ? furniture.length / region_resolver->BoxSize : furniture.length / region_resolver->BoxSize + 1);
	int furniture_row_num = static_cast<int>(((int)furniture.width % region_resolver->BoxSize == 0) ? furniture.width / region_resolver->BoxSize : furniture.width / region_resolver->BoxSize + 1);

	/** 提供缩小覆盖区域的误差 */
	float error_range = 0.1f;

	// 无需整体遍历，只需要遍历部分即可
	int row_count = region_resolver->max_row - furniture_row_num + 1;
	int rank_count = region_resolver->max_rank - furniture_rank_num + 1;

	for (int row_index = 0; row_index < row_count; ++row_index)
	{
		for (int rank_index = 0; rank_index < rank_count; ++rank_index)
		{
			/********************************************
			* vector 存储点位顺序
			*   [0]top_left        [1]top_right
			*
			*
			*
			*
			*	[3]bottom_right    [2]bottom_right
			********************************************/
			//判断是否本身就是门点或者不在区域内 或者在地标区域内
			if (furniture_weight_map.regulation_map[row_index][rank_index]->bIndoor || !furniture_weight_map.regulation_map[row_index][rank_index]->bInside || furniture_weight_map.regulation_map[row_index][rank_index]->bInfloorbox)
			{
				continue;
			}
			/** ← 270° */
			vector<Point3f> furniture_points_270;
			/** 构造家具覆盖区域（误差内） */
			furniture_points_270.push_back(Point3f(furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.x - error_range, furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.y + error_range, 0));
			furniture_points_270.push_back(Point3f(furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.x - error_range, furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.y + furniture.length - error_range, 0));
			furniture_points_270.push_back(Point3f(furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.x - furniture.width + error_range, furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.y + furniture.length - error_range, 0));
			furniture_points_270.push_back(Point3f(furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.x - furniture.width + error_range, furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.y + error_range, 0));

			if (ComMath::PolygonInPolygon(furniture_points_270, region_resolver->room_corner))
			{
				/** 初始化数据 */
				float temp_vaild_score_270 = 0.0f;
				float temp_vaild_score_90 = 0.0f;

				/** 计算旋转中心 */
				Point3f temp_translation_point = Point3f(furniture_points_270[0].x + error_range - furniture.width*0.5f, furniture_points_270[0].y - error_range + furniture.length*0.5f, 0);
				float temp_vaild_score = 0.0f;
				int temp_rotation_degree = 0;

				/** 家具覆盖区域的行列 */
				int row_count = furniture_row_num + row_index;
				int rank_count = furniture_rank_num + rank_index;

				/** 在门区域的格子数量 */
				int in_door_amount = 0;

				/** 判断是否在地表内 */
				bool in_floor_box = false;

				/** 计算区域内分数，0，180度计算 */
				for (int temp_row_index = row_index; temp_row_index < row_count; ++temp_row_index)
				{
					/** 占门区域超过8格终止循环 */
					if (in_door_amount > region_resolver->door_region_depth)
					{
						break;
					}
					if (in_floor_box == true)
					{
						break;
					}

					for (int temp_rank_index = rank_index; temp_rank_index < rank_count; ++temp_rank_index)
					{
						/** 判断格子是否在门区域内 */
						if (furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->bIndoor)
						{
							++in_door_amount;
						}

						/** 占门区域8格内则进行权重检索 */
						if (in_door_amount <= region_resolver->door_region_depth)
						{
							if (temp_rank_index == rank_index)
							{
								temp_vaild_score_270 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight*10.f;
								temp_vaild_score_90 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight;
							}
							else if (temp_rank_index == rank_count - 1)
							{
								temp_vaild_score_270 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight;
								temp_vaild_score_90 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight*10.f;
							}
							else
							{
								temp_vaild_score_270 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight;
								temp_vaild_score_90 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight;
							}
						}

						/** 超过8格终止循环 */
						if (in_door_amount > region_resolver->door_region_depth)
						{
							temp_vaild_score_270 = 0.0f;
							temp_vaild_score_90 = 0.0f;
							break;
						}
						/**  判断格子是否在地标区域内 */
						if (furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->bInfloorbox)
						{
							temp_vaild_score_270 = 0.0f;
							temp_vaild_score_90 = 0.0f;
							in_floor_box = true;
							break;
						}
					}
				}

				/** 将结果赋予临时变量 */
				temp_vaild_score = (temp_vaild_score_270 >= temp_vaild_score_90) ? temp_vaild_score_270 : temp_vaild_score_90;
				temp_rotation_degree = (temp_vaild_score_270 >= temp_vaild_score_90) ? 90 : 270;

				/** 如果分数大于所存分数，结果替换 */
				if (temp_vaild_score > min_score)
				{
					int rule_index = (int)single_furniture_weight_data.size() - 1;
					min_score_index = GetMinScoreIndex(single_furniture_weight_data[rule_index].vaild_scores);
					single_furniture_weight_data[rule_index].vaild_scores[min_score_index] = temp_vaild_score;
					single_furniture_weight_data[rule_index].translation_points[min_score_index] = temp_translation_point;
					single_furniture_weight_data[rule_index].rotation_degrees[min_score_index] = temp_rotation_degree;
					single_furniture_weight_data[rule_index].furniture_vertex_pointss[min_score_index] = furniture_points_270;
					single_furniture_weight_data[rule_index].model_ids[min_score_index] = furniture.id;
					single_furniture_weight_data[rule_index].widths[min_score_index] = furniture.width;
					single_furniture_weight_data[rule_index].rule_nos[min_score_index] = furniture_weight_map.rule_no;

					/** 将分数新的最小值赋给min_score */
					min_score_index = GetMinScoreIndex(single_furniture_weight_data[rule_index].vaild_scores);
					min_score = single_furniture_weight_data[rule_index].vaild_scores[min_score_index];
				}
			}

			furniture_points_270.clear();
		}
	}
}

void MetisDynamicLayout::PickAttachSuitableResult(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_weight_data, FurnitureWeight& furniture_weight_map, Model &furniture, shared_ptr<MetisRegionResolver> &region_resolver)
{
	region_resolver->AddAttachLayoutDataArray(single_furniture_weight_data);

	float min_score = -999999.0f;
	int min_score_index;

	TraveseRoomLeaf_Attach_0(single_furniture_weight_data, furniture_weight_map, furniture, region_resolver, min_score, min_score_index);
	TraveseRoomLeaf_Attach_270(single_furniture_weight_data, furniture_weight_map, furniture, region_resolver, min_score, min_score_index);

	SortExcellent(single_furniture_weight_data);
}

void MetisDynamicLayout::TraveseRoomLeaf_Attach_0(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_weight_data, FurnitureWeight& furniture_weight_map, Model &furniture, shared_ptr<MetisRegionResolver> &region_resolver, float &min_score, int &min_score_index)
{
	/** 家具的行列数量 */
	int furniture_row_num = static_cast<int>(((int)furniture.length % region_resolver->BoxSize == 0) ? furniture.length / region_resolver->BoxSize : furniture.length / region_resolver->BoxSize + 1);
	int furniture_rank_num = static_cast<int>(((int)furniture.width % region_resolver->BoxSize == 0) ? furniture.width / region_resolver->BoxSize : furniture.width / region_resolver->BoxSize + 1);

	/** 提供缩小覆盖区域的误差 */
	float error_range = 0.1f;

	// 无需整体遍历，只需要遍历部分即可
	int row_count = region_resolver->max_row - furniture_row_num + 1;
	int rank_count = region_resolver->max_rank - furniture_rank_num + 1;

	for (int row_index = 0; row_index < row_count; ++row_index)
	{
		for (int rank_index = 0; rank_index < rank_count; ++rank_index)
		{

			/********************************************
				* vector 存储点位顺序
				*   [0]top_left        [1]top_right
				*
				*
				*
				*
				*	[3]bottom_right    [2]bottom_right
			********************************************/
			//判断是否本身就是门点或者不在区域内 或者在地标区域内
			if (furniture_weight_map.regulation_map[row_index][rank_index]->bIndoor || !furniture_weight_map.regulation_map[row_index][rank_index]->bInside || furniture_weight_map.regulation_map[row_index][rank_index]->bInfloorbox)
			{
				continue;
			}
			/** ↑ 0° */
			vector<Point3f> furniture_points_0;
			/** 构造家具覆盖区域（误差内） */
			furniture_points_0.push_back(Point3f(furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.x - error_range, furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.y + error_range, 0));
			furniture_points_0.push_back(Point3f(furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.x - error_range, furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.y + furniture.width - error_range, 0));
			furniture_points_0.push_back(Point3f(furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.x - furniture.length + error_range, furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.y + furniture.width - error_range, 0));
			furniture_points_0.push_back(Point3f(furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.x - furniture.length + error_range, furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.y + error_range, 0));

			if (ComMath::PolygonInPolygon(furniture_points_0, region_resolver->room_corner))
			{
				/** 初始化数据 */
				float temp_vaild_score_0 = 0.0f;
				float temp_vaild_score_180 = 0.0f;

				/** 计算旋转中心 */
				Point3f temp_translation_point = Point3f(furniture_points_0[0].x + error_range - furniture.length*0.5f, furniture_points_0[0].y - error_range + furniture.width*0.5f, 0);
				float temp_vaild_score = 0.0f;
				int temp_rotation_degree = 0;

				/** 家具覆盖区域的行列 */
				int row_count = furniture_row_num + row_index;
				int rank_count = furniture_rank_num + rank_index;

				/** 在门区域的格子数量 */
				int in_door_amount = 0;

				/** 判断是否在地表内 */
				bool in_floor_box = false;

				/** 计算区域内分数，0，180度计算 */
				for (int temp_row_index = row_index; temp_row_index < row_count; ++temp_row_index)
				{
					/** 占门区域超过8格终止循环 */
					if (in_door_amount > region_resolver->door_region_depth)
					{
						break;
					}
					if (in_floor_box == true)
					{
						break;
					}

					for (int temp_rank_index = rank_index; temp_rank_index < rank_count; ++temp_rank_index)
					{
						/** 判断格子是否在门区域内 */
						if (furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->bIndoor)
						{
							++in_door_amount;
						}

						/** 占门区域8格内则进行权重检索 */
						if (in_door_amount <= region_resolver->door_region_depth)
						{
							if (temp_row_index == row_index)
							{
								temp_vaild_score_0 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight*10.f;
								temp_vaild_score_180 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight;
							}
							else if (temp_row_index == row_count - 1)
							{
								temp_vaild_score_0 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight;
								temp_vaild_score_180 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight*10.f;
							}
							else
							{
								temp_vaild_score_180 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight;
								temp_vaild_score_0 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight;
							}
						}

						/** 超过8格终止循环 */
						if (in_door_amount > region_resolver->door_region_depth)
						{
							temp_vaild_score_0 = 0.0f;
							temp_vaild_score_180 = 0.0f;
							break;
						}
						/**  判断格子是否在地标区域内 */
						if (furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->bInfloorbox)
						{
							temp_vaild_score_0 = 0.0f;
							temp_vaild_score_180 = 0.0f;
							in_floor_box = true;
							break;
						}
					}
				}

				/** 将结果赋予临时变量 */
				temp_vaild_score = (temp_vaild_score_0 >= temp_vaild_score_180) ? temp_vaild_score_0 : temp_vaild_score_180;
				temp_rotation_degree = (temp_vaild_score_0 >= temp_vaild_score_180) ? 180 : 0;

				/** 如果分数大于所存分数，结果替换 */
				if (temp_vaild_score > min_score)
				{
					int rule_index = (int)single_furniture_weight_data.size() - 1;
					min_score_index = GetMinScoreIndex(single_furniture_weight_data[rule_index].vaild_scores);
					single_furniture_weight_data[rule_index].vaild_scores[min_score_index] = temp_vaild_score;
					single_furniture_weight_data[rule_index].translation_points[min_score_index] = temp_translation_point;
					single_furniture_weight_data[rule_index].rotation_degrees[min_score_index] = temp_rotation_degree;
					single_furniture_weight_data[rule_index].furniture_vertex_pointss[min_score_index] = furniture_points_0;
					single_furniture_weight_data[rule_index].model_ids[min_score_index] = furniture.id;
					single_furniture_weight_data[rule_index].widths[min_score_index] = furniture.width;
					single_furniture_weight_data[rule_index].rule_nos[min_score_index] = furniture_weight_map.rule_no;

					/** 将分数新的最小值赋给min_score */
					min_score_index = GetMinScoreIndex(single_furniture_weight_data[rule_index].vaild_scores);
					min_score = single_furniture_weight_data[rule_index].vaild_scores[min_score_index];
				}
			}

			furniture_points_0.clear();
		}
	}
}

void MetisDynamicLayout::TraveseRoomLeaf_Attach_270(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_weight_data, FurnitureWeight& furniture_weight_map, Model & furniture, shared_ptr<MetisRegionResolver>& region_resolver, float & min_score, int & min_score_index)
{
	/** 家具的行列数量 */
	int furniture_rank_num = static_cast<int>(((int)furniture.length % region_resolver->BoxSize == 0) ? furniture.length / region_resolver->BoxSize : furniture.length / region_resolver->BoxSize + 1);
	int furniture_row_num = static_cast<int>(((int)furniture.width % region_resolver->BoxSize == 0) ? furniture.width / region_resolver->BoxSize : furniture.width / region_resolver->BoxSize + 1);

	/** 提供缩小覆盖区域的误差 */
	float error_range = 0.1f;

	// 无需整体遍历，只需要遍历部分即可
	int row_count = region_resolver->max_row - furniture_row_num + 1;
	int rank_count = region_resolver->max_rank - furniture_rank_num + 1;

	for (int row_index = 0; row_index < row_count; ++row_index)
	{
		for (int rank_index = 0; rank_index < rank_count; ++rank_index)
		{
			/********************************************
			* vector 存储点位顺序
			*   [0]top_left        [1]top_right
			*
			*
			*
			*
			*	[3]bottom_right    [2]bottom_right
			********************************************/
			//判断是否本身就是门点或者不在区域内 或者在地标区域内
			if (furniture_weight_map.regulation_map[row_index][rank_index]->bIndoor || !furniture_weight_map.regulation_map[row_index][rank_index]->bInside || furniture_weight_map.regulation_map[row_index][rank_index]->bInfloorbox)
			{
				continue;
			}
			/** ← 270° */
			vector<Point3f> furniture_points_270;

			/** 构造家具覆盖区域（误差内） */
			furniture_points_270.push_back(Point3f(furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.x - error_range, furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.y + error_range, 0));
			furniture_points_270.push_back(Point3f(furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.x - error_range, furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.y + furniture.length - error_range, 0));
			furniture_points_270.push_back(Point3f(furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.x - furniture.width + error_range, furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.y + furniture.length - error_range, 0));
			furniture_points_270.push_back(Point3f(furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.x - furniture.width + error_range, furniture_weight_map.regulation_map[row_index][rank_index]->top_left_point.y + error_range, 0));

			if (ComMath::PolygonInPolygon(furniture_points_270, region_resolver->room_corner))
			{
				/** 初始化数据 */
				float temp_vaild_score_270 = 0.0f;
				float temp_vaild_score_90 = 0.0f;

				/** 计算旋转中心 */
				Point3f temp_translation_point = Point3f(furniture_points_270[0].x + error_range - furniture.width*0.5f, furniture_points_270[0].y - error_range + furniture.length*0.5f, 0);
				float temp_vaild_score = 0.0f;
				int temp_rotation_degree = 0;

				/** 家具覆盖区域的行列 */
				int row_count = furniture_row_num + row_index;
				int rank_count = furniture_rank_num + rank_index;

				/** 在门区域的格子数量 */
				int in_door_amount = 0;

				/** 判断是否在地表内 */
				bool in_floor_box = false;

				/** 计算区域内分数，0，180度计算 */
				for (int temp_row_index = row_index; temp_row_index < row_count; ++temp_row_index)
				{
					/** 占门区域超过8格终止循环 */
					if (in_door_amount > region_resolver->door_region_depth)
					{
						break;
					}
					if (in_floor_box == true)
					{
						break;
					}

					for (int temp_rank_index = rank_index; temp_rank_index < rank_count; ++temp_rank_index)
					{
						/** 判断格子是否在门区域内 */
						if (furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->bIndoor)
						{
							++in_door_amount;
						}

						/** 占门区域8格内则进行权重检索 */
						if (in_door_amount <= region_resolver->door_region_depth)
						{
							if (temp_rank_index == rank_index)
							{
								temp_vaild_score_270 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight*10.f;
								temp_vaild_score_90 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight;
							}
							else if (temp_rank_index == rank_count - 1)
							{
								temp_vaild_score_270 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight;
								temp_vaild_score_90 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight*10.f;
							}
							else
							{
								temp_vaild_score_270 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight;
								temp_vaild_score_90 += furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->Weight;
							}
						}

						/** 超过8格终止循环 */
						if (in_door_amount > region_resolver->door_region_depth)
						{
							temp_vaild_score_270 = 0.0f;
							temp_vaild_score_90 = 0.0f;
							break;
						}
						/**  判断格子是否在地标区域内 */
						if (furniture_weight_map.regulation_map[temp_row_index][temp_rank_index]->bInfloorbox)
						{
							temp_vaild_score_270 = 0.0f;
							temp_vaild_score_90 = 0.0f;
							in_floor_box = true;
							break;
						}
					}
				}

				/** 将结果赋予临时变量 */
				temp_vaild_score = (temp_vaild_score_270 >= temp_vaild_score_90) ? temp_vaild_score_270 : temp_vaild_score_90;
				temp_rotation_degree = (temp_vaild_score_270 >= temp_vaild_score_90) ? 90 : 270;

				/** 如果分数大于所存分数，结果替换 */
				if (temp_vaild_score > min_score)
				{
					int rule_index = (int)single_furniture_weight_data.size() - 1;
					min_score_index = GetMinScoreIndex(single_furniture_weight_data[rule_index].vaild_scores);
					single_furniture_weight_data[rule_index].vaild_scores[min_score_index] = temp_vaild_score;
					single_furniture_weight_data[rule_index].translation_points[min_score_index] = temp_translation_point;
					single_furniture_weight_data[rule_index].rotation_degrees[min_score_index] = temp_rotation_degree;
					single_furniture_weight_data[rule_index].furniture_vertex_pointss[min_score_index] = furniture_points_270;
					single_furniture_weight_data[rule_index].model_ids[min_score_index] = furniture.id;
					single_furniture_weight_data[rule_index].widths[min_score_index] = furniture.width;
					single_furniture_weight_data[rule_index].rule_nos[min_score_index] = furniture_weight_map.rule_no;

					/** 将分数新的最小值赋给min_score */
					min_score_index = GetMinScoreIndex(single_furniture_weight_data[rule_index].vaild_scores);
					min_score = single_furniture_weight_data[rule_index].vaild_scores[min_score_index];
				}
			}

			furniture_points_270.clear();
		}
	}
}

int MetisDynamicLayout::GetMinScoreIndex(vector<float>& vaild_score)
{
	float min_score = vaild_score[0];
	int min_score_index = 0;

	/** 遍历容器内分数 */
	for (int i = 0; i < vaild_score.size(); ++i)
	{
		/** 找到最小分数的index */
		if (vaild_score[i] < min_score)
		{
			min_score = vaild_score[i];
			min_score_index = i;
		}
	}
	return min_score_index;
}

void MetisDynamicLayout::SortExcellent(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_all_rule_layout_data)
{
	int rule_index = (int)single_furniture_all_rule_layout_data.size() - 1;
	for (int i = 1; i < single_furniture_all_rule_layout_data[rule_index].vaild_scores.size(); ++i)
	{
		for (int j = i; j > 0; --j)
		{
			/** 冒泡，按照分数最大排序 */
			if (single_furniture_all_rule_layout_data[rule_index].vaild_scores[j] > single_furniture_all_rule_layout_data[rule_index].vaild_scores[j - 1])
			{
				/** 交换 */
				float temp_vaild_score = single_furniture_all_rule_layout_data[rule_index].vaild_scores[j];
				Point3f temp_translation_point = single_furniture_all_rule_layout_data[rule_index].translation_points[j];
				int temp_rotation_degree = single_furniture_all_rule_layout_data[rule_index].rotation_degrees[j];
				vector<Point3f> temp_furniture_points = single_furniture_all_rule_layout_data[rule_index].furniture_vertex_pointss[j];
				int temp_model_id = single_furniture_all_rule_layout_data[rule_index].model_ids[j];
				float temp_width = single_furniture_all_rule_layout_data[rule_index].widths[j];
				string temp_rule_no = single_furniture_all_rule_layout_data[rule_index].rule_nos[j];

				single_furniture_all_rule_layout_data[rule_index].vaild_scores[j] = single_furniture_all_rule_layout_data[rule_index].vaild_scores[j - 1];
				single_furniture_all_rule_layout_data[rule_index].translation_points[j] = single_furniture_all_rule_layout_data[rule_index].translation_points[j - 1];
				single_furniture_all_rule_layout_data[rule_index].rotation_degrees[j] = single_furniture_all_rule_layout_data[rule_index].rotation_degrees[j - 1];
				single_furniture_all_rule_layout_data[rule_index].furniture_vertex_pointss[j] = single_furniture_all_rule_layout_data[rule_index].furniture_vertex_pointss[j - 1];
				single_furniture_all_rule_layout_data[rule_index].model_ids[j] = single_furniture_all_rule_layout_data[rule_index].model_ids[j - 1];
				single_furniture_all_rule_layout_data[rule_index].widths[j] = single_furniture_all_rule_layout_data[rule_index].widths[j - 1];
				single_furniture_all_rule_layout_data[rule_index].rule_nos[j] = single_furniture_all_rule_layout_data[rule_index].rule_nos[j - 1];

				single_furniture_all_rule_layout_data[rule_index].vaild_scores[j - 1] = temp_vaild_score;
				single_furniture_all_rule_layout_data[rule_index].translation_points[j - 1] = temp_translation_point;
				single_furniture_all_rule_layout_data[rule_index].rotation_degrees[j - 1] = temp_rotation_degree;
				single_furniture_all_rule_layout_data[rule_index].furniture_vertex_pointss[j - 1] = temp_furniture_points;
				single_furniture_all_rule_layout_data[rule_index].model_ids[j - 1] = temp_model_id;
				single_furniture_all_rule_layout_data[rule_index].widths[j - 1] = temp_width;
				single_furniture_all_rule_layout_data[rule_index].rule_nos[j - 1] = temp_rule_no;
			}
		}
	}
}


void MetisDynamicLayout::CalculateIndependentFurnitureSingleRule(shared_ptr<MetisRegionResolver>& region_resolver)
{
	/** 临时单个家具所有规则的布局map */
	map<int, SingleFurnitureAllRuleLayoutData> temp_single_furniture_all_map;

	/** 匹配独立性家具规则，生成独立性方案 */
	MatchIndependentAllRule(0, temp_single_furniture_all_map, region_resolver);

	/** 临时单个家具单个规则的数据Map */
	map<int, SingleFurnitureSingleRuleLayoutData> temp_single_furniture_map;

	/** 遍历所有规则方案，进行组合*/
	for (auto &single_rule_layout : region_resolver->independent_furniture_rule_layout)
	{
		TraveseIndependentFurnitureSingleRule(single_rule_layout, 0, temp_single_furniture_map, region_resolver);
		temp_single_furniture_map.clear();
	}

	/** 筛选最终独立性家具方案（重叠删除） */
	PickIndependentFurnitureLayout(region_resolver);
}

void MetisDynamicLayout::MatchIndependentAllRule(int furniture_index, map<int, SingleFurnitureAllRuleLayoutData> temp_single_furniture_all_map, shared_ptr<MetisRegionResolver>& region_resolver)
{
	//到最后一个了
	if (furniture_index == region_resolver->independent_model_container.size())
	{
		region_resolver->independent_furniture_rule_layout.push_back(temp_single_furniture_all_map);
		return;
	}

	/** 临时家具Id,index 用于递归判定 */
	int temp_furniture_id = region_resolver->independent_model_container[furniture_index].id;
	int temp_furniture_index = furniture_index + 1;

	/** 独立性家具方案进行组合 */
	for (int i = 0; i < region_resolver->independent_furniture_all_rule_weight[temp_furniture_id].size(); ++i)
	{
		SingleFurnitureAllRuleLayoutData temp_data;

		temp_data.vaild_scores = region_resolver->independent_furniture_all_rule_weight[temp_furniture_id][i].vaild_scores;
		temp_data.translation_points = region_resolver->independent_furniture_all_rule_weight[temp_furniture_id][i].translation_points;
		temp_data.rotation_degrees = region_resolver->independent_furniture_all_rule_weight[temp_furniture_id][i].rotation_degrees;
		temp_data.furniture_vertex_pointss = region_resolver->independent_furniture_all_rule_weight[temp_furniture_id][i].furniture_vertex_pointss;
		temp_data.model_ids = region_resolver->independent_furniture_all_rule_weight[temp_furniture_id][i].model_ids;
		temp_data.widths = region_resolver->independent_furniture_all_rule_weight[temp_furniture_id][i].widths;
		temp_data.rule_nos = region_resolver->independent_furniture_all_rule_weight[temp_furniture_id][i].rule_nos;

		map<int, SingleFurnitureAllRuleLayoutData> temp_single_furniture_map1 = temp_single_furniture_all_map;
		temp_single_furniture_map1.insert(pair<int, SingleFurnitureAllRuleLayoutData>(temp_furniture_id, temp_data));

		MatchIndependentAllRule(temp_furniture_index, temp_single_furniture_map1, region_resolver);
	}
}

void MetisDynamicLayout::TraveseIndependentFurnitureSingleRule(map<int, SingleFurnitureAllRuleLayoutData>& single_rule_layout, int furniture_index, map<int, SingleFurnitureSingleRuleLayoutData> temp_single_furniture_map, shared_ptr<MetisRegionResolver>& region_resolver)
{
	//到最后一个了
	if (furniture_index == region_resolver->independent_model_container.size())
	{
		region_resolver->independent_furniture_layout_combo_data.push_back(temp_single_furniture_map);
		return;
	}

	/** 临时家具Id,index 用于递归判定 */
	int temp_furniture_id = region_resolver->independent_model_container[furniture_index].id;
	int temp_furniture_index = furniture_index + 1;

	/** 5套方案进行组合 */
	for (int i = 0; i < solution_num; ++i)
	{
		SingleFurnitureSingleRuleLayoutData temp_data;
		temp_data.vaild_score = single_rule_layout[temp_furniture_id].vaild_scores[i];
		temp_data.translation_point = single_rule_layout[temp_furniture_id].translation_points[i];
		temp_data.rotation_degree = single_rule_layout[temp_furniture_id].rotation_degrees[i];
		temp_data.furniture_vertex_points = single_rule_layout[temp_furniture_id].furniture_vertex_pointss[i];
		temp_data.model_id = single_rule_layout[temp_furniture_id].model_ids[i];
		temp_data.width = single_rule_layout[temp_furniture_id].widths[i];
		temp_data.rule_no = single_rule_layout[temp_furniture_id].rule_nos[i];

		map<int, SingleFurnitureSingleRuleLayoutData> temp_single_furniture_map1 = temp_single_furniture_map;
		temp_single_furniture_map1.insert(pair<int, SingleFurnitureSingleRuleLayoutData>(temp_furniture_id, temp_data));

		TraveseIndependentFurnitureSingleRule(single_rule_layout, temp_furniture_index, temp_single_furniture_map1, region_resolver);
	}
}

void MetisDynamicLayout::PickIndependentFurnitureLayout(shared_ptr<MetisRegionResolver>& region_resolver)
{
	/** 家具的所有id */
	vector<int> furniture_id;

	/** 提取家具ID */
	for (auto&furniture : region_resolver->independent_model_container)
	{
		furniture_id.push_back(furniture.id);
	}

	/** 独立性家具的布局方案 */
	vector<map<int, SingleFurnitureSingleRuleLayoutData>> temp_all_furniture_weight_combo_data = region_resolver->independent_furniture_layout_combo_data;
	region_resolver->independent_furniture_layout_combo_data.clear();

	/** 进行方案组合 */
	for (auto& single_combo_map : temp_all_furniture_weight_combo_data)
	{
		/** 家具重叠  初始化 */
		bool furniture_overlap = false;

		/** 家具进行比较是否重叠 */
		for (int i = 0; i < furniture_id.size(); ++i)
		{
			for (int j = i + 1; j < furniture_id.size(); ++j)
			{
				/** 检测是否重叠 */
				if (ComMath::IsCollisionRect(single_combo_map[furniture_id[i]].furniture_vertex_points, single_combo_map[furniture_id[j]].furniture_vertex_points))
				{
					furniture_overlap = true;
					break;
				}
			}

			if (furniture_overlap)
			{
				break;
			}
		}

		/** 不重叠，提取组合布局 */
		if (!furniture_overlap)
		{
			region_resolver->independent_furniture_layout_combo_data.push_back(single_combo_map);
		}
	}
}

void MetisDynamicLayout::MatchAttachFurniture(shared_ptr<MetisRegionResolver>& region_resolver)
{
	/** 总布局 */
	region_resolver->furniture_layout_result_vector.push_back(region_resolver->independent_furniture_layout_combo_data);
	int attach_furniture_index = 1;

	for (auto &attach_furntiure : region_resolver->attach_model_container)
	{
		attach_furniture_index++;
		/** 临时的所有布局方案 */
		vector<map<int, SingleFurnitureSingleRuleLayoutData>> temp_furniture_layout;

		/**  最终的布局赋给临时布局容器 */
		vector< vector<map<int, SingleFurnitureSingleRuleLayoutData>> > temp_furniture_layout_result_vector = region_resolver->furniture_layout_result_vector;
		region_resolver->furniture_layout_result_vector.clear();

		/** 针对多套规则生成的布局来匹配 */
		for (int layout_result_index = 0; layout_result_index < temp_furniture_layout_result_vector.size(); ++layout_result_index)
		{
			/** 存储备用 */
			vector<map<int, SingleFurnitureSingleRuleLayoutData>> temp_independent_all_furniture_weight_combo_data = temp_furniture_layout_result_vector[layout_result_index];

			/** 每个依附性规则都是根据独立性规则对应生成，开始匹配 */
			for (int attach_weight_index = 0; attach_weight_index < region_resolver->attach_furniture_all_rule_weight[0][attach_furntiure.id].size(); ++attach_weight_index)
			{
				for (int program_index = 0; program_index < temp_independent_all_furniture_weight_combo_data.size(); ++program_index)
				{
					/** 依附性规则都要求生成一整套布局 */
					for (int store_index = 0; store_index < solution_num; ++store_index)
					{
						bool furntiure_overlap = false;

						for (auto& persent_furntiure : region_resolver->model_container)
						{
							if (ComMath::IsCollisionRect(temp_independent_all_furniture_weight_combo_data[program_index][persent_furntiure.id].furniture_vertex_points,
								region_resolver->attach_furniture_all_rule_weight[program_index][attach_furntiure.id][attach_weight_index].furniture_vertex_pointss[store_index])

								|| region_resolver->attach_furniture_all_rule_weight[program_index][attach_furntiure.id][attach_weight_index].model_ids[store_index] == 0)
							{
								furntiure_overlap = true;
								break;
							}
						}

						/** 若重叠且循环到最后一个，没有合适的匹配方案，则进行删除 */
						if (furntiure_overlap == true && store_index == solution_num - 1)
						{
							SingleFurnitureSingleRuleLayoutData temp_attach_furniture_layout;
							temp_attach_furniture_layout.vaild_score = 10.f;
							float temp_point = 1000000 + 2000 * (float)attach_furniture_index;
							temp_attach_furniture_layout.translation_point = Point3f(temp_point, temp_point, 0);
							temp_attach_furniture_layout.rotation_degree = 0;
							vector<Point3f> temp_attach_furniture_vertex_points;
							temp_attach_furniture_vertex_points.push_back(Point3f(temp_point + 1.f, temp_point - 1.f, 0));
							temp_attach_furniture_vertex_points.push_back(Point3f(temp_point + 1.f, temp_point + 1.f, 0));
							temp_attach_furniture_vertex_points.push_back(Point3f(temp_point - 1.f, temp_point + 1.f, 0));
							temp_attach_furniture_vertex_points.push_back(Point3f(temp_point - 1.f, temp_point - 1.f, 0));
							temp_attach_furniture_layout.furniture_vertex_points = temp_attach_furniture_vertex_points;
							temp_attach_furniture_layout.model_id = 0;
							temp_attach_furniture_layout.width = 10;
							temp_attach_furniture_layout.rule_no = "value";

							map<int, SingleFurnitureSingleRuleLayoutData> temp_layout_weight_combo = temp_independent_all_furniture_weight_combo_data[program_index];

							temp_layout_weight_combo.insert(make_pair(attach_furntiure.id, temp_attach_furniture_layout));

							temp_furniture_layout.push_back(temp_layout_weight_combo);
							temp_layout_weight_combo.clear();
							break;
						}

						/** 若不重叠，添加布局方案 */
						if (!furntiure_overlap)
						{
							SingleFurnitureSingleRuleLayoutData temp_attach_furniture_layout;
							temp_attach_furniture_layout.vaild_score = region_resolver->attach_furniture_all_rule_weight[program_index][attach_furntiure.id][attach_weight_index].vaild_scores[store_index];
							temp_attach_furniture_layout.translation_point = region_resolver->attach_furniture_all_rule_weight[program_index][attach_furntiure.id][attach_weight_index].translation_points[store_index];
							temp_attach_furniture_layout.rotation_degree = region_resolver->attach_furniture_all_rule_weight[program_index][attach_furntiure.id][attach_weight_index].rotation_degrees[store_index];
							temp_attach_furniture_layout.furniture_vertex_points = region_resolver->attach_furniture_all_rule_weight[program_index][attach_furntiure.id][attach_weight_index].furniture_vertex_pointss[store_index];
							temp_attach_furniture_layout.model_id = region_resolver->attach_furniture_all_rule_weight[program_index][attach_furntiure.id][attach_weight_index].model_ids[store_index];
							temp_attach_furniture_layout.width = region_resolver->attach_furniture_all_rule_weight[program_index][attach_furntiure.id][attach_weight_index].widths[store_index];
							temp_attach_furniture_layout.rule_no = region_resolver->attach_furniture_all_rule_weight[program_index][attach_furntiure.id][attach_weight_index].rule_nos[store_index];

							map<int, SingleFurnitureSingleRuleLayoutData> temp_layout_weight_combo = temp_independent_all_furniture_weight_combo_data[program_index];

							temp_layout_weight_combo.insert(make_pair(attach_furntiure.id, temp_attach_furniture_layout));

							temp_furniture_layout.push_back(temp_layout_weight_combo);
							temp_layout_weight_combo.clear();
							break;
						}
					}
				}

				region_resolver->furniture_layout_result_vector.push_back(temp_furniture_layout);
				temp_furniture_layout.clear();
			}
		}

		temp_furniture_layout_result_vector.clear();
		region_resolver->model_container.push_back(attach_furntiure);
	}
}

void MetisDynamicLayout::SortProject(shared_ptr<MetisRegionResolver>& region_resolver)
{
	/** 对于所有方案，按照分数降序排序 */
	for (int i = 1; i < region_resolver->all_furniture_weight_combo_data.size(); ++i)
	{
		for (int j = i; j > 0; --j)
		{
			float first_total_score = 0.0f;
			float second_total_score = 0.0f;

			for (auto&furniture : region_resolver->model_container)
			{
				first_total_score += region_resolver->all_furniture_weight_combo_data[j][furniture.id].vaild_score;
				second_total_score += region_resolver->all_furniture_weight_combo_data[j - 1][furniture.id].vaild_score;
			}

			/** 进行交换 */
			if (first_total_score > second_total_score)
			{
				map<int, SingleFurnitureSingleRuleLayoutData> temp_project;
				temp_project = region_resolver->all_furniture_weight_combo_data[j];
				region_resolver->all_furniture_weight_combo_data[j] = region_resolver->all_furniture_weight_combo_data[j - 1];
				region_resolver->all_furniture_weight_combo_data[j - 1] = temp_project;
			}
		}
	}
}

void MetisDynamicLayout::ReviseFurnitureInformaiton(shared_ptr<MetisRegionResolver>& region_resolver, SingleCase* single_case)
{
	/** 处理床头柜方向 */
	ManageBedside(region_resolver);

	/** 方案按照分数从高到低排序 */
	SortProject(region_resolver);

	/** 数据传给singcase */
	TransferToSingcase(region_resolver, single_case);
}

// 针对定制类型的家具进行延展性操作
void MetisDynamicLayout::ScalableFurniture(shared_ptr<MetisRegionResolver>& region_resolver)
{
	// 存在定制类家具-布局数据整合
	for (auto&layout_results : region_resolver->furniture_layout_result_vector)
	{
		for (auto&layout_result : layout_results)
		{
			region_resolver->all_furniture_weight_combo_data.push_back(layout_result);
		}
	}

	/** 判定是否有方案 */
	if (region_resolver->all_furniture_weight_combo_data.empty())
	{
		return;
	}

	// 判断是否存在定制类型的家具,一个空间只处理一个定制家具
	bool scale_flag = false;
	int model_id = 0;
	for (vector<Model>::iterator model = region_resolver->p_region->model_list.begin(); model != region_resolver->p_region->model_list.end(); model++)
	{
		// 测试使用10010
		if (model->is_maker)
		{
			model_id = model->id;
			scale_flag = true;
			break;
		}
	}
	// 不存在定制类家具
	if (!scale_flag)
	{
		return;
	}

	// 遍历处理每套布局方案
	for (auto&design_layout : region_resolver->all_furniture_weight_combo_data)
	{
		SingleFurnitureSingleRuleLayoutData model_layout;
		model_layout.model_id = model_id;

		// 处理边角问题
		region_resolver->ModifyCustomModel(model_layout, design_layout);

		//  获得延展后的模型
		if (!region_resolver->GetScaleModel(model_layout, design_layout))
		{
			continue;
		}

		//  检测与家具的碰撞并布局
		if (!region_resolver->CheckModelWithLayout(model_layout, design_layout))
		{
			continue;
		}
	}
}

/** 后期优化家具布局 */
void MetisDynamicLayout::OptimizedFurnitureLayout(shared_ptr<MetisRegionResolver> & region_resolver)
{
	// 遍历处理每套布局方案
	for (auto&design_layout : region_resolver->all_furniture_weight_combo_data)
	{

		// 吸附性处理
		region_resolver->CheckFurnitureAdsorption(design_layout);
	}
}

void MetisDynamicLayout::ManageBedside(shared_ptr<MetisRegionResolver> & region_resolver)
{
	if (region_resolver->p_region->single_room.getSpaceId() == ZHUWO || region_resolver->p_region->single_room.getSpaceId() == CIWO|| region_resolver->p_region->single_room.getSpaceId() == KEWO || region_resolver->p_region->single_room.getSpaceId() == ERTONGFANG)
	{

		// 遍历处理每套布局方案
		for (auto& design_layouts : region_resolver->all_furniture_weight_combo_data)
		{
			/** 床 */
			map<int, SingleFurnitureSingleRuleLayoutData > ::iterator iter_bed = design_layouts.find(GROUP_BED);

			if (iter_bed != design_layouts.end())
			{
				map<int, SingleFurnitureSingleRuleLayoutData > ::iterator iter_bedside_r = design_layouts.find(GROUP_RIGHT_BEDSIDE);
				map<int, SingleFurnitureSingleRuleLayoutData > ::iterator iter_bedside_l = design_layouts.find(GROUP_LEFT_BEDSIDE);
				if (iter_bedside_r != design_layouts.end())
				{
					/** 方向不一致 删除 */
					if (design_layouts[GROUP_BED].rotation_degree != design_layouts[GROUP_RIGHT_BEDSIDE].rotation_degree)
					{
						design_layouts[GROUP_RIGHT_BEDSIDE].vaild_score = 0.0f;
					}
				}
				if (iter_bedside_l != design_layouts.end())
				{
					/** 方向不一致 删除 */
					if (design_layouts[GROUP_BED].rotation_degree != design_layouts[GROUP_LEFT_BEDSIDE].rotation_degree)
					{
						design_layouts[GROUP_LEFT_BEDSIDE].vaild_score = 0.0f;
					}
				}
			}
		}
	}
}

void MetisDynamicLayout::TransferToSingcase(shared_ptr<MetisRegionResolver>& region_resolver, SingleCase* single_case)
{
	/** 判定是否有方案 */
	if (region_resolver->all_furniture_weight_combo_data.empty())
	{
		return;
	}

	/** 传递给 single_case */
	for (int i = 0; i < region_resolver->all_furniture_weight_combo_data.size(); ++i)
	{
		map<int, SingleFurnitureSingleRuleLayoutData> furniture_weight_combo_data = region_resolver->all_furniture_weight_combo_data[i];
		ModelLayout temp_modellayout;

		float totatl_score = 0.0f;
		vector<Model> temp_model;
		temp_model = region_resolver->model_container;

		/** 去除10分判定 */
		/** 将rotation转换为Point3f */
		for (auto& furniture : temp_model)
		{
			if (!ComMath::IsSameScalar(furniture_weight_combo_data[furniture.id].vaild_score, 10.f, 0.1f) && !ComMath::IsSameScalar(furniture_weight_combo_data[furniture.id].vaild_score, 0.0f, 0.1f))//去掉重叠的家具
			{
				/** 中心点 */
				furniture.location = furniture_weight_combo_data[furniture.id].translation_point;
				furniture.layout_rule = furniture_weight_combo_data[furniture.id].rule_no;

				/** 旋转角度 */
				Point3f p_rotation;
				switch (furniture_weight_combo_data[furniture.id].rotation_degree)
				{
				case 0:
					p_rotation = Point3f(0, 0, 0);
					break;
				case 90:
					p_rotation = Point3f(0, 90, 0);
					break;
				case 180:
					p_rotation = Point3f(0, 180, 0);
					break;
				case 270:
					p_rotation = Point3f(0, 270, 0);
					break;
				}

				furniture.rotation = p_rotation;
				/** 方案总分数 */
				totatl_score += furniture_weight_combo_data[furniture.id].vaild_score;
				if (furniture.is_maker)
				{
					if (furniture.width < furniture_weight_combo_data[furniture.id].width)
					{
						furniture.width = (float)furniture_weight_combo_data[furniture.id].width;
					}
				}
				temp_modellayout.model_list.push_back(furniture);
			}
		}

		/** 添加到layout_list */
		temp_modellayout.score = totatl_score;
		single_case->layout_list.push_back(temp_modellayout);
	}
}

