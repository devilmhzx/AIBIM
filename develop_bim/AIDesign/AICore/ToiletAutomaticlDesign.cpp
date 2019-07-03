#include "ToiletAutomaticlDesign.h"
#include <vector>


bool operator<(const BoundingBox&left, const BoundingBox&right)
{
	return left.Inner_type < right.Inner_type;
}

ToiletAutomaticlDesign::ToiletAutomaticlDesign()
{
}

ToiletAutomaticlDesign::~ToiletAutomaticlDesign()
{
}

bool ToiletAutomaticlDesign::AutoDesign(SingleCase*single_case) 
{
	if (!single_case)
		return false;
	m_single_case = single_case;
	// ������
	InitLayoutDoor();
	// ������Ͱ
	LayoutCommode();
	// ���ֻ���
	LayoutSprinkler();
	//������ԡ��
	LayoutNormalBathroom();
	//����һ��ԡ��
	//LayoutBathroomWithLineShape();
	//����ԡ��
	LayoutCabinet();
	//����ԡ��
	LayoutBathtub();
	//��չ����
	ExpandLayout();
	// ���ɺ�ԡ�׵Ĳ���
	ExpandBathtubLayout();
	//��̬����ϴ�»�
	LayoutWasher(case_layout);
	if (!case_layout.size())
		return false;

	// �������յĲ��ֽṹ
	FillModelToSingleCase();
	return true;
}

// ��ʼ���ŵĲ���
void ToiletAutomaticlDesign::InitLayoutDoor()
{
	for (auto &wall : m_single_case->wall_list)
	{
		for (auto &door : wall.door_list)
		{
			auto bounding_box = CalculateDoorByWall(&door,&wall);
			if (bounding_box.flag)
			{
				door_layout.push_back(bounding_box);
			}
		}
	}
}
void ToiletAutomaticlDesign::LayoutCommode() 
{
	// �ж��Ƿ���д����ֵ���Ͱ
	auto model = FindLayoutModel(GROUP_COMMODE);
	if (model.id == Model_Inner_Type)
		return;
	//�ҿ�λ
	FloorBox*stool = nullptr;
	for (auto &floor_box : m_single_case->floor_box_list)
	{
		if (floor_box.floor_box_type == FB_STOOL) {
			stool = &floor_box;
		}
	}
	if (!stool)
		return;

	//�������ǽ��
	std::vector<Wall*> wall_to_stool_dist;
	Wall*  min_dist = nullptr;
	float max = FLT_MAX;
	for (auto &wall : m_single_case->wall_list)
	{
		auto stool_pos = stool->center;
		auto dist = ComMath::getPointToLineDis(stool_pos, wall.start_corner.point, wall.end_corner.point);
		if(dist<= Commode_Dis)
			wall_to_stool_dist.push_back(&wall);
		if (dist < max) {
			min_dist = &wall;
			max = dist;
		}
	}
	//����С��40��ǽ��
	if (wall_to_stool_dist.size())
	{
		for (auto &wall : wall_to_stool_dist)
		{
			//ǽ�峤�ȱ��������Ͱ�Ŀ��,���ܷ���
			if (ComMath::getTwoPointDistance(wall->start_corner.point, wall->end_corner.point) < model.width)
				continue;
			auto box = CalculateCommodeByWall(stool->center, model, wall);
			commode_layout.push_back(box);
			if (commode_layout.size() >= 2)
				break;
		}
	}
	else
	{
		if (ComMath::getTwoPointDistance(min_dist->start_corner.point, min_dist->end_corner.point) > model.width) {
			auto box = CalculateCommodeByWall(stool->center, model, min_dist);
			commode_layout.push_back(box);
		}
	}
}
 
// ��þ����ž����ǽ��
vector<CornerData> ToiletAutomaticlDesign::GetFastCornerDataWithDoor()
{
	// Ѱ����Чǽǽ��
	vector<CornerData> corner_data;
	for (auto &corner : m_single_case->corner_list)
	{
		CornerData current_corner;
		current_corner.corner = corner;
		float max = 0;
		for (auto &door : m_single_case->door_list)
		{
			auto dist = ComMath::getTwoPointDistance(corner.point, door.pos);
			if (dist > max)
			{
				max = dist;
			}
		}
		current_corner.distance = max;
		corner_data.push_back(current_corner);
	}

	// ����ǽ�Ǿ���
	corner_data = SortCornerData(corner_data,0);

	return corner_data;
}

// ��þ����ŽϽ���ǽ��
vector<CornerData> ToiletAutomaticlDesign::GetNearCornerDataWithDoor()
{
	// Ѱ����Чǽǽ��
	vector<CornerData> corner_data;
	for (auto &corner : m_single_case->corner_list)
	{
		CornerData current_corner;
		current_corner.corner = corner;
		float min = 9999;
		for (auto &door : m_single_case->door_list)
		{
			auto dist = ComMath::getTwoPointDistance(corner.point, door.pos);
			if (dist < min)
			{
				min = dist;
			}
		}
		current_corner.distance = min;
		corner_data.push_back(current_corner);
	}

	// ����ǽ�Ǿ���
	corner_data = SortCornerData(corner_data,1);

	return corner_data;
}

// ���ֻ���
void ToiletAutomaticlDesign::LayoutSprinkler() 
{
	auto model = FindLayoutModel(GROUP_SPRINKLER);
	if (model.id == Model_Inner_Type)
		return;
	// ��������ǽ��
	vector<CornerData> corner_data = GetFastCornerDataWithDoor();
	if (corner_data.size() < 2)
	{
		return;
	}

	// ѡ������ǽ�ǲ��ֻ���
	//��ӵ�еĽ��䲢��ǽ������û�д���������ǽ�岻����
	for (int i = 0; i < 2; i++)
	{
		vector<Wall*> relative_walls = m_single_case->GetWallsByCorner(&corner_data[i].corner);
		for (int j = 0; j < relative_walls.size(); j++)
		{
			Wall *p_wall = relative_walls[j];

			// ǽ�峤��С�ڻ�������
			if (ComMath::getTwoPointDistance(p_wall->start_corner.point, p_wall->end_corner.point) < model.width)
			{
				continue;
			}
			// ǽ���������Ŵ�
			if (p_wall->door_list.size() > 0 || p_wall->window_list.size() > 0)
			{
				continue;
			}

			// ǽ������Ż򴰻�
			/*if (m_single_case->IsWallFaceToDoor(p_wall))
			{
				continue;
			}*/

			// ���в���
			auto bounding_box = CalculateSprinklerByWall(&corner_data[i].corner, model, p_wall);
			sprinkler_layout.push_back(bounding_box);
		}
	}
}


// ������ԡ��
void ToiletAutomaticlDesign::LayoutNormalBathroom() 
{
	//���ģ��
	auto model = FindLayoutModel(GROUP_BATH_ROOM);
	if (model.id == Model_Inner_Type)
		return;

	//���������С����ƽ�ײ�����
	auto area = ComMath::GetPolygonArea(m_single_case->single_room.point_list);
	if (area < Toilet_Area)
		return;

	// ��������ǽ��
	vector<CornerData> corner_data = GetFastCornerDataWithDoor();
	if (corner_data.size() < 2)
	{
		return;
	}

	// ѡ������ǽ�ǲ�����ԡ��
	//��ӵ�еĽ��䲢��ǽ������û�д���������ǽ�岻����
	for (int i = 0; i < 2; i++)
	{
		vector<Wall*> relative_walls = m_single_case->GetWallsByCorner(&corner_data[i].corner);
		for (int j = 0; j < relative_walls.size(); j++)
		{
			Wall *p_wall = relative_walls[j];

			// ǽ�峤��С����ԡ������
			if (ComMath::getTwoPointDistance(p_wall->start_corner.point, p_wall->end_corner.point) < model.width)
			{
				continue;
			}
			// ǽ���������Ŵ�
			if (p_wall->door_list.size() > 0 || p_wall->window_list.size() > 0)
			{
				continue;
			}

			// ǽ������Ż򴰻�
			/*if (m_single_case->IsWallFaceToDoor(p_wall))
			{
				continue;
			}*/

			// ���в���
			auto bounding_box = CalculateNormalBathroomByWall(&corner_data[i].corner, model, p_wall);
			bathroom_layout.push_back(bounding_box);
		}
	}
}

// ����һ������ԡ��
void ToiletAutomaticlDesign::LayoutBathroomWithLineShape() 
{
	//�޲���Ҫ��
	auto model = FindLayoutModel(GROUP_SPRINKLER);
	if (model.id == Model_Inner_Type)
		return;

	//���������С����ƽ�ײ�����
	auto area = ComMath::GetPolygonArea(m_single_case->single_room.point_list);
	if (area < Toilet_Area)
		return;

	// ��������ǽ��
	vector<CornerData> corner_data = GetFastCornerDataWithDoor();
	if (corner_data.size() < 2)
	{
		return;
	}

	// ѡ������ǽ�ǲ�����ԡ��
	for (int i = 0; i < 2; i++)
	{
		vector<Wall*> relative_walls = m_single_case->GetWallsByCorner(&corner_data[i].corner);
		for (int j = 0; j < relative_walls.size(); j++)
		{
			Wall *p_wall = relative_walls[j];

			// ǽ�峤��С����ԡ������
			if (ComMath::getTwoPointDistance(p_wall->start_corner.point, p_wall->end_corner.point) < model.width)
			{
				continue;
			}

			// ���в���
			auto bounding_box = CalculateBathroomWithLineShape(&corner_data[i].corner, model, p_wall);
			bathroom_layout.push_back(bounding_box);
		}
	}

	// ԡ�׼�����ŵ���ײ����
	vector<BoundingBox>::iterator it = sprinkler_layout.begin();
	for (; it != sprinkler_layout.end();)
	{
		if (IsCollisionDoor(*it))
		{
			it = sprinkler_layout.erase(it);
		}
		else
			//������ָ����һ��Ԫ��λ��
			++it;
	}
}

// ����ԡ�ҹ�
void ToiletAutomaticlDesign::LayoutCabinet() 
{
	auto model = FindLayoutModel(GROUP_CABINET_BATH);
	if (model.id == Model_Inner_Type)
		return;

	// ���ŽϽ���ǽ��������ԡ�ҹ�
	// ��������ǽ��
	vector<CornerData> corner_data = GetNearCornerDataWithDoor();
	if (corner_data.size() < 2)
	{
		return;
	}

	// ѡ������ǽ�ǲ���ԡ�ҹ�
	for (int i = 0; i < corner_data.size()-2; i++)
	{
		Corner *current_corner = &corner_data[i].corner;
		vector<Wall*> relative_walls = m_single_case->GetWallsByCorner(current_corner);
		for (int j = 0; j < relative_walls.size(); j++)
		{
			Wall *p_wall = relative_walls[j];

			// ǽ�峤��С��ԡ�ҹ񳤶�
			if (ComMath::getTwoPointDistance(p_wall->start_corner.point, p_wall->end_corner.point) < model.width)
			{
				continue;
			}

			// ���ǽ���������Ų����д������������
			if (p_wall->door_list.size() > 0 && p_wall->window_list.size() > 0)
			{
				continue;
			}
			// ���ǽ�������д���
			if (p_wall->window_list.size() > 0)
			{
				// ������������1��Ҳ������
				if (p_wall->window_list.size() > 1)
				{
					continue;
				}
				Window tmp_window = p_wall->window_list[0];
				auto end_point = tmp_window.start_point;
				if (ComMath::getTwoPointDistance(current_corner->point, tmp_window.start_point) > ComMath::getTwoPointDistance(current_corner->point, tmp_window.end_point))
				{
					end_point = tmp_window.end_point;
				}

				// ����ԡ�ҹ�
				auto bounding_box = CalculateCabinetByCorner(current_corner->point, end_point, model, p_wall);
				if (bounding_box.flag)
				{
					cabinet_layout.push_back(bounding_box);
				}
				continue;
			}

			// ���ǽ�������Ų����ŵĸ���Ϊ1�����д���
			if (p_wall->door_list.size() >0)
			{
				if (p_wall->door_list.size() > 1)
				{
					continue;
				}

				Door tmp_door = p_wall->door_list[0];
				auto end_point = tmp_door.start_point;
				if (ComMath::getTwoPointDistance(current_corner->point, tmp_door.start_point) > ComMath::getTwoPointDistance(current_corner->point, tmp_door.end_point))
				{
					end_point = tmp_door.end_point;
				}

				// ���յ�λ���в���ԡ�ҹ�
				auto bounding_box = CalculateCabinetByCorner(current_corner->point, end_point, model, p_wall);
				if (bounding_box.flag)
				{
					cabinet_layout.push_back(bounding_box);
				}

				// ���տ�ʼ����в���ԡ�ҹ�
				auto bounding_box2 = CalculateCabinetByPoint(current_corner->point, end_point, model, p_wall);
				if (bounding_box2.flag)
				{
					cabinet_layout.push_back(bounding_box2);
				}
				continue;
			}

			// ǽ��ʲô��û�н��в���
			auto end_point = p_wall->start_corner.point;
			if (ComMath::getTwoPointDistance(current_corner->point, p_wall->start_corner.point) < ComMath::getTwoPointDistance(current_corner->point, p_wall->end_corner.point))
			{
				end_point = p_wall->end_corner.point;
			}

			// ����ԡ�ҹ�
			auto bounding_box = CalculateCabinetByCorner(current_corner->point, end_point, model, p_wall);
			if (bounding_box.flag)
			{
				cabinet_layout.push_back(bounding_box);
			}
			continue;
		}
	}

	// ԡ�ҹ������ŵ���ײ����
	vector<BoundingBox>::iterator it = cabinet_layout.begin();
	for (; it != cabinet_layout.end();)
	{
		if (IsCollisionDoor(*it))
		{
			it = cabinet_layout.erase(it);
		}
		else
			//������ָ����һ��Ԫ��λ��
			++it;
	}
}

// ��չ���ֽ�����Ч���ֵ����
void ToiletAutomaticlDesign::ExpandLayout() 
{
	int model_count = 0;

	// ��������ԡ���Ĵ���
	bool is_sprinkler = true;
	if (sprinkler_layout.size() < 1)
	{
		if (bathroom_layout.size() > 0)
		{
			sprinkler_layout = bathroom_layout;
			is_sprinkler = false;
		}
	}

	if (commode_layout.size() > 0)
	{
		model_count++;
	}

	if (sprinkler_layout.size() > 0)
	{
		model_count++;
	}

	if (cabinet_layout.size() > 0)
	{
		model_count++;
	}
	ToiletDesign tmp_design_layout;
	// ģ��һ�׼ٵĲ��ַ����������������
	// ��Ͱ��������
	commode_layout.push_back(GetVirtualBox(GROUP_COMMODE));
	// ������������
	if (is_sprinkler)
	{
		sprinkler_layout.push_back(GetVirtualBox(GROUP_SPRINKLER));
	}
	else
	{
		sprinkler_layout.push_back(GetVirtualBox(GROUP_BATH_ROOM));
	}
	
	// ԡ�ҹ���������
	cabinet_layout.push_back(GetVirtualBox(GROUP_CABINET_BATH));
	// ԡ����������
	//bathtub_layout.push_back(GetVirtualBox(GROUP_BATHTUB));

	// ��Ͱ
	for (auto &commode : commode_layout)
	// ���в��ֵ����
	{
		// ����
		for (auto &sprinkler : sprinkler_layout)
		{
			// ��Ͱ�뻨����ײ
			if (IsCollisionBoundingBox(commode, sprinkler))
			{
				continue;
			}
			// ԡ�ҹ�
			for (auto &cabinet : cabinet_layout)
			{
				// ��Ͱ��ԡ�ҹ���ײ
				if (IsCollisionBoundingBox(commode, cabinet))
				{
					continue;
				}
				// ������ԡ�ҹ���ײ
				if (IsCollisionBoundingBox(sprinkler, cabinet))
				{
					continue;
				}

				// ����һ�׺�����
				std::set<BoundingBox> temp_set;
				temp_set.insert(commode);
				temp_set.insert(sprinkler);
				temp_set.insert(cabinet);
				tmp_design_layout.push_back(temp_set);
			}
		}
	}

	// ȥ�������а������ⷽ���ķ���
	for (auto &tmp_single : tmp_design_layout)
	{
		int accept_count = 0;
		std::set<BoundingBox> temp_set;
		for (auto&tmp_box : tmp_single)
		{
			if (tmp_box.flag)
			{
				accept_count++;
				temp_set.insert(tmp_box);
			}
		}
		if (accept_count == model_count && model_count > 0)
		{
			case_layout.push_back(temp_set);
		}
	}
}

// ���ɺ�ԡ�׵Ĳ���
void ToiletAutomaticlDesign::ExpandBathtubLayout()
{
	if (bathtub_layout.size() < 1)
	{
		return;
	}
	if (case_layout.size() > 0)
	{
		ToiletDesign tmp_design_layout;
		// ԡ�״���
		for (auto &tmp_single : case_layout)
		{
			for (auto &bathtub : bathtub_layout)
			{
				BoundingBoxSet new_layout = CopyBoundingBoxSet(tmp_single);
				if (CheckBathtubLayout(tmp_single, bathtub))
				{
					new_layout.insert(bathtub);
				}
				tmp_design_layout.push_back(new_layout);
			}
		}

		if (tmp_design_layout.size() != case_layout.size())
		{
			case_layout.clear();
			for (auto &tmp_layout : tmp_design_layout)
			{
				case_layout.push_back(tmp_layout);
			}
		}
	}
	else
	{
		for (auto &bathtub : bathtub_layout)
		{
			std::set<BoundingBox> temp_set;
			temp_set.insert(bathtub);
			case_layout.push_back(temp_set);
		}
	}
}

// �ж�ԡ���ڲ������Ƿ����
bool ToiletAutomaticlDesign::CheckBathtubLayout(BoundingBoxSet&single_layout, BoundingBox bathtub)
{
	for (auto&tmp_box : single_layout)
	{
		if (tmp_box.Inner_type == GROUP_SPRINKLER)
		{
			continue;
		}

		if (tmp_box.model.id == bathtub.model.id)
		{
			continue;
		}

		if (IsCollisionBoundingBox(tmp_box, bathtub))
		{
			return false;
		}
	}

	return true;
}
// ����ԡ��
void ToiletAutomaticlDesign::LayoutBathtub() 
{
	auto model = FindLayoutModel(GROUP_BATHTUB);
	if (model.id == Model_Inner_Type)
		return;


	// ��������ǽ��
	vector<CornerData> corner_data = GetFastCornerDataWithDoor();
	if (corner_data.size() < 2)
	{
		return;
	}

	// ѡ������ǽ�ǲ���ԡ��
	for (int i = 0; i < 2; i++)
	{
		vector<Wall*> relative_walls = m_single_case->GetWallsByCorner(&corner_data[i].corner);
		for (int j = 0; j < relative_walls.size(); j++)
		{
			Wall *p_wall = relative_walls[j];

			// ǽ�峤��С��ԡ�׳���
			if (ComMath::getTwoPointDistance(p_wall->start_corner.point, p_wall->end_corner.point) < model.width)
			{
				continue;
			}
			// ���в���
			auto bounding_box = CalculateBathtubByWall(&corner_data[i].corner, model, p_wall);
			bathtub_layout.push_back(bounding_box);
		}
	}

	// ԡ�׼�����ŵ���ײ����
	vector<BoundingBox>::iterator it = bathtub_layout.begin();
	for (; it != bathtub_layout.end();)
	{
		if (IsCollisionDoor(*it))
		{
			it = bathtub_layout.erase(it);
		}
		else
			//������ָ����һ��Ԫ��λ��
			++it;
	}

}

// ��̬����ϴ�»�
void ToiletAutomaticlDesign::LayoutWasher(ToiletDesign &ref)
{
	auto model = FindLayoutModel(GROUP_WASHER);
	if (model.id == Model_Inner_Type)
		return;

	// û�в��֣���������ϴ�»�
	if (ref.size() < 1)
	{
		AloneLayoutWasher(model);
		return;
	}

	for (auto &per_case : ref)
	{
		bool corner_flag = false;
		for(auto &corner : m_single_case->corner_list)
		{
			if (DynamicLayoutWasherByCorner(per_case, &corner,model))
			{
				corner_flag = true;
				break;
			}
		}
		
		if (corner_flag)
		{
			break;
		}

		for (auto &wall : m_single_case->wall_list)
		{
			if (DynamicLayoutWasher(per_case, &wall, model))
			{
				break;
			}	
		}
	}
}
 
//����ǽ���Զ�����ϴ�»�
bool ToiletAutomaticlDesign::DynamicLayoutWasher(BoundingBoxSet&single_layout, Wall*wall, Model model) 
{
	auto res = false;
	if (ComMath::getTwoPointDistance(wall->end_corner.point, wall->start_corner.point) < model.width)
		return res;
	auto normal = CalculateWallNormal(wall);
	auto direction = ComMath::Normalize(wall->end_corner.point-wall->start_corner.point);
	//�����ʼBoundingBox
	BoundingBox box;
	BoundingBox extend_box;
	box.Direction = direction;
	box.Wall = wall;
	box.model = model;
	box.flag = true;
	
	extend_box.Direction = direction;
	extend_box.Wall = wall;
	extend_box.model = model;
	extend_box.flag = true;


	auto wall_length = ComMath::getTwoPointDistance(wall->end_corner.point, wall->start_corner.point);
	auto start_length = 0.0f;
	auto end_length = wall_length - model.width;
	
	while (start_length < end_length)
	{
		auto V1 = wall->start_corner.point + direction * start_length;
		auto V2 = wall->start_corner.point + direction * (model.width + start_length);
		auto V3 = V1 + normal * model.length;
		auto V4 = V2 + normal * model.length;
		box.Max = ComMath::Point3fMax(V1, ComMath::Point3fMax(V2, ComMath::Point3fMax(V3, V4)));
		box.Min = ComMath::Point3fMin(V1, ComMath::Point3fMin(V2, ComMath::Point3fMin(V3, V4)));
		box.location.x = (V2.x + V3.x) / 2;
		box.location.y = (V2.y + V3.y) / 2;

		auto E_V1 = wall->start_corner.point + direction * start_length;
		auto E_V2 = wall->start_corner.point + direction * (model.width + start_length+ Commode_Extend);
		auto E_V3 = E_V1 + normal * (model.length+ Commode_Extend);
		auto E_V4 = E_V2 + normal * (model.length+ Commode_Extend);
		box.Max = ComMath::Point3fMax(V1, ComMath::Point3fMax(V2, ComMath::Point3fMax(V3, V4)));
		box.Min = ComMath::Point3fMin(V1, ComMath::Point3fMin(V2, ComMath::Point3fMin(V3, V4)));
		box.location.x = (V2.x + V3.x) / 2;
		box.location.y = (V2.y + V3.y) / 2;


		float accept = true;
		// �ж��Ƿ����ų�ͻ
		if (IsCollisionDoor(box))
		{
			accept = false;
			goto no_accept;
		}

		for (auto &boundbox : single_layout)
		{
			if (IsCollisionBoundingBox(box, boundbox))
			{
				accept = false;
				break;
			}
		}

     no_accept:
		if (!accept)
		{
			start_length += DynanmicLayoutStep;
		}
		else
		{
			res = true;
			break;
		}
	}

	if (res)
		single_layout.insert(box);
	return res;
}
 
//���Ž����Զ�����ϴ�»�
bool ToiletAutomaticlDesign::DynamicLayoutWasherByCorner(BoundingBoxSet&single_layout, Corner*corner, Model model)
{
	vector<Wall*> relative_walls = m_single_case->GetWallsByCorner(corner);
	for (int i = 0; i < relative_walls.size(); i++)
	{
		Wall *p_wall = relative_walls[i];
		if (ComMath::getTwoPointDistance(p_wall->end_corner.point, p_wall->start_corner.point) < model.width)
		{
			continue;
		}

		auto end_point = p_wall->start_corner.point;
		if (corner->GetNo() == p_wall->start_corner.GetNo())
		{
			end_point = p_wall->end_corner.point;
		}
		auto normal = CalculateWallNormal(p_wall);
		auto direction = ComMath::Normalize(end_point - corner->point);

		//�����ʼBoundingBox
		BoundingBox box;
		BoundingBox extend_box;
		box.Direction = direction;
		box.Wall = p_wall;
		box.model = model;
		box.flag = true;

		auto V1 = corner->point;
		auto V2 = V1 + direction * model.width;
		auto V3 = V1 + normal * model.length;
		auto V4 = V2 + normal * model.length;
		box.Max = ComMath::Point3fMax(V1, ComMath::Point3fMax(V2, ComMath::Point3fMax(V3, V4)));
		box.Min = ComMath::Point3fMin(V1, ComMath::Point3fMin(V2, ComMath::Point3fMin(V3, V4)));
		box.location.x = (V2.x + V3.x) / 2;
		box.location.y = (V2.y + V3.y) / 2;

		float accept = true;
		// �ж��Ƿ����ų�ͻ
		if (IsCollisionDoor(box))
		{
			continue;;
		}

		for (auto &boundbox : single_layout)
		{
			if (IsCollisionBoundingBox(box, boundbox))
			{
				accept = false;
				break;
			}
		}

		if (accept)
		{
			single_layout.insert(box);
			return true;
		}
	}

	return false;
}
// ���ɽ��
void ToiletAutomaticlDesign::FillModelToSingleCase() 
{
	for (auto per_case : case_layout)
	{
		ModelLayout layout;
		FillModelToModelLayout(layout,per_case);
		m_single_case->layout_list.push_back(layout);
	}
}

// ���л�ÿһ�ײ���
void ToiletAutomaticlDesign::FillModelToModelLayout(ModelLayout&layout,BoundingBoxSet&Set) 
{
	for (auto boundbox : Set) 
	{
		//Model model(boundbox.model);
		auto centre = ComMath::Cross(Point3f(1.f, 0.f, 0.f), boundbox.Direction);
		auto angle = ComMath::GetAngleBetweenTwoLines(Point3f(1.f, 0.f, 0.f), boundbox.Direction);
		boundbox.model.rotation = boundbox.Direction;
		if (centre.z > 0) {
			boundbox.model.rotation.y = angle / (3.1415926f * 2) * 360;
		}
		else {
			boundbox.model.rotation.y = -angle / (3.1415926f * 2) * 360;
		}
		//����������ģ�͵ĸ߶�
		//@��С��
		if (boundbox.model.id == GROUP_SPRINKLER)
		{
			boundbox.location.z += 100.f;
		}

		boundbox.model.location = boundbox.location;
		layout.model_list.push_back(boundbox.model);
	}
}

// Ѱ��ģ��
Model ToiletAutomaticlDesign::FindLayoutModel(int id)
{   
	for (auto &model : m_single_case->model_list) {


		if (model.id == id){
			return model;
		}

	}

	Model result;
	result.id = Model_Inner_Type;
	return result;
}

// ������
//V2---------V1
//V4---------V3
BoundingBox ToiletAutomaticlDesign::CalculateDoorByWall( Door*door, Wall*wall)
{
	BoundingBox box;
	if (door->door_type == D_SLIDING || door->door_type == D_PASS)
	{
		box.flag = false;
		return box;
	}

	auto normal = CalculateWallNormal(wall);
	auto V1 = door->start_point;
	auto V2 = door->end_point;
	auto tmp_length = 80.f;
	auto V3 = V1 + normal * tmp_length;
	auto V4 = V2 + normal * tmp_length;
	box.Max = ComMath::Point3fMax(V1, ComMath::Point3fMax(V2, ComMath::Point3fMax(V3, V4)));
	box.Min = ComMath::Point3fMin(V1, ComMath::Point3fMin(V2, ComMath::Point3fMin(V3, V4)));
	box.Direction = normal;
	box.Inner_type = GROUP_SPRINKLER;
	box.Wall = wall;
	box.location.x = (V2.x + V3.x) / 2;
	box.location.y = (V2.y + V3.y) / 2;
	box.flag = true;

	return box;
}

// ������Ͱ��λ��
BoundingBox ToiletAutomaticlDesign::CalculateCommodeByWall(Point3f stool, Model model, Wall*wall) 
{
	auto proj = ComMath::getPointToLinePedal(stool, wall->start_corner.point, wall->end_corner.point);
	auto direction = ComMath::Normalize(wall->start_corner.point - wall->end_corner.point);
	auto normal = CalculateWallNormal(wall);
	Point3f bound[4];
 
	bound[0] = proj + direction*(model.width/2.f+ Commode_Extend);
	bound[1] = proj - direction*(model.width/2.f + Commode_Extend);
	bound[2] = bound[0] + normal*(model.length + Commode_Extend);
	bound[3] = bound[1] + normal*(model.length + Commode_Extend);
	BoundingBox box;
	box.Max = ComMath::Point3fMax(bound[0], ComMath::Point3fMax(bound[1], ComMath::Point3fMax(bound[2], bound[3])));
	box.Min = ComMath::Point3fMin(bound[0], ComMath::Point3fMin(bound[1], ComMath::Point3fMin(bound[2], bound[3])));
	box.Direction = normal;
	box.Inner_type = GROUP_COMMODE;
	box.model = model;
	box.Wall = wall;
	box.location = stool;
	box.flag = true;
	box.location.x = (bound[0].x + bound[3].x) / 2;
	box.location.y = (bound[0].y + bound[3].y) / 2;
	return box;
}

// ���ֻ���
//V2---------V1
//V4---------V3
BoundingBox ToiletAutomaticlDesign::CalculateSprinklerByWall(Corner*corner, Model model, Wall*wall) 
{
	bool on_begin = false;
	if (corner->GetNo() == wall->start_corner.GetNo())
		on_begin = true;
	auto direction = ComMath::Normalize(wall->end_corner.point - wall->start_corner.point);
	BoundingBox box;
	if (on_begin)
	{
		auto V1 = wall->start_corner.point + direction*(model.width+ Sprinkler_Extend);
		auto &V2 = wall->start_corner.point;
		auto normal = CalculateWallNormal(wall);
		auto V3 = V1 + normal*(model.length+ Sprinkler_Extend);
		auto V4 = V2 + normal*(model.length+ Sprinkler_Extend);
		box.Max = ComMath::Point3fMax(V1, ComMath::Point3fMax(V2, ComMath::Point3fMax(V3, V4)));
		box.Min = ComMath::Point3fMin(V1, ComMath::Point3fMin(V2, ComMath::Point3fMin(V3, V4)));
		box.Direction = normal;
		box.Inner_type = GROUP_SPRINKLER;
		box.model = model;
		box.Wall = wall;
		box.flag = true;
		box.location.x = (V2.x + V3.x) / 2;
		box.location.y = (V2.y + V3.y) / 2;
	}
	else
	{
		auto V1 = wall->end_corner.point - direction*(model.width+ Sprinkler_Extend);
		auto &V2 = wall->end_corner.point;
		auto normal = CalculateWallNormal(wall);
		auto V3 = V1 + normal*(model.length+ Sprinkler_Extend);
		auto V4 = V2 + normal*(model.length+ Sprinkler_Extend);

		box.Max = ComMath::Point3fMax(V1, ComMath::Point3fMax(V2, ComMath::Point3fMax(V3, V4)));
		box.Min = ComMath::Point3fMin(V1, ComMath::Point3fMin(V2, ComMath::Point3fMin(V3, V4)));
		box.Direction = normal;
		box.Inner_type = GROUP_SPRINKLER;
		box.model = model;
		box.Wall = wall;
		box.flag = true;
		box.location.x = (V2.x + V3.x) / 2;
		box.location.y = (V2.y + V3.y) / 2;
	}
	return box;
}

// ������ԡ��
//V2---------V1
//V4---------V3
BoundingBox ToiletAutomaticlDesign::CalculateNormalBathroomByWall(Corner*corner, Model model, Wall*wall) 
{
	bool on_begin = false;
	if (corner->GetNo() == wall->start_corner.GetNo())
		on_begin = true;
	auto direction = ComMath::Normalize(wall->end_corner.point - wall->start_corner.point);
	BoundingBox box;

	if (on_begin)
	{
		auto V1 = wall->start_corner.point + direction * model.width;
		auto &V2 = wall->start_corner.point;
		auto normal = CalculateWallNormal(wall);
		auto V3 = V1 + normal * model.length;
		auto V4 = V2 + normal * model.length;
		box.Max = ComMath::Point3fMax(V1, ComMath::Point3fMax(V2, ComMath::Point3fMax(V3, V4)));
		box.Min = ComMath::Point3fMin(V1, ComMath::Point3fMin(V2, ComMath::Point3fMin(V3, V4)));
		box.Direction = normal;
		box.Inner_type = GROUP_SPRINKLER;
		box.model = model;
		box.Wall = wall;
		box.flag = true;
		box.location.x = (V2.x + V3.x) / 2;
		box.location.y = (V2.y + V3.y) / 2;
	}
	else
	{
		auto V1 = wall->end_corner.point - direction * model.width;
		auto &V2 = wall->end_corner.point;
		auto normal = CalculateWallNormal(wall);
		auto V3 = V1 + normal * model.length;
		auto V4 = V2 + normal * model.length;

		box.Max = ComMath::Point3fMax(V1, ComMath::Point3fMax(V2, ComMath::Point3fMax(V3, V4)));
		box.Min = ComMath::Point3fMin(V1, ComMath::Point3fMin(V2, ComMath::Point3fMin(V3, V4)));
		box.Direction = normal;
		box.Inner_type = GROUP_SPRINKLER;
		box.model = model;
		box.Wall = wall;
		box.flag = true;
		box.location.x = (V2.x + V3.x) / 2;
		box.location.y = (V2.y + V3.y) / 2;
	}
	return box;
}

// ����һ������ԡ��
//V2---------V1
//V4---------V3
BoundingBox ToiletAutomaticlDesign::CalculateBathroomWithLineShape(Corner*corner, Model model, Wall*wall) 
{

	auto normal = CalculateWallNormal(wall);
	auto V1 = wall->start_corner.point;
	auto V2 = wall->end_corner.point;
	auto V3 = V1 + normal * model.length;
	auto V4 = V2 + normal * model.length;
	BoundingBox box;
	box.Max = ComMath::Point3fMax(V1, ComMath::Point3fMax(V2, ComMath::Point3fMax(V3, V4)));
	box.Min = ComMath::Point3fMin(V1, ComMath::Point3fMin(V2, ComMath::Point3fMin(V3, V4)));
	box.Direction = normal;
	box.Inner_type = GROUP_SPRINKLER;
	model.width = ComMath::getTwoPointDistance(V1, V2);
	box.model = model;
	box.Wall = wall;
	box.flag = true;
	box.location.x = (V2.x + V3.x) / 2;
	box.location.y = (V2.y + V3.y) / 2;

	return box;
}

// ����ԡ�ҹ�
//V1---------V2
//V3---------V4
BoundingBox ToiletAutomaticlDesign::CalculateCabinetByCorner(Point3f beg, Point3f end, Model model, Wall*wall) 
{
	BoundingBox box;
	// �ж��Ƿ��ʺϲ���
	if (ComMath::getTwoPointDistance(beg, end) < model.width)
	{
		box.flag = false;
		return box;
	}
	auto normal = CalculateWallNormal(wall);
	auto direction =ComMath::Normalize(end - beg);
	auto V1 = beg;
	auto V2 = V1 + direction * model.width;;
	auto V3 = V1 + normal *model.length;
	auto V4 = V2+ normal *model.length;
	
	box.Max = ComMath::Point3fMax(V1, ComMath::Point3fMax(V2, ComMath::Point3fMax(V3, V4)));
	box.Min = ComMath::Point3fMin(V1, ComMath::Point3fMin(V2, ComMath::Point3fMin(V3, V4)));
	box.Inner_type = GROUP_CABINET_BATH;
	box.model = model;
	box.Direction = normal;
	box.Wall = wall;
	box.location.x = (V2.x + V3.x) / 2;
	box.location.y = (V2.y + V3.y) / 2;
	box.flag = true;
	return box;
}

// ����ԡ�ҹ�
//V1---------V2
//V3---------V4
BoundingBox ToiletAutomaticlDesign::CalculateCabinetByPoint(Point3f beg, Point3f end, Model model, Wall*wall)
{
	BoundingBox box;
	// �ж��Ƿ��ʺϲ���
	if (ComMath::getTwoPointDistance(beg, end) < (model.width+10))
	{
		box.flag = false;
		return box;
	}

	auto center = (beg + end)*0.5;
	auto normal = CalculateWallNormal(wall);
	auto direction = ComMath::Normalize(end - beg);
	auto V2 = center + direction * (model.width/2);
	auto V1 = center + (-direction * (model.width/2));
	auto V3 = V1 + normal * model.length;
	auto V4 = V2 + normal * model.length;

	box.Max = ComMath::Point3fMax(V1, ComMath::Point3fMax(V2, ComMath::Point3fMax(V3, V4)));
	box.Min = ComMath::Point3fMin(V1, ComMath::Point3fMin(V2, ComMath::Point3fMin(V3, V4)));
	box.Inner_type = GROUP_CABINET_BATH;
	box.model = model;
	box.Direction = normal;
	box.Wall = wall;
	box.location.x = (V2.x + V3.x) / 2;
	box.location.y = (V2.y + V3.y) / 2;
	box.flag = true;
	return box;
}

// ͨ��ǽ��ĵ����ԡ�׵�box
// ע��ԡ�׵ĳߴ糤��Ϊwidth
BoundingBox ToiletAutomaticlDesign::CalculateBathtubByWall(Corner *corner, Model model, Wall*wall)
{
	BoundingBox box;
	bool on_begin = false;
	if (corner->GetNo() == wall->start_corner.GetNo())
		on_begin = true;
	auto direction = ComMath::Normalize(wall->end_corner.point - wall->start_corner.point);

	if (on_begin)
	{
		auto V1 = wall->start_corner.point + direction * model.width;
		auto V2 = wall->start_corner.point;
		auto normal = CalculateWallNormal(wall);
		auto V3 = V1 + normal * model.length;
		auto V4 = V2 + normal * model.length;
		box.Max = ComMath::Point3fMax(V1, ComMath::Point3fMax(V2, ComMath::Point3fMax(V3, V4)));
		box.Min = ComMath::Point3fMin(V1, ComMath::Point3fMin(V2, ComMath::Point3fMin(V3, V4)));
		box.Direction = normal;
		box.Inner_type = GROUP_BATHTUB;
		box.model = model;
		box.Wall = wall;
		box.flag = true;
		box.location.x = (V2.x + V3.x) / 2;
		box.location.y = (V2.y + V3.y) / 2;
	}
	else
	{
		auto V1 = wall->end_corner.point - direction * model.width;
		auto V2 = wall->end_corner.point;
		auto normal = CalculateWallNormal(wall);
		auto V3 = V1 + normal * model.length;
		auto V4 = V2 + normal * model.length;

		box.Max = ComMath::Point3fMax(V1, ComMath::Point3fMax(V2, ComMath::Point3fMax(V3, V4)));
		box.Min = ComMath::Point3fMin(V1, ComMath::Point3fMin(V2, ComMath::Point3fMin(V3, V4)));
		box.Direction = normal;
		box.Inner_type = GROUP_BATHTUB;
		box.model = model;
		box.Wall = wall;
		box.flag = true;
		box.location.x = (V2.x + V3.x) / 2;
		box.location.y = (V2.y + V3.y) / 2;
	}
	return box;
}

//���㲼��ϴ�»�
void ToiletAutomaticlDesign::AloneLayoutWasher(Model model)
{
	for (auto &wall : m_single_case->wall_list)
	{
		if (ComMath::getTwoPointDistance(wall.end_corner.point, wall.start_corner.point) < model.width)
		{
			continue;
		}
		auto normal = CalculateWallNormal(&wall);
		auto direction = ComMath::Normalize(wall.end_corner.point - wall.start_corner.point);
		//�����ʼBoundingBox
		BoundingBox box;
		box.Direction = direction;
		box.Wall = &wall;
		box.model = model;
		box.flag = true;

		auto wall_length = ComMath::getTwoPointDistance(wall.end_corner.point, wall.start_corner.point);
		auto start_length = 0.f;
		auto end_length = wall_length - model.width;

		while (start_length < end_length)
		{
			auto V1 = wall.start_corner.point + direction * start_length;
			auto V2 = wall.start_corner.point + direction * (model.width + start_length);
			auto V3 = V1 + normal * model.length;
			auto V4 = V2 + normal * model.length;
			box.Max = ComMath::Point3fMax(V1, ComMath::Point3fMax(V2, ComMath::Point3fMax(V3, V4)));
			box.Min = ComMath::Point3fMin(V1, ComMath::Point3fMin(V2, ComMath::Point3fMin(V3, V4)));
			box.location.x = (V2.x + V3.x) / 2;
			box.location.y = (V2.y + V3.y) / 2;

			float accept = true;
			// �ж��Ƿ����ų�ͻ
			if (IsCollisionDoor(box))
			{
				accept = false;
			}

			if (!accept)
			{
				start_length += DynanmicLayoutStep;
			}
			else
			{
				BoundingBoxSet single_layout;
				single_layout.insert(box);
				case_layout.push_back(single_layout);
				break;
			}
		}
	}
}
// ����ǽ��ĳ�������
Point3f ToiletAutomaticlDesign::CalculateWallNormal(Wall*wall) 
{
	auto wall_direction = wall->start_corner.point - wall->end_corner.point;

	// ��ת90��
	auto rotate =0.25f*6.283185307f;//2*pi/4
	auto x = cosf(rotate)*wall_direction.x - sinf(rotate)*wall_direction.y;
	wall_direction.y = sinf(rotate)*wall_direction.x + cosf(rotate)*wall_direction.y;
	wall_direction.x = x;
	
	wall_direction = ComMath::Normalize(wall_direction);
	auto extend = (wall->start_corner.point + wall->end_corner.point)*0.5f+ wall_direction*5.f;
	if (ComMath::PointInPolygon(extend, m_single_case->single_room.getPointList()))
		return wall_direction;
	return -wall_direction;
}

// �Ե����ݰ��վ������� 0 ���� 1 ����
vector<CornerData> ToiletAutomaticlDesign::SortCornerData(vector<CornerData> src_data,int sort_type)
{
	int count = static_cast<int>(src_data.size());
	for (int i = 0; i < count; i++)
	{
		for (int j = i+1; j < count; j++)
		{
			if (sort_type == 0)
			{
				if (src_data[i].distance < src_data[j].distance)
				{
					CornerData tmp = src_data[i];
					src_data[i] = src_data[j];
					src_data[j] = tmp;
				}
			}
			else
			{
				if (src_data[i].distance > src_data[j].distance)
				{
					CornerData tmp = src_data[i];
					src_data[i] = src_data[j];
					src_data[j] = tmp;
				}
			}
			
		}
	}

	return src_data;
}

// ���������Ƿ���ײ
bool ToiletAutomaticlDesign::IsCollisionBoundingBox(BoundingBox src_box, BoundingBox des_box)
{
	//return ComMath::CollisionDetection(src_box.Min, src_box.Max, des_box.Min, des_box.Max);
	if (src_box.flag == false || des_box.flag == false)
	{
		return false;
	}
	if ((src_box.Min.x >= des_box.Max.x || src_box.Min.y >= des_box.Max.y) || (des_box.Min.x >= src_box.Max.x || des_box.Min.y >= src_box.Max.y))
		return false;
	return true;
}

// ��Ⲽ���Ƿ����Ž�������ײ
bool ToiletAutomaticlDesign::IsCollisionDoor(BoundingBox box)
{
	for (auto &door : door_layout)
	{
		if (IsCollisionBoundingBox(box, door))
		{
			return true;
		}
	}

	return false;
}
// �õ�һ������Ĳ��ַ���,Ϊ����װʹ��
BoundingBox ToiletAutomaticlDesign::GetVirtualBox(int inner_type)
{
	BoundingBox box;
	box.Inner_type = inner_type;
	box.flag = false;

	return box;
}
// ����һ���µĲ���
set<BoundingBox> ToiletAutomaticlDesign::CopyBoundingBoxSet(BoundingBoxSet src_set)
{
	set<BoundingBox> result;
	for (auto &model : src_set)
	{
		result.insert(model);
	}

	return result;
}
