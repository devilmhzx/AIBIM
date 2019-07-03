#include "MetisRegionResolver.h"
#include <math.h>
MetisRegionResolver::MetisRegionResolver(SingleCase* singlecase)
	:p_region(singlecase), generate_solution_num(2)
{
	InitGenerateRegion(singlecase);
}

void MetisRegionResolver::InitGenerateRegion(SingleCase* singlecase)
{

	/** 分类家具 */
	SortFurniture(singlecase->model_list);

	/** 整理区域盒子 */
	ArrangeRegionBox(singlecase->corner_list);

	/** 创建节点 */
	CreateArray2D(room_box);

	/** 添加bInside的值 */
	AddBinside();

	/** 添加bIndoor */
	AddBindoor();

	/** 添加地标 */
	AddBinfloorbox();
}

void MetisRegionResolver::SortFurniture(vector<Model> & model_list)
{
	/** 导出模型 */
	for (auto &model : model_list)
	{
		if (model.layout_type == 0)
		{
			independent_model_container.push_back(model);
		}

		else if (model.layout_type == 1)
		{
			attach_model_container.push_back(model);
		}
	}

	model_container = independent_model_container;
}

void MetisRegionResolver::ArrangeRegionBox(vector<Corner> &corner_list)
{
	/** 导出房间点位 */
	for (auto &corner : corner_list)
	{
		room_corner.push_back(corner.point);
	}

	/** 区域最大保围盒子 */
	room_box = ComMath::GetVertsBoundingBox(room_corner);

	// 计算两位数组的行列
	if (ComMath::comTwoFloat(static_cast<float>((int(room_box.GetSize().x) % BoxSize)), 1.0f))
	{
		max_row = static_cast<int>(int(room_box.GetSize().x) / BoxSize);
	}
	else
	{
		max_row = static_cast<int>(int(room_box.GetSize().x) / BoxSize + 1);
	}

	if (ComMath::comTwoFloat(static_cast<float>((int(room_box.GetSize().y) % BoxSize)), 1.0f))
	{
		max_rank = static_cast<int>(room_box.GetSize().y / BoxSize);
	}
	else
	{
		max_rank = static_cast<int>(room_box.GetSize().y / BoxSize + 1);
	}
}

void MetisRegionResolver::CreateArray2D(Box2D &room_box)
{
	/** 初始化 */
	Point2f initial_point = Point2f(room_box.max_point.x, room_box.min_point.y);

	shared_ptr<MetisNode> temp_node/*=make_shared<MetisNode>(Box2D(Point2f(0,0),Point2f(1,1)))*/;
	vector<shared_ptr<MetisNode>> temp_base_node_row;
	temp_base_node_row.assign(max_rank, temp_node);
	base_nodes.assign(max_row, temp_base_node_row);

	/** 创建格子，构建数组 */
	for (int row_index = 0; row_index < max_row; ++row_index)
	{
		Point2f temp_initial_point = Point2f(initial_point.x - row_index * 10.f, initial_point.y);
		for (int rank_index = 0; rank_index < max_rank; ++rank_index)
		{
			Box2D base_node_box(Point2f(temp_initial_point.x - BoxSize, temp_initial_point.y), Point2f(temp_initial_point.x, temp_initial_point.y + BoxSize));
			shared_ptr<MetisNode> base_node = make_shared<MetisNode>(base_node_box);
			base_nodes[row_index][rank_index] = base_node;
			temp_initial_point.y += 10.f;
		}
	}
}

void MetisRegionResolver::AddBinside()
{
	for (int i = 0; i < max_row; ++i)
	{
		for (int j = 0; j < max_rank; ++j)
		{
			Point3f tmp_point = Point3f(base_nodes[i][j]->center_point.x, base_nodes[i][j]->center_point.y, 0);

			/**  若中心点在房间区域（房间点围成）内 */
			if (ComMath::PointInPolygon(tmp_point, room_corner))
			{
				/** 标记为在房间内 */
				base_nodes[i][j]->bInside = true;
			}
			else
			{
				base_nodes[i][j]->bInside = false;
			}
		}
	}
}

void MetisRegionResolver::AddBindoor()
{
	/** 标准门、防盗门、推拉门点位 */
	vector<vector<Point3f>> door_corner;

	/** 提取墙内的门 */
	for (auto& wall : p_region->wall_list)
	{
		/** 墙内有门 */
		if (!wall.door_list.empty())
		{
			for (auto& door : wall.door_list)
			{
				/** 标准门和防盗门 */
				if (door.door_type == D_NORMAL || door.door_type == D_SECURITY)
				{
					vector<Point3f> temp_door_corner;

					/** 构建门所占80*80的区域 */
					CreateNormalDoorRegion(door, temp_door_corner);
					door_corner.push_back(temp_door_corner);
					temp_door_corner.clear();
				}

				/** 推拉门 */
				if (door.door_type == D_SLIDING)
				{
					vector<Point3f> temp_door_corner;

					/** 构建门所占20*length的区域 */
					CreateSlidingDoorRegion(door, temp_door_corner);
					door_corner.push_back(temp_door_corner);
					temp_door_corner.clear();
				}
			}
		}
	}
	/** 遍历添加Bindoor */
	for (auto& door_corner_region : door_corner)
	{
		for (int i = 0; i < max_row; ++i)
		{
			for (int j = 0; j < max_rank; ++j)
			{
				Point3f temp_point = Point3f(base_nodes[i][j]->center_point.x, base_nodes[i][j]->center_point.y, 0);
				/** 是否中心点在门区域内 */
				if (ComMath::PointInPolygon(temp_point, door_corner_region))
				{
					base_nodes[i][j]->bIndoor = true;
				}
				else
				{
					base_nodes[i][j]->bIndoor = false;
				}
			}
		}
	}
}

void MetisRegionResolver::AddBinfloorbox()
{
	for (int i = 0; i < max_row; ++i)
	{
		for (int j = 0; j < max_rank; ++j)
		{
			Point3f temp_point = Point3f(base_nodes[i][j]->center_point.x, base_nodes[i][j]->center_point.y, 0);

			/**  若中心点在地标区域（地标点围成）内 */
			for (auto& pillar : p_region->floor_box_list)
			{
				if (ComMath::PointInPolygon(temp_point, pillar.point_list))
				{
					/** 标记为地标区域内 */
					base_nodes[i][j]->bInfloorbox = true;
				}
				else if(base_nodes[i][j]->bInfloorbox == false)
				{
					base_nodes[i][j]->bInfloorbox = false;
				}
			}
		}
	}
}

void MetisRegionResolver::CreateNormalDoorRegion(Door& door, vector<Point3f>& temp_door_corner)
{
	/*****************************
	*   构建门区域为80*length    *
	*****************************/

	/** 判定方向在x或y轴 */
	bool direction_x = (door.start_point.x == door.end_point.x) ? 1 : 0;
	if (direction_x)
	{
		/** x正方向 */
		Point3f temp_test_point = Point3f(door.pos.x + 30, door.pos.y, 0);
		if (ComMath::PointInPolygon(temp_test_point, room_corner))
		{
			temp_door_corner.push_back(door.start_point);
			temp_door_corner.push_back(door.end_point);
			temp_door_corner.push_back(Point3f(door.end_point.x + 80, door.end_point.y, 0));
			temp_door_corner.push_back(Point3f(door.start_point.x + 80, door.start_point.y, 0));
		}
		/** x负方向 */
		else
		{
			temp_door_corner.push_back(door.start_point);
			temp_door_corner.push_back(door.end_point);
			temp_door_corner.push_back(Point3f(door.end_point.x - 80, door.end_point.y, 0));
			temp_door_corner.push_back(Point3f(door.start_point.x - 80, door.start_point.y, 0));
		}
	}
	/** y方向 */
	else
	{
		/** y正方向 */
		Point3f temp_test_point = Point3f(door.pos.x, door.pos.y + 20, 0);
		if (ComMath::PointInPolygon(temp_test_point, room_corner))
		{
			temp_door_corner.push_back(door.start_point);
			temp_door_corner.push_back(door.end_point);
			temp_door_corner.push_back(Point3f(door.end_point.x, door.end_point.y + 80, 0));
			temp_door_corner.push_back(Point3f(door.start_point.x, door.start_point.y + 80, 0));
		}
		/** y负方向 */
		else
		{
			temp_door_corner.push_back(door.start_point);
			temp_door_corner.push_back(door.end_point);
			temp_door_corner.push_back(Point3f(door.end_point.x, door.end_point.y - 80, 0));
			temp_door_corner.push_back(Point3f(door.start_point.x, door.start_point.y - 80, 0));
		}
	}
}

void MetisRegionResolver::CreateSlidingDoorRegion(Door &door, vector<Point3f>& temp_door_corner)
{
	/****************************
	*   构建门区域为20*length   *
	****************************/

	/** 判定方向在x或y轴 */
	bool direction_x = (door.start_point.x == door.end_point.x) ? 1 : 0;
	if (direction_x)
	{
		/** x正方向 */
		Point3f temp_test_point = Point3f(door.pos.x + 30, door.pos.y, 0);
		if (ComMath::PointInPolygon(temp_test_point, room_corner))
		{
			temp_door_corner.push_back(door.start_point);
			temp_door_corner.push_back(door.end_point);
			temp_door_corner.push_back(Point3f(door.end_point.x + 20, door.end_point.y, 0));
			temp_door_corner.push_back(Point3f(door.start_point.x + 20, door.start_point.y, 0));
		}
		/** x负方向 */
		else
		{
			temp_door_corner.push_back(door.start_point);
			temp_door_corner.push_back(door.end_point);
			temp_door_corner.push_back(Point3f(door.end_point.x - 20, door.end_point.y, 0));
			temp_door_corner.push_back(Point3f(door.start_point.x - 20, door.start_point.y, 0));
		}
	}
	/** y方向 */
	else
	{
		/** y正方向 */
		Point3f temp_test_point = Point3f(door.pos.x, door.pos.y + 20, 0);
		if (ComMath::PointInPolygon(temp_test_point, room_corner))
		{
			temp_door_corner.push_back(door.start_point);
			temp_door_corner.push_back(door.end_point);
			temp_door_corner.push_back(Point3f(door.end_point.x, door.end_point.y + 20, 0));
			temp_door_corner.push_back(Point3f(door.start_point.x, door.start_point.y + 20, 0));
		}
		/** y负方向 */
		else
		{
			temp_door_corner.push_back(door.start_point);
			temp_door_corner.push_back(door.end_point);
			temp_door_corner.push_back(Point3f(door.end_point.x, door.end_point.y - 20, 0));
			temp_door_corner.push_back(Point3f(door.start_point.x, door.start_point.y - 20, 0));
		}
	}
}

void MetisRegionResolver::ClearLeafWeight()
{
	for (int i = 0; i < max_row; ++i)
	{
		for (int j = 0; j < max_rank; ++j)
		{
			if (base_nodes[i][j]->bInside)
				base_nodes[i][j]->Weight = 0.f;
		}
	}
}

void MetisRegionResolver::ClearLeafWeightToValue(float Value)
{
	for (int i = 0; i < max_row; ++i)
	{
		for (int j = 0; j < max_rank; ++j)
		{
			if (base_nodes[i][j]->bInside)
				base_nodes[i][j]->Weight = Value;
		}
	}
}

void MetisRegionResolver::AddIndependentLayoutDataArray(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_weight_data)
{
	/** 进行初始化 generate_solution_num为初始化vector个数 */
	SingleFurnitureAllRuleLayoutData data;

	/** 分数 */
	data.vaild_scores.assign(generate_solution_num, -999999.0f);

	/** 转换点，即家具的旋转点位 */
	data.translation_points.assign(generate_solution_num, Point3f(0, 0, 0));

	/** 旋转角度 */
	data.rotation_degrees.assign(generate_solution_num, 0);

	/** 家具的盒子的四个点 */
	vector<Point3f> furnituer_points(4, Point3f(0, 0, 0));
	data.furniture_vertex_pointss.assign(generate_solution_num, furnituer_points);

	/** 家具ID */
	data.model_ids.assign(generate_solution_num, 0);

	/** 家具的width */
	data.widths.assign(generate_solution_num, 0);

	/** 规则ID */
	data.rule_nos.assign(generate_solution_num, "value");

	single_furniture_weight_data.push_back(data);
}

void MetisRegionResolver::AddAttachLayoutDataArray(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_weight_data)
{
	/** 进行初始化 generate_solution_num为初始化vector个数 */
	SingleFurnitureAllRuleLayoutData data;

	/** 分数 */
	data.vaild_scores.assign(generate_solution_num, -999999.0f);

	/** 转换点，即家具的旋转点位 */
	data.translation_points.assign(generate_solution_num, Point3f(0, 0, 0));

	/** 旋转角度 */
	data.rotation_degrees.assign(generate_solution_num, 0);

	/** 家具的盒子的四个点 */
	vector<Point3f> furnituer_points(4, Point3f(0, 0, 0));
	data.furniture_vertex_pointss.assign(generate_solution_num, furnituer_points);

	/** 家具ID */
	data.model_ids.assign(generate_solution_num, 0);

	/** 家具的width */
	data.widths.assign(generate_solution_num, 0);

	/** 规则ID */
	data.rule_nos.assign(generate_solution_num, "value");

	single_furniture_weight_data.push_back(data);
}

Model MetisRegionResolver::ProcessingLivingCombinedModel(vector<Model>& in_model)
{
	/** 构造假模型组合 */
	Model temp_model;
	for (auto& inmodel:in_model)
	{
		if (inmodel.id==10028)
		{
			temp_model = inmodel;
			temp_model.length=180;
			temp_model.width=340;
			return temp_model;
		}
	}
	return temp_model;
}

// 获得延展后的模型的边角问题
bool MetisRegionResolver::ModifyCustomModel(SingleFurnitureSingleRuleLayoutData &model_layout, map<int, SingleFurnitureSingleRuleLayoutData> &design_layout)
{
	// 特殊处理定制家具相关问题
	// 如果定制家具靠近多个墙体，则删除靠近的较短墙体的柜体
	// 判断布局中是否存在延展的模型
	if (design_layout.find(model_layout.model_id) == design_layout.end())
	{
		return false;
	}

	SingleFurnitureSingleRuleLayoutData current_layout = design_layout[model_layout.model_id];
	if (current_layout.furniture_vertex_points.size() != 4)
	{
		return false;
	}

	// 获得模型信息
	Model *p_model = GetModelInfoById(model_layout.model_id);
	if (p_model == NULL)
	{
		return false;
	}

	vector<AiLine> lines;
	// 必定是4个节点，所以这里直接使用4
	for (int i = 0; i < 4; i++)
	{
		int j = (i + 1) % 4;
		float distance = ComMath::getTwoPointDistance(current_layout.furniture_vertex_points[i], current_layout.furniture_vertex_points[j]);
		if (distance > (p_model->width - Maker_Limit) && distance < (p_model->width + Maker_Limit))
		{
			AiLine tmp_line;
			tmp_line.start_point = current_layout.furniture_vertex_points[i];
			tmp_line.end_point = current_layout.furniture_vertex_points[j];
			lines.push_back(tmp_line);
		}
	}

	if (lines.size() != 2)
	{
		return false;
	}

	// 遍历当前户型的墙体
	Wall *p_wall = new Wall();
	int line_index = -1;
	Point3f pedal0;
	Point3f pedal1;

	for (auto &wall : p_region->wall_list)
	{
		Point3f pA = wall.start_corner.point;
		Point3f pB = wall.end_corner.point;
		float dis1 = ComMath::getPointToSegDis(lines[0].start_point, pA, pB);
		if (dis1 < 10)
		{
			// 找到平行的直线
			if (ComMath::isParallelTwoLine(lines[0].start_point, lines[0].end_point, pA, pB))
			{
				p_wall = &wall;
				line_index = 0;
				pedal0 = ComMath::getPointToLinePedal(lines[0].start_point, pA, pB);
				pedal1 = ComMath::getPointToLinePedal(lines[0].end_point, pA, pB);
				break;
			}
		}

		float dis2 = ComMath::getPointToSegDis(lines[1].start_point, pA, pB);
		if (dis2 < 10)
		{
			// 找到平行的直线
			if (ComMath::isParallelTwoLine(lines[1].start_point, lines[1].end_point, pA, pB))
			{
				p_wall = &wall;
				line_index = 1;
				pedal0 = ComMath::getPointToLinePedal(lines[1].start_point, pA, pB);
				pedal1 = ComMath::getPointToLinePedal(lines[1].end_point, pA, pB);
				break;
			}
		}
	}

	// 需要延展的家具不靠近墙体
	if (line_index == -1)
	{
		return false;
	}

	// 家具的有效点
	Point3f pA = p_wall->start_corner.point;
	Point3f pB = p_wall->end_corner.point;

	// 判断是否能够布局家具
	if (ComMath::getTwoPointDistance(pA, pB) < ComMath::getTwoPointDistance(pedal0, pedal1))
	{
		return false;
	}

	//  pA ----pedal0-----B------pedal1
	if (ComMath::isPointBetweenPoint(pedal0, pA, pB) && ComMath::isPointBetweenPoint(pedal1, pA, pB))
	{
		return false;
	}

	//  pA ----pedal0-----B------pedal1
	if (ComMath::isPointBetweenPoint(pedal0, pA, pB) && !ComMath::isPointBetweenPoint(pedal1, pA, pB))
	{
		float pedal1_pA = ComMath::getTwoPointDistance(pedal1, pA);
		float pedal1_pB = ComMath::getTwoPointDistance(pedal1, pB);
		if (pedal1_pA < p_model->width / 10.0f && pedal1_pA < pedal1_pB)
		{
			pedal1 = pA;
		}

		if (pedal1_pB < p_model->width / 10.0f && pedal1_pA > pedal1_pB)
		{
			pedal1 = pB;
		}
	}

	//  pA ----pedal1-----B------pedal0
	if (!ComMath::isPointBetweenPoint(pedal0, pA, pB) && ComMath::isPointBetweenPoint(pedal1, pA, pB))
	{
		float pedal0_pA = ComMath::getTwoPointDistance(pedal0, pA);
		float pedal0_pB = ComMath::getTwoPointDistance(pedal0, pB);
		if (pedal0_pA < p_model->width / 10.0f && pedal0_pA < pedal0_pB)
		{
			pedal0 = pA;
		}

		if (pedal0_pB < p_model->width / 10.0f && pedal0_pA > pedal0_pB)
		{
			pedal0 = pB;
		}
	}

	Point3f p0 = ComMath::getPointToLinePedal(pedal0, lines[0].start_point, lines[0].end_point);
	Point3f p1 = ComMath::getPointToLinePedal(pedal1, lines[0].start_point, lines[0].end_point);
	Point3f p2 = ComMath::getPointToLinePedal(pedal1, lines[1].start_point, lines[1].end_point);
	Point3f p3 = ComMath::getPointToLinePedal(pedal0, lines[1].start_point, lines[1].end_point);

	// 延展后的数据存储
	model_layout = current_layout;
	model_layout.furniture_vertex_points.clear();
	model_layout.furniture_vertex_points.push_back(p0);
	model_layout.furniture_vertex_points.push_back(p1);
	model_layout.furniture_vertex_points.push_back(p2);
	model_layout.furniture_vertex_points.push_back(p3);
	model_layout.translation_point.x = (p0.x + p2.x) / 2;
	model_layout.translation_point.y = (p0.y + p2.y) / 2;
	model_layout.width = ComMath::getTwoPointDistance(p0, p1);

	design_layout[model_layout.model_id] = model_layout;

	// 更新模型长度信息
	p_model->width = model_layout.width;
	return true;

}
//////////////////////////////////延展相关方法/////////////////////////////
// 获得延展后的模型
bool MetisRegionResolver::GetScaleModel(SingleFurnitureSingleRuleLayoutData &model_layout, map<int, SingleFurnitureSingleRuleLayoutData> design_layout)
{
	// 判断布局中是否存在延展的模型
	if (design_layout.find(model_layout.model_id) == design_layout.end())
	{
		return false;
	}
	SingleFurnitureSingleRuleLayoutData current_layout = design_layout[model_layout.model_id];
	if (current_layout.furniture_vertex_points.size() != 4)
	{
		return false;
	}

	// 获得模型信息
	Model *p_model = GetModelInfoById(model_layout.model_id);
	if (p_model == NULL)
	{
		return false;
	}

	vector<AiLine> lines;
	// 必定是4个节点，所以这里直接使用4
	for (int i = 0; i < 4; i++)
	{
		int j = (i + 1) % 4;
		float distance = ComMath::getTwoPointDistance(current_layout.furniture_vertex_points[i], current_layout.furniture_vertex_points[j]);
		if (distance > (p_model->width - Maker_Limit) && distance < (p_model->width + Maker_Limit))
		{
			AiLine tmp_line;
			tmp_line.start_point = current_layout.furniture_vertex_points[i];
			tmp_line.end_point = current_layout.furniture_vertex_points[j];
			lines.push_back(tmp_line);
		}
	}

	if (lines.size() != 2)
	{
		return false;
	}

	// 遍历当前户型的墙体
	Wall *p_wall = new Wall();
	int line_index = -1;
	Point3f pedal0;
	Point3f pedal1;

	for (auto &wall : p_region->wall_list)
	{
		Point3f pA = wall.start_corner.point;
		Point3f pB = wall.end_corner.point;
		float dis1 = ComMath::getPointToSegDis(lines[0].start_point, pA, pB);
		if (dis1 < 10)
		{
			// 找到平行的直线
			if (ComMath::isParallelTwoLine2(lines[0].start_point, lines[0].end_point, pA, pB))
			{
				p_wall = &wall;
				line_index = 0;
				pedal0 = ComMath::getPointToLinePedal(lines[0].start_point, pA, pB);
				pedal1 = ComMath::getPointToLinePedal(lines[0].end_point, pA, pB);
				break;
			}
		}

		float dis2 = ComMath::getPointToSegDis(lines[1].start_point, pA, pB);
		if (dis2 < 10)
		{
			// 找到平行的直线
			if (ComMath::isParallelTwoLine2(lines[1].start_point, lines[1].end_point, pA, pB))
			{
				p_wall = &wall;
				line_index = 1;
				pedal0 = ComMath::getPointToLinePedal(lines[1].start_point, pA, pB);
				pedal1 = ComMath::getPointToLinePedal(lines[1].end_point, pA, pB);
				break;
			}
		}
	}

	// 需要延展的家具不靠近墙体
	if (line_index == -1)
	{
		return false;
	}

	// 家具的有效点
	Point3f pA = p_wall->start_corner.point;
	Point3f pB = p_wall->end_corner.point;

	// 找到此最大墙体上面的有效墙体-即挖去门或窗户
	//A--------------d0------------d1-------------B 原始墙体
	// A--------------d0------------d1-----pedal0---pedal1--------B
	for (auto &door : p_wall->door_list)
	{
		Point3f d0 = door.start_point;
		Point3f d1 = door.end_point;
		if (ComMath::getTwoPointDistance(pA, door.start_point) > ComMath::getTwoPointDistance(pA, door.end_point))
		{
			d0 = door.end_point;
			d1 = door.start_point;
		}

		//      A--- pedal0-------pedal1---------d0--------d1-----------B
		if (ComMath::isPointBetweenPoint(pedal0, pA, d0) && ComMath::isPointBetweenPoint(pedal1, pA, d0))
		{
			pB = d0;
			continue;
		}
		//      A--- d0-------d1---------pedal0--------pedal1-----------B
		if (ComMath::isPointBetweenPoint(pedal0, pB, d1) && ComMath::isPointBetweenPoint(pedal1, pB, d1))
		{
			pA = d1;
			continue;
		}

		return false;
	}

	// 窗户
	for (auto &window : p_wall->window_list)
	{
		Point3f d0 = window.start_point;
		Point3f d1 = window.end_point;
		if (ComMath::getTwoPointDistance(pA, window.start_point) > ComMath::getTwoPointDistance(pA, window.end_point))
		{
			d0 = window.end_point;
			d1 = window.start_point;
		}

		//      A--- pedal0-------pedal1---------d0--------d1-----------B
		if (ComMath::isPointBetweenPoint(pedal0, pA, d0) && ComMath::isPointBetweenPoint(pedal1, pA, d0))
		{
			pB = d0;
			continue;
		}
		//      A--- d0-------d1---------pedal0--------pedal1-----------B
		if (ComMath::isPointBetweenPoint(pedal0, pB, d1) && ComMath::isPointBetweenPoint(pedal1, pB, d1))
		{
			pA = d1;
			continue;
		}

		return false;
	}

	// 根据家具靠近的最大墙体延展家具
	// A----pedal0-----pedal1------B
	// ----p0---------p1--------------line0
	// ----p2---------p3--------------line1
	// 计算AB到两直线的垂点
	Point3f p0 = ComMath::getPointToLinePedal(pA, lines[0].start_point, lines[0].end_point);
	Point3f p1 = ComMath::getPointToLinePedal(pB, lines[0].start_point, lines[0].end_point);
	Point3f p2 = ComMath::getPointToLinePedal(pB, lines[1].start_point, lines[1].end_point);
	Point3f p3 = ComMath::getPointToLinePedal(pA, lines[1].start_point, lines[1].end_point);
	Point3f new_pA = pA;
	Point3f new_pB = pB;
	// 处理左右墙体数据
	if (CheckModelWithHouse(p0, p1, p2, p3, new_pA, new_pB))
	{
		if (ComMath::isPointBetweenPoint(pedal0, new_pA, new_pB) && ComMath::isPointBetweenPoint(pedal1, new_pA, new_pB))
		{
			p0 = ComMath::getPointToLinePedal(new_pA, lines[0].start_point, lines[0].end_point);
			p1 = ComMath::getPointToLinePedal(new_pB, lines[0].start_point, lines[0].end_point);
			p2 = ComMath::getPointToLinePedal(new_pB, lines[1].start_point, lines[1].end_point);
			p3 = ComMath::getPointToLinePedal(new_pA, lines[1].start_point, lines[1].end_point);
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	// 延展后的数据存储
	model_layout = current_layout;
	model_layout.furniture_vertex_points.clear();
	model_layout.furniture_vertex_points.push_back(p0);
	model_layout.furniture_vertex_points.push_back(p1);
	model_layout.furniture_vertex_points.push_back(p2);
	model_layout.furniture_vertex_points.push_back(p3);
	model_layout.translation_point.x = (p0.x + p2.x) / 2;
	model_layout.translation_point.y = (p0.y + p2.y) / 2;
	model_layout.width = ComMath::getTwoPointDistance(p0, p1);

	return true;
}

// 检测模型与户型的碰撞
// 使用时必须结合获得延展模型的操作，否则报错
bool MetisRegionResolver::CheckModelWithHouse(Point3f p0, Point3f p1, Point3f p2, Point3f p3, Point3f &pA, Point3f &pB)
{
	// 遍历当前户型的墙体
	Wall *p_wall = new Wall();
	Point3f pedal0;
	Point3f pedal1;
	bool end_flag = true;

	// 处理左边墙体
	//p0--------------p1
	//-----------------
	//p3--------------p2
	for (auto &wall : p_region->wall_list)
	{
		Point3f pC = wall.start_corner.point;
		Point3f pD = wall.end_corner.point;
		// 忽略共线的情况
		if (ComMath::isPointBetweenPoint(pA, pC, pD) && ComMath::isPointBetweenPoint(pB, pC, pD))
		{
			continue;
		}

		float dis = ComMath::getPointToSegDis(p0, pC, pD);
		if (dis < 80)
		{
			Point3f f0 = ComMath::getPointToLinePedal(p0, pC, pD);
			Point3f f1 = ComMath::getPointToLinePedal(p3, pC, pD);
			if (ComMath::isPointBetweenPoint(f0, pC, pD) && ComMath::isPointBetweenPoint(f1, pC, pD))
			{
				p_wall = &wall;
				pedal0 = f0;
				pedal1 = f1;
				end_flag = false;
				break;
			}

			if (ComMath::isPointBetweenPoint(f0, pC, pD) || ComMath::isPointBetweenPoint(f1, pC, pD))
			{
				pA = Point3f(0, 0, 0);
				pB = Point3f(1000, 1000, 0);
				return false;
			}

		}
	}

	//  没有
	if (end_flag)
	{
		goto right_wall;
	}
	// 处理门相关的问题
	for (auto &door : p_wall->door_list)
	{
		Point3f d0 = door.start_point;
		Point3f d1 = door.end_point;

		// 家具与门冲突
		// pedal_pA---------A-------------------B/
		if (ComMath::isPointBetweenPoint(pedal0, d0, d1) || ComMath::isPointBetweenPoint(pedal1, d0, d1) || ComMath::isPointBetweenPoint(d0, pedal0, pedal1) || ComMath::isPointBetweenPoint(d1, pedal0, pedal1))
		{
			Point3f pedal_pA = ComMath::getPointToLinePedal(pA, d0, d1);
			Point3f pedal_pA_move = ComMath::getEffectPointOnLine(pedal_pA, pA, pB, 80);
			if (ComMath::comTwoPoint3f(pedal_pA_move, Point3f(0, 0, 0)))
			{
				continue;
			}
			else
			{
				// pedal_sA---------B-------------------A
				if (ComMath::getTwoPointDistance(pedal_pA, pA) > ComMath::getTwoPointDistance(pedal_pA, pB))
				{
					pB = pedal_pA_move;
				}
				else
				{
					pA = pedal_pA_move;
				}
			}
		}
		else
		{
			continue;
		}
	}

	// 处理窗户相关的问题
	for (auto &window : p_wall->window_list)
	{
		Point3f d0 = window.start_point;
		Point3f d1 = window.end_point;

		// 家具与门冲突
		// pedal_pA---------A-------------------B
		if (ComMath::isPointBetweenPoint(pedal0, d0, d1) || ComMath::isPointBetweenPoint(pedal1, d0, d1) || ComMath::isPointBetweenPoint(d0, pedal0, pedal1) || ComMath::isPointBetweenPoint(d1, pedal0, pedal1))
		{
			Point3f pedal_pA = ComMath::getPointToLinePedal(pA, d0, d1);
			Point3f pedal_pA_move = ComMath::getEffectPointOnLine(pedal_pA, pA, pB, 80);
			if (ComMath::comTwoPoint3f(pedal_pA_move, Point3f(0, 0, 0)))
			{
				continue;
			}
			else
			{
				// pedal_pA---------B-------------------A
				if (ComMath::getTwoPointDistance(pedal_pA, pA) > ComMath::getTwoPointDistance(pedal_pA, pB))
				{
					pB = pedal_pA_move;
				}
				else
				{
					pA = pedal_pA_move;
				}
			}
		}
		else
		{
			continue;
		}
	}


right_wall:
	// 处理右边墙体
	//p0--------------p1
	//-----------------
	//p3--------------p2
	for (auto &wall : p_region->wall_list)
	{
		Point3f pC = wall.start_corner.point;
		Point3f pD = wall.end_corner.point;
		if (ComMath::isPointBetweenPoint(pA, pC, pD) && ComMath::isPointBetweenPoint(pB, pC, pD))
		{
			continue;
		}
		float dis = ComMath::getPointToSegDis(p1, pC, pD);
		if (dis < 80)
		{
			Point3f f0 = ComMath::getPointToLinePedal(p1, pC, pD);
			Point3f f1 = ComMath::getPointToLinePedal(p2, pC, pD);
			if (ComMath::isPointBetweenPoint(f0, pC, pD) && ComMath::isPointBetweenPoint(f1, pC, pD))
			{
				p_wall = &wall;
				pedal0 = f0;
				pedal1 = f1;
				end_flag = false;
				break;
			}

			if (ComMath::isPointBetweenPoint(f0, pC, pD) || ComMath::isPointBetweenPoint(f1, pC, pD))
			{
				pA = Point3f(0, 0, 0);
				pB = Point3f(1000, 1000, 0);
				return false;
			}
		}
	}

	//  没有合适的墙体
	if (end_flag)
	{
		return true;
	}

	// 处理门相关的问题
	for (auto &door : p_wall->door_list)
	{
		Point3f d0 = door.start_point;
		Point3f d1 = door.end_point;

		// 家具与门冲突
		// pedal_pA---------A-------------------B/
		if (ComMath::isPointBetweenPoint(pedal0, d0, d1) || ComMath::isPointBetweenPoint(pedal1, d0, d1) || ComMath::isPointBetweenPoint(d0, pedal0, pedal1) || ComMath::isPointBetweenPoint(d1, pedal0, pedal1))
		{
			Point3f pedal_pA = ComMath::getPointToLinePedal(pA, d0, d1);
			Point3f pedal_pA_move = ComMath::getEffectPointOnLine(pedal_pA, pA, pB, 80);
			if (ComMath::comTwoPoint3f(pedal_pA_move, Point3f(0, 0, 0)))
			{
				continue;
			}
			else
			{
				// pedal_sA---------B-------------------A
				if (ComMath::getTwoPointDistance(pedal_pA, pA) > ComMath::getTwoPointDistance(pedal_pA, pB))
				{
					pB = pedal_pA_move;
				}
				else
				{
					pA = pedal_pA_move;
				}
			}
		}
		else
		{
			continue;
		}
	}

	// 处理窗户相关的问题
	for (auto &window : p_wall->window_list)
	{
		Point3f d0 = window.start_point;
		Point3f d1 = window.end_point;

		// 家具与门冲突
		// pedal_pA---------A-------------------B
		if (ComMath::isPointBetweenPoint(pedal0, d0, d1) || ComMath::isPointBetweenPoint(pedal1, d0, d1) || ComMath::isPointBetweenPoint(d0, pedal0, pedal1) || ComMath::isPointBetweenPoint(d1, pedal0, pedal1))
		{
			Point3f pedal_pA = ComMath::getPointToLinePedal(pA, d0, d1);
			Point3f pedal_pA_move = ComMath::getEffectPointOnLine(pedal_pA, pA, pB, 80);
			if (ComMath::comTwoPoint3f(pedal_pA_move, Point3f(0, 0, 0)))
			{
				continue;
			}
			else
			{
				// pedal_pA---------B-------------------A
				if (ComMath::getTwoPointDistance(pedal_pA, pA) > ComMath::getTwoPointDistance(pedal_pA, pB))
				{
					pB = pedal_pA_move;
				}
				else
				{
					pA = pedal_pA_move;
				}
			}
		}
		else
		{
			continue;
		}
	}

	return true;
}
//  检测与家具的碰撞
bool MetisRegionResolver::CheckModelWithLayout(SingleFurnitureSingleRuleLayoutData model_layout, map<int, SingleFurnitureSingleRuleLayoutData> &design_layout)
{
	/** 检测与布局内构件是否碰撞*/
	for (auto& layout : design_layout)
	{
		/** 不和自身检测 */
		if (layout.first != model_layout.model_id)
		{
			if (ComMath::IsCollisionRect(model_layout.furniture_vertex_points, layout.second.furniture_vertex_points))
			{
				return false;
			}
		}
	}

	design_layout[model_layout.model_id] = model_layout;

	return true;
}

//  家具吸附性处理
bool MetisRegionResolver::CheckFurnitureAdsorption(map<int, SingleFurnitureSingleRuleLayoutData> &design_layout)
{
	// 选择出可能需要吸附的墙体和家具对应的边
	for (auto& layout : design_layout)
	{
		AdsorptivityData result_data;
		if (FurnitureAdsorption(layout.second, result_data))
		{

			SingleFurnitureSingleRuleLayoutData &tmp_furniture = layout.second;
			Model *p_model = GetModelInfoById(tmp_furniture.model_id);
			float move_length = tmp_furniture.width;

			// 处理家具
			float dis = ComMath::getTwoPointDistance(tmp_furniture.furniture_vertex_points[result_data.start_furniture_index], tmp_furniture.furniture_vertex_points[result_data.end_furniture_index]);
			if (fabsf(move_length - dis) < 3)
			{
				move_length = p_model->length;
			}

			int next_index = (result_data.end_furniture_index + 1) % tmp_furniture.furniture_vertex_points.size();
			int last_index = (result_data.start_furniture_index - 1) % tmp_furniture.furniture_vertex_points.size();

			//------------p_wall----------------------
			//--start--------------------end-------
			//-last-----------------------next-----
			Point3f next_point = tmp_furniture.furniture_vertex_points[next_index];
			Point3f last_point = tmp_furniture.furniture_vertex_points[last_index];
			Point3f start_pedal = ComMath::getPointToLinePedal(tmp_furniture.furniture_vertex_points[result_data.start_furniture_index], result_data.p_wall->start_corner.point, result_data.p_wall->end_corner.point);
			Point3f end_pedal = ComMath::getPointToLinePedal(tmp_furniture.furniture_vertex_points[result_data.end_furniture_index], result_data.p_wall->start_corner.point, result_data.p_wall->end_corner.point);
			next_point = end_pedal + ComMath::Normalize(next_point - end_pedal)*move_length;
			last_point = start_pedal + ComMath::Normalize(last_point - start_pedal)*move_length;

			// 重新复制家具点
			tmp_furniture.furniture_vertex_points[next_index] = next_point;
			tmp_furniture.furniture_vertex_points[last_index] = last_point;
			tmp_furniture.furniture_vertex_points[result_data.start_furniture_index] = start_pedal;
			tmp_furniture.furniture_vertex_points[result_data.end_furniture_index] = end_pedal;
			tmp_furniture.translation_point.x = (start_pedal.x + next_point.x) / 2;
			tmp_furniture.translation_point.y = (start_pedal.y + next_point.y) / 2;
		}
		else
		{
			continue;
		}
	}
	return true;
}
// 根据家具编号获得家具信息
Model *MetisRegionResolver::GetModelInfoById(int model_id)
{
	for (auto &model : p_region->model_list)
	{
		if (model.id == model_id)
		{
			return &model;
		}
	}

	return NULL;
}

// 家具是否需要吸附性处理，如果需要返回吸附性的点
bool MetisRegionResolver::FurnitureAdsorption(SingleFurnitureSingleRuleLayoutData model_layout, AdsorptivityData &adsorption)
{
	// 遍历墙体
	for (auto &wall : p_region->wall_list)
	{
		for (int i = 0; i < model_layout.furniture_vertex_points.size(); i++)
		{
			int j = (i + 1) % model_layout.furniture_vertex_points.size();
			float angle = ComMath::GetAngleBetweenTwoLines(wall.start_corner.point - wall.end_corner.point, model_layout.furniture_vertex_points[i] - model_layout.furniture_vertex_points[j]);
			angle = angle / M_PI * 180;
			if (angle < 5 || angle > 175)
			{
				// 家具是否靠近墙体
				if (IsFurnitureOnWall(model_layout.furniture_vertex_points[i], model_layout.furniture_vertex_points[j], &wall))
				{
					adsorption.start_furniture_index = i;
					adsorption.end_furniture_index = j;
					adsorption.p_wall = &wall;
					return true;
				}
				else
				{
					continue;
				}
			}
			else
			{
				continue;
			}
		}
	}

	return false;
}
// 家具是否靠近该墙体
bool MetisRegionResolver::IsFurnitureOnWall(Point3f start_point, Point3f end_point, Wall *p_wall)
{
	float dis = ComMath::getPointToLineDis(start_point, p_wall->start_corner.point, p_wall->end_corner.point);
	if (dis <2 || dis > 9.1f)
	{
		return false;
	}

	Point3f start_pedal = ComMath::getPointToLinePedal(start_point, p_wall->start_corner.point, p_wall->end_corner.point);
	Point3f end_pedal = ComMath::getPointToLinePedal(end_point, p_wall->start_corner.point, p_wall->end_corner.point);
	if (ComMath::isPointBetweenPoint(start_pedal, p_wall->start_corner.point, p_wall->end_corner.point) && ComMath::isPointBetweenPoint(end_pedal, p_wall->start_corner.point, p_wall->end_corner.point))
	{
		return true;
	}
	else
	{
		return false;
	}
}
