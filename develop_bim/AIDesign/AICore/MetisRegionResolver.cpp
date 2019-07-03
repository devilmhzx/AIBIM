#include "MetisRegionResolver.h"
#include <math.h>
MetisRegionResolver::MetisRegionResolver(SingleCase* singlecase)
	:p_region(singlecase), generate_solution_num(2)
{
	InitGenerateRegion(singlecase);
}

void MetisRegionResolver::InitGenerateRegion(SingleCase* singlecase)
{

	/** ����Ҿ� */
	SortFurniture(singlecase->model_list);

	/** ����������� */
	ArrangeRegionBox(singlecase->corner_list);

	/** �����ڵ� */
	CreateArray2D(room_box);

	/** ���bInside��ֵ */
	AddBinside();

	/** ���bIndoor */
	AddBindoor();

	/** ��ӵر� */
	AddBinfloorbox();
}

void MetisRegionResolver::SortFurniture(vector<Model> & model_list)
{
	/** ����ģ�� */
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
	/** ���������λ */
	for (auto &corner : corner_list)
	{
		room_corner.push_back(corner.point);
	}

	/** �������Χ���� */
	room_box = ComMath::GetVertsBoundingBox(room_corner);

	// ������λ���������
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
	/** ��ʼ�� */
	Point2f initial_point = Point2f(room_box.max_point.x, room_box.min_point.y);

	shared_ptr<MetisNode> temp_node/*=make_shared<MetisNode>(Box2D(Point2f(0,0),Point2f(1,1)))*/;
	vector<shared_ptr<MetisNode>> temp_base_node_row;
	temp_base_node_row.assign(max_rank, temp_node);
	base_nodes.assign(max_row, temp_base_node_row);

	/** �������ӣ��������� */
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

			/**  �����ĵ��ڷ������򣨷����Χ�ɣ��� */
			if (ComMath::PointInPolygon(tmp_point, room_corner))
			{
				/** ���Ϊ�ڷ����� */
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
	/** ��׼�š������š������ŵ�λ */
	vector<vector<Point3f>> door_corner;

	/** ��ȡǽ�ڵ��� */
	for (auto& wall : p_region->wall_list)
	{
		/** ǽ������ */
		if (!wall.door_list.empty())
		{
			for (auto& door : wall.door_list)
			{
				/** ��׼�źͷ����� */
				if (door.door_type == D_NORMAL || door.door_type == D_SECURITY)
				{
					vector<Point3f> temp_door_corner;

					/** ��������ռ80*80������ */
					CreateNormalDoorRegion(door, temp_door_corner);
					door_corner.push_back(temp_door_corner);
					temp_door_corner.clear();
				}

				/** ������ */
				if (door.door_type == D_SLIDING)
				{
					vector<Point3f> temp_door_corner;

					/** ��������ռ20*length������ */
					CreateSlidingDoorRegion(door, temp_door_corner);
					door_corner.push_back(temp_door_corner);
					temp_door_corner.clear();
				}
			}
		}
	}
	/** �������Bindoor */
	for (auto& door_corner_region : door_corner)
	{
		for (int i = 0; i < max_row; ++i)
		{
			for (int j = 0; j < max_rank; ++j)
			{
				Point3f temp_point = Point3f(base_nodes[i][j]->center_point.x, base_nodes[i][j]->center_point.y, 0);
				/** �Ƿ����ĵ����������� */
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

			/**  �����ĵ��ڵر����򣨵ر��Χ�ɣ��� */
			for (auto& pillar : p_region->floor_box_list)
			{
				if (ComMath::PointInPolygon(temp_point, pillar.point_list))
				{
					/** ���Ϊ�ر������� */
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
	*   ����������Ϊ80*length    *
	*****************************/

	/** �ж�������x��y�� */
	bool direction_x = (door.start_point.x == door.end_point.x) ? 1 : 0;
	if (direction_x)
	{
		/** x������ */
		Point3f temp_test_point = Point3f(door.pos.x + 30, door.pos.y, 0);
		if (ComMath::PointInPolygon(temp_test_point, room_corner))
		{
			temp_door_corner.push_back(door.start_point);
			temp_door_corner.push_back(door.end_point);
			temp_door_corner.push_back(Point3f(door.end_point.x + 80, door.end_point.y, 0));
			temp_door_corner.push_back(Point3f(door.start_point.x + 80, door.start_point.y, 0));
		}
		/** x������ */
		else
		{
			temp_door_corner.push_back(door.start_point);
			temp_door_corner.push_back(door.end_point);
			temp_door_corner.push_back(Point3f(door.end_point.x - 80, door.end_point.y, 0));
			temp_door_corner.push_back(Point3f(door.start_point.x - 80, door.start_point.y, 0));
		}
	}
	/** y���� */
	else
	{
		/** y������ */
		Point3f temp_test_point = Point3f(door.pos.x, door.pos.y + 20, 0);
		if (ComMath::PointInPolygon(temp_test_point, room_corner))
		{
			temp_door_corner.push_back(door.start_point);
			temp_door_corner.push_back(door.end_point);
			temp_door_corner.push_back(Point3f(door.end_point.x, door.end_point.y + 80, 0));
			temp_door_corner.push_back(Point3f(door.start_point.x, door.start_point.y + 80, 0));
		}
		/** y������ */
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
	*   ����������Ϊ20*length   *
	****************************/

	/** �ж�������x��y�� */
	bool direction_x = (door.start_point.x == door.end_point.x) ? 1 : 0;
	if (direction_x)
	{
		/** x������ */
		Point3f temp_test_point = Point3f(door.pos.x + 30, door.pos.y, 0);
		if (ComMath::PointInPolygon(temp_test_point, room_corner))
		{
			temp_door_corner.push_back(door.start_point);
			temp_door_corner.push_back(door.end_point);
			temp_door_corner.push_back(Point3f(door.end_point.x + 20, door.end_point.y, 0));
			temp_door_corner.push_back(Point3f(door.start_point.x + 20, door.start_point.y, 0));
		}
		/** x������ */
		else
		{
			temp_door_corner.push_back(door.start_point);
			temp_door_corner.push_back(door.end_point);
			temp_door_corner.push_back(Point3f(door.end_point.x - 20, door.end_point.y, 0));
			temp_door_corner.push_back(Point3f(door.start_point.x - 20, door.start_point.y, 0));
		}
	}
	/** y���� */
	else
	{
		/** y������ */
		Point3f temp_test_point = Point3f(door.pos.x, door.pos.y + 20, 0);
		if (ComMath::PointInPolygon(temp_test_point, room_corner))
		{
			temp_door_corner.push_back(door.start_point);
			temp_door_corner.push_back(door.end_point);
			temp_door_corner.push_back(Point3f(door.end_point.x, door.end_point.y + 20, 0));
			temp_door_corner.push_back(Point3f(door.start_point.x, door.start_point.y + 20, 0));
		}
		/** y������ */
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
	/** ���г�ʼ�� generate_solution_numΪ��ʼ��vector���� */
	SingleFurnitureAllRuleLayoutData data;

	/** ���� */
	data.vaild_scores.assign(generate_solution_num, -999999.0f);

	/** ת���㣬���Ҿߵ���ת��λ */
	data.translation_points.assign(generate_solution_num, Point3f(0, 0, 0));

	/** ��ת�Ƕ� */
	data.rotation_degrees.assign(generate_solution_num, 0);

	/** �Ҿߵĺ��ӵ��ĸ��� */
	vector<Point3f> furnituer_points(4, Point3f(0, 0, 0));
	data.furniture_vertex_pointss.assign(generate_solution_num, furnituer_points);

	/** �Ҿ�ID */
	data.model_ids.assign(generate_solution_num, 0);

	/** �Ҿߵ�width */
	data.widths.assign(generate_solution_num, 0);

	/** ����ID */
	data.rule_nos.assign(generate_solution_num, "value");

	single_furniture_weight_data.push_back(data);
}

void MetisRegionResolver::AddAttachLayoutDataArray(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_weight_data)
{
	/** ���г�ʼ�� generate_solution_numΪ��ʼ��vector���� */
	SingleFurnitureAllRuleLayoutData data;

	/** ���� */
	data.vaild_scores.assign(generate_solution_num, -999999.0f);

	/** ת���㣬���Ҿߵ���ת��λ */
	data.translation_points.assign(generate_solution_num, Point3f(0, 0, 0));

	/** ��ת�Ƕ� */
	data.rotation_degrees.assign(generate_solution_num, 0);

	/** �Ҿߵĺ��ӵ��ĸ��� */
	vector<Point3f> furnituer_points(4, Point3f(0, 0, 0));
	data.furniture_vertex_pointss.assign(generate_solution_num, furnituer_points);

	/** �Ҿ�ID */
	data.model_ids.assign(generate_solution_num, 0);

	/** �Ҿߵ�width */
	data.widths.assign(generate_solution_num, 0);

	/** ����ID */
	data.rule_nos.assign(generate_solution_num, "value");

	single_furniture_weight_data.push_back(data);
}

Model MetisRegionResolver::ProcessingLivingCombinedModel(vector<Model>& in_model)
{
	/** �����ģ����� */
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

// �����չ���ģ�͵ı߽�����
bool MetisRegionResolver::ModifyCustomModel(SingleFurnitureSingleRuleLayoutData &model_layout, map<int, SingleFurnitureSingleRuleLayoutData> &design_layout)
{
	// ���⴦���ƼҾ��������
	// ������ƼҾ߿������ǽ�壬��ɾ�������Ľ϶�ǽ��Ĺ���
	// �жϲ������Ƿ������չ��ģ��
	if (design_layout.find(model_layout.model_id) == design_layout.end())
	{
		return false;
	}

	SingleFurnitureSingleRuleLayoutData current_layout = design_layout[model_layout.model_id];
	if (current_layout.furniture_vertex_points.size() != 4)
	{
		return false;
	}

	// ���ģ����Ϣ
	Model *p_model = GetModelInfoById(model_layout.model_id);
	if (p_model == NULL)
	{
		return false;
	}

	vector<AiLine> lines;
	// �ض���4���ڵ㣬��������ֱ��ʹ��4
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

	// ������ǰ���͵�ǽ��
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
			// �ҵ�ƽ�е�ֱ��
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
			// �ҵ�ƽ�е�ֱ��
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

	// ��Ҫ��չ�ļҾ߲�����ǽ��
	if (line_index == -1)
	{
		return false;
	}

	// �Ҿߵ���Ч��
	Point3f pA = p_wall->start_corner.point;
	Point3f pB = p_wall->end_corner.point;

	// �ж��Ƿ��ܹ����ּҾ�
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

	// ��չ������ݴ洢
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

	// ����ģ�ͳ�����Ϣ
	p_model->width = model_layout.width;
	return true;

}
//////////////////////////////////��չ��ط���/////////////////////////////
// �����չ���ģ��
bool MetisRegionResolver::GetScaleModel(SingleFurnitureSingleRuleLayoutData &model_layout, map<int, SingleFurnitureSingleRuleLayoutData> design_layout)
{
	// �жϲ������Ƿ������չ��ģ��
	if (design_layout.find(model_layout.model_id) == design_layout.end())
	{
		return false;
	}
	SingleFurnitureSingleRuleLayoutData current_layout = design_layout[model_layout.model_id];
	if (current_layout.furniture_vertex_points.size() != 4)
	{
		return false;
	}

	// ���ģ����Ϣ
	Model *p_model = GetModelInfoById(model_layout.model_id);
	if (p_model == NULL)
	{
		return false;
	}

	vector<AiLine> lines;
	// �ض���4���ڵ㣬��������ֱ��ʹ��4
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

	// ������ǰ���͵�ǽ��
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
			// �ҵ�ƽ�е�ֱ��
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
			// �ҵ�ƽ�е�ֱ��
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

	// ��Ҫ��չ�ļҾ߲�����ǽ��
	if (line_index == -1)
	{
		return false;
	}

	// �Ҿߵ���Ч��
	Point3f pA = p_wall->start_corner.point;
	Point3f pB = p_wall->end_corner.point;

	// �ҵ������ǽ���������Чǽ��-����ȥ�Ż򴰻�
	//A--------------d0------------d1-------------B ԭʼǽ��
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

	// ����
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

	// ���ݼҾ߿��������ǽ����չ�Ҿ�
	// A----pedal0-----pedal1------B
	// ----p0---------p1--------------line0
	// ----p2---------p3--------------line1
	// ����AB����ֱ�ߵĴ���
	Point3f p0 = ComMath::getPointToLinePedal(pA, lines[0].start_point, lines[0].end_point);
	Point3f p1 = ComMath::getPointToLinePedal(pB, lines[0].start_point, lines[0].end_point);
	Point3f p2 = ComMath::getPointToLinePedal(pB, lines[1].start_point, lines[1].end_point);
	Point3f p3 = ComMath::getPointToLinePedal(pA, lines[1].start_point, lines[1].end_point);
	Point3f new_pA = pA;
	Point3f new_pB = pB;
	// ��������ǽ������
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

	// ��չ������ݴ洢
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

// ���ģ���뻧�͵���ײ
// ʹ��ʱ�����ϻ����չģ�͵Ĳ��������򱨴�
bool MetisRegionResolver::CheckModelWithHouse(Point3f p0, Point3f p1, Point3f p2, Point3f p3, Point3f &pA, Point3f &pB)
{
	// ������ǰ���͵�ǽ��
	Wall *p_wall = new Wall();
	Point3f pedal0;
	Point3f pedal1;
	bool end_flag = true;

	// �������ǽ��
	//p0--------------p1
	//-----------------
	//p3--------------p2
	for (auto &wall : p_region->wall_list)
	{
		Point3f pC = wall.start_corner.point;
		Point3f pD = wall.end_corner.point;
		// ���Թ��ߵ����
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

	//  û��
	if (end_flag)
	{
		goto right_wall;
	}
	// ��������ص�����
	for (auto &door : p_wall->door_list)
	{
		Point3f d0 = door.start_point;
		Point3f d1 = door.end_point;

		// �Ҿ����ų�ͻ
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

	// ��������ص�����
	for (auto &window : p_wall->window_list)
	{
		Point3f d0 = window.start_point;
		Point3f d1 = window.end_point;

		// �Ҿ����ų�ͻ
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
	// �����ұ�ǽ��
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

	//  û�к��ʵ�ǽ��
	if (end_flag)
	{
		return true;
	}

	// ��������ص�����
	for (auto &door : p_wall->door_list)
	{
		Point3f d0 = door.start_point;
		Point3f d1 = door.end_point;

		// �Ҿ����ų�ͻ
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

	// ��������ص�����
	for (auto &window : p_wall->window_list)
	{
		Point3f d0 = window.start_point;
		Point3f d1 = window.end_point;

		// �Ҿ����ų�ͻ
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
//  �����Ҿߵ���ײ
bool MetisRegionResolver::CheckModelWithLayout(SingleFurnitureSingleRuleLayoutData model_layout, map<int, SingleFurnitureSingleRuleLayoutData> &design_layout)
{
	/** ����벼���ڹ����Ƿ���ײ*/
	for (auto& layout : design_layout)
	{
		/** ���������� */
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

//  �Ҿ������Դ���
bool MetisRegionResolver::CheckFurnitureAdsorption(map<int, SingleFurnitureSingleRuleLayoutData> &design_layout)
{
	// ѡ���������Ҫ������ǽ��ͼҾ߶�Ӧ�ı�
	for (auto& layout : design_layout)
	{
		AdsorptivityData result_data;
		if (FurnitureAdsorption(layout.second, result_data))
		{

			SingleFurnitureSingleRuleLayoutData &tmp_furniture = layout.second;
			Model *p_model = GetModelInfoById(tmp_furniture.model_id);
			float move_length = tmp_furniture.width;

			// ����Ҿ�
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

			// ���¸��ƼҾߵ�
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
// ���ݼҾ߱�Ż�üҾ���Ϣ
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

// �Ҿ��Ƿ���Ҫ�����Դ��������Ҫ���������Եĵ�
bool MetisRegionResolver::FurnitureAdsorption(SingleFurnitureSingleRuleLayoutData model_layout, AdsorptivityData &adsorption)
{
	// ����ǽ��
	for (auto &wall : p_region->wall_list)
	{
		for (int i = 0; i < model_layout.furniture_vertex_points.size(); i++)
		{
			int j = (i + 1) % model_layout.furniture_vertex_points.size();
			float angle = ComMath::GetAngleBetweenTwoLines(wall.start_corner.point - wall.end_corner.point, model_layout.furniture_vertex_points[i] - model_layout.furniture_vertex_points[j]);
			angle = angle / M_PI * 180;
			if (angle < 5 || angle > 175)
			{
				// �Ҿ��Ƿ񿿽�ǽ��
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
// �Ҿ��Ƿ񿿽���ǽ��
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
