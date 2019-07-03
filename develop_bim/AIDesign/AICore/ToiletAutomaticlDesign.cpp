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
	// 布局门
	InitLayoutDoor();
	// 布局马桶
	LayoutCommode();
	// 布局花洒
	LayoutSprinkler();
	//布局淋浴房
	LayoutNormalBathroom();
	//布局一字浴房
	//LayoutBathroomWithLineShape();
	//布局浴柜
	LayoutCabinet();
	//布局浴缸
	LayoutBathtub();
	//扩展布局
	ExpandLayout();
	// 生成含浴缸的布局
	ExpandBathtubLayout();
	//动态布局洗衣机
	LayoutWasher(case_layout);
	if (!case_layout.size())
		return false;

	// 生成最终的布局结构
	FillModelToSingleCase();
	return true;
}

// 初始化门的布局
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
	// 判断是否具有待布局的马桶
	auto model = FindLayoutModel(GROUP_COMMODE);
	if (model.id == Model_Inner_Type)
		return;
	//找坑位
	FloorBox*stool = nullptr;
	for (auto &floor_box : m_single_case->floor_box_list)
	{
		if (floor_box.floor_box_type == FB_STOOL) {
			stool = &floor_box;
		}
	}
	if (!stool)
		return;

	//找最近的墙体
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
	//具有小于40的墙体
	if (wall_to_stool_dist.size())
	{
		for (auto &wall : wall_to_stool_dist)
		{
			//墙体长度必须大于马桶的宽度,才能放下
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
 
// 获得距离门距离的墙角
vector<CornerData> ToiletAutomaticlDesign::GetFastCornerDataWithDoor()
{
	// 寻找有效墙墙角
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

	// 排序墙角距离
	corner_data = SortCornerData(corner_data,0);

	return corner_data;
}

// 获得距离门较近的墙角
vector<CornerData> ToiletAutomaticlDesign::GetNearCornerDataWithDoor()
{
	// 寻找有效墙墙角
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

	// 排序墙角距离
	corner_data = SortCornerData(corner_data,1);

	return corner_data;
}

// 布局花洒
void ToiletAutomaticlDesign::LayoutSprinkler() 
{
	auto model = FindLayoutModel(GROUP_SPRINKLER);
	if (model.id == Model_Inner_Type)
		return;
	// 获得有序的墙角
	vector<CornerData> corner_data = GetFastCornerDataWithDoor();
	if (corner_data.size() < 2)
	{
		return;
	}

	// 选择两个墙角布局花洒
	//找拥有的角落并且墙体上面没有窗户，并且墙体不对门
	for (int i = 0; i < 2; i++)
	{
		vector<Wall*> relative_walls = m_single_case->GetWallsByCorner(&corner_data[i].corner);
		for (int j = 0; j < relative_walls.size(); j++)
		{
			Wall *p_wall = relative_walls[j];

			// 墙体长度小于花洒长度
			if (ComMath::getTwoPointDistance(p_wall->start_corner.point, p_wall->end_corner.point) < model.width)
			{
				continue;
			}
			// 墙体上面有门窗
			if (p_wall->door_list.size() > 0 || p_wall->window_list.size() > 0)
			{
				continue;
			}

			// 墙体面对门或窗户
			/*if (m_single_case->IsWallFaceToDoor(p_wall))
			{
				continue;
			}*/

			// 进行布局
			auto bounding_box = CalculateSprinklerByWall(&corner_data[i].corner, model, p_wall);
			sprinkler_layout.push_back(bounding_box);
		}
	}
}


// 布局淋浴房
void ToiletAutomaticlDesign::LayoutNormalBathroom() 
{
	//获得模型
	auto model = FindLayoutModel(GROUP_BATH_ROOM);
	if (model.id == Model_Inner_Type)
		return;

	//卫生间面积小与四平米不考虑
	auto area = ComMath::GetPolygonArea(m_single_case->single_room.point_list);
	if (area < Toilet_Area)
		return;

	// 获得有序的墙角
	vector<CornerData> corner_data = GetFastCornerDataWithDoor();
	if (corner_data.size() < 2)
	{
		return;
	}

	// 选择两个墙角布局淋浴房
	//找拥有的角落并且墙体上面没有窗户，并且墙体不对门
	for (int i = 0; i < 2; i++)
	{
		vector<Wall*> relative_walls = m_single_case->GetWallsByCorner(&corner_data[i].corner);
		for (int j = 0; j < relative_walls.size(); j++)
		{
			Wall *p_wall = relative_walls[j];

			// 墙体长度小于淋浴房长度
			if (ComMath::getTwoPointDistance(p_wall->start_corner.point, p_wall->end_corner.point) < model.width)
			{
				continue;
			}
			// 墙体上面有门窗
			if (p_wall->door_list.size() > 0 || p_wall->window_list.size() > 0)
			{
				continue;
			}

			// 墙体面对门或窗户
			/*if (m_single_case->IsWallFaceToDoor(p_wall))
			{
				continue;
			}*/

			// 进行布局
			auto bounding_box = CalculateNormalBathroomByWall(&corner_data[i].corner, model, p_wall);
			bathroom_layout.push_back(bounding_box);
		}
	}
}

// 布局一字型淋浴房
void ToiletAutomaticlDesign::LayoutBathroomWithLineShape() 
{
	//无布置要求
	auto model = FindLayoutModel(GROUP_SPRINKLER);
	if (model.id == Model_Inner_Type)
		return;

	//卫生间面积小与四平米不考虑
	auto area = ComMath::GetPolygonArea(m_single_case->single_room.point_list);
	if (area < Toilet_Area)
		return;

	// 获得有序的墙角
	vector<CornerData> corner_data = GetFastCornerDataWithDoor();
	if (corner_data.size() < 2)
	{
		return;
	}

	// 选择两个墙角布局淋浴房
	for (int i = 0; i < 2; i++)
	{
		vector<Wall*> relative_walls = m_single_case->GetWallsByCorner(&corner_data[i].corner);
		for (int j = 0; j < relative_walls.size(); j++)
		{
			Wall *p_wall = relative_walls[j];

			// 墙体长度小于淋浴房长度
			if (ComMath::getTwoPointDistance(p_wall->start_corner.point, p_wall->end_corner.point) < model.width)
			{
				continue;
			}

			// 进行布局
			auto bounding_box = CalculateBathroomWithLineShape(&corner_data[i].corner, model, p_wall);
			bathroom_layout.push_back(bounding_box);
		}
	}

	// 浴缸检测与门的碰撞问题
	vector<BoundingBox>::iterator it = sprinkler_layout.begin();
	for (; it != sprinkler_layout.end();)
	{
		if (IsCollisionDoor(*it))
		{
			it = sprinkler_layout.erase(it);
		}
		else
			//迭代器指向下一个元素位置
			++it;
	}
}

// 布局浴室柜
void ToiletAutomaticlDesign::LayoutCabinet() 
{
	auto model = FindLayoutModel(GROUP_CABINET_BATH);
	if (model.id == Model_Inner_Type)
		return;

	// 离门较近的墙角来布局浴室柜
	// 获得有序的墙角
	vector<CornerData> corner_data = GetNearCornerDataWithDoor();
	if (corner_data.size() < 2)
	{
		return;
	}

	// 选择两个墙角布局浴室柜
	for (int i = 0; i < corner_data.size()-2; i++)
	{
		Corner *current_corner = &corner_data[i].corner;
		vector<Wall*> relative_walls = m_single_case->GetWallsByCorner(current_corner);
		for (int j = 0; j < relative_walls.size(); j++)
		{
			Wall *p_wall = relative_walls[j];

			// 墙体长度小于浴室柜长度
			if (ComMath::getTwoPointDistance(p_wall->start_corner.point, p_wall->end_corner.point) < model.width)
			{
				continue;
			}

			// 如果墙体上面有门并且有窗忽略这个布局
			if (p_wall->door_list.size() > 0 && p_wall->window_list.size() > 0)
			{
				continue;
			}
			// 如果墙体上面有窗户
			if (p_wall->window_list.size() > 0)
			{
				// 窗户个数大于1，也不处理
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

				// 布局浴室柜
				auto bounding_box = CalculateCabinetByCorner(current_corner->point, end_point, model, p_wall);
				if (bounding_box.flag)
				{
					cabinet_layout.push_back(bounding_box);
				}
				continue;
			}

			// 如果墙面上有门并且门的个数为1个进行处理
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

				// 按照点位进行布局浴室柜
				auto bounding_box = CalculateCabinetByCorner(current_corner->point, end_point, model, p_wall);
				if (bounding_box.flag)
				{
					cabinet_layout.push_back(bounding_box);
				}

				// 按照开始点进行布局浴室柜
				auto bounding_box2 = CalculateCabinetByPoint(current_corner->point, end_point, model, p_wall);
				if (bounding_box2.flag)
				{
					cabinet_layout.push_back(bounding_box2);
				}
				continue;
			}

			// 墙上什么都没有进行布局
			auto end_point = p_wall->start_corner.point;
			if (ComMath::getTwoPointDistance(current_corner->point, p_wall->start_corner.point) < ComMath::getTwoPointDistance(current_corner->point, p_wall->end_corner.point))
			{
				end_point = p_wall->end_corner.point;
			}

			// 布局浴室柜
			auto bounding_box = CalculateCabinetByCorner(current_corner->point, end_point, model, p_wall);
			if (bounding_box.flag)
			{
				cabinet_layout.push_back(bounding_box);
			}
			continue;
		}
	}

	// 浴室柜检测与门的碰撞问题
	vector<BoundingBox>::iterator it = cabinet_layout.begin();
	for (; it != cabinet_layout.end();)
	{
		if (IsCollisionDoor(*it))
		{
			it = cabinet_layout.erase(it);
		}
		else
			//迭代器指向下一个元素位置
			++it;
	}
}

// 扩展布局进行有效布局的组合
void ToiletAutomaticlDesign::ExpandLayout() 
{
	int model_count = 0;

	// 花洒与淋浴房的处理
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
	// 模拟一套假的布局方案放入各个布局中
	// 马桶虚拟数据
	commode_layout.push_back(GetVirtualBox(GROUP_COMMODE));
	// 花洒虚拟数据
	if (is_sprinkler)
	{
		sprinkler_layout.push_back(GetVirtualBox(GROUP_SPRINKLER));
	}
	else
	{
		sprinkler_layout.push_back(GetVirtualBox(GROUP_BATH_ROOM));
	}
	
	// 浴室柜虚拟数据
	cabinet_layout.push_back(GetVirtualBox(GROUP_CABINET_BATH));
	// 浴缸虚拟数据
	//bathtub_layout.push_back(GetVirtualBox(GROUP_BATHTUB));

	// 马桶
	for (auto &commode : commode_layout)
	// 进行布局的组合
	{
		// 花洒
		for (auto &sprinkler : sprinkler_layout)
		{
			// 马桶与花洒碰撞
			if (IsCollisionBoundingBox(commode, sprinkler))
			{
				continue;
			}
			// 浴室柜
			for (auto &cabinet : cabinet_layout)
			{
				// 马桶与浴室柜碰撞
				if (IsCollisionBoundingBox(commode, cabinet))
				{
					continue;
				}
				// 花洒与浴室柜碰撞
				if (IsCollisionBoundingBox(sprinkler, cabinet))
				{
					continue;
				}

				// 生成一套合理布局
				std::set<BoundingBox> temp_set;
				temp_set.insert(commode);
				temp_set.insert(sprinkler);
				temp_set.insert(cabinet);
				tmp_design_layout.push_back(temp_set);
			}
		}
	}

	// 去掉方案中包含虚拟方案的方案
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

// 生成含浴缸的布局
void ToiletAutomaticlDesign::ExpandBathtubLayout()
{
	if (bathtub_layout.size() < 1)
	{
		return;
	}
	if (case_layout.size() > 0)
	{
		ToiletDesign tmp_design_layout;
		// 浴缸处理
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

// 判定浴缸在布局中是否合适
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
// 布局浴缸
void ToiletAutomaticlDesign::LayoutBathtub() 
{
	auto model = FindLayoutModel(GROUP_BATHTUB);
	if (model.id == Model_Inner_Type)
		return;


	// 获得有序的墙角
	vector<CornerData> corner_data = GetFastCornerDataWithDoor();
	if (corner_data.size() < 2)
	{
		return;
	}

	// 选择两个墙角布局浴缸
	for (int i = 0; i < 2; i++)
	{
		vector<Wall*> relative_walls = m_single_case->GetWallsByCorner(&corner_data[i].corner);
		for (int j = 0; j < relative_walls.size(); j++)
		{
			Wall *p_wall = relative_walls[j];

			// 墙体长度小于浴缸长度
			if (ComMath::getTwoPointDistance(p_wall->start_corner.point, p_wall->end_corner.point) < model.width)
			{
				continue;
			}
			// 进行布局
			auto bounding_box = CalculateBathtubByWall(&corner_data[i].corner, model, p_wall);
			bathtub_layout.push_back(bounding_box);
		}
	}

	// 浴缸检测与门的碰撞问题
	vector<BoundingBox>::iterator it = bathtub_layout.begin();
	for (; it != bathtub_layout.end();)
	{
		if (IsCollisionDoor(*it))
		{
			it = bathtub_layout.erase(it);
		}
		else
			//迭代器指向下一个元素位置
			++it;
	}

}

// 动态布局洗衣机
void ToiletAutomaticlDesign::LayoutWasher(ToiletDesign &ref)
{
	auto model = FindLayoutModel(GROUP_WASHER);
	if (model.id == Model_Inner_Type)
		return;

	// 没有布局，单独布局洗衣机
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
 
//沿着墙体自动布局洗衣机
bool ToiletAutomaticlDesign::DynamicLayoutWasher(BoundingBoxSet&single_layout, Wall*wall, Model model) 
{
	auto res = false;
	if (ComMath::getTwoPointDistance(wall->end_corner.point, wall->start_corner.point) < model.width)
		return res;
	auto normal = CalculateWallNormal(wall);
	auto direction = ComMath::Normalize(wall->end_corner.point-wall->start_corner.point);
	//计算初始BoundingBox
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
		// 判断是否与门冲突
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
 
//沿着角落自动布局洗衣机
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

		//计算初始BoundingBox
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
		// 判断是否与门冲突
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
// 生成结果
void ToiletAutomaticlDesign::FillModelToSingleCase() 
{
	for (auto per_case : case_layout)
	{
		ModelLayout layout;
		FillModelToModelLayout(layout,per_case);
		m_single_case->layout_list.push_back(layout);
	}
}

// 序列化每一套布局
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
		//修正花洒的模型的高度
		//@冯小翼
		if (boundbox.model.id == GROUP_SPRINKLER)
		{
			boundbox.location.z += 100.f;
		}

		boundbox.model.location = boundbox.location;
		layout.model_list.push_back(boundbox.model);
	}
}

// 寻找模型
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

// 布局门
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

// 布局马桶的位置
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

// 布局花洒
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

// 布局淋浴房
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

// 布局一字型淋浴房
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

// 布局浴室柜
//V1---------V2
//V3---------V4
BoundingBox ToiletAutomaticlDesign::CalculateCabinetByCorner(Point3f beg, Point3f end, Model model, Wall*wall) 
{
	BoundingBox box;
	// 判断是否适合布局
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

// 布局浴室柜
//V1---------V2
//V3---------V4
BoundingBox ToiletAutomaticlDesign::CalculateCabinetByPoint(Point3f beg, Point3f end, Model model, Wall*wall)
{
	BoundingBox box;
	// 判断是否适合布局
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

// 通过墙体的点计算浴缸的box
// 注意浴缸的尺寸长的为width
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

//计算布局洗衣机
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
		//计算初始BoundingBox
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
			// 判断是否与门冲突
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
// 计算墙体的朝向向量
Point3f ToiletAutomaticlDesign::CalculateWallNormal(Wall*wall) 
{
	auto wall_direction = wall->start_corner.point - wall->end_corner.point;

	// 旋转90度
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

// 对点数据按照距离排序 0 降序 1 升序
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

// 两个布局是否碰撞
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

// 检测布局是否与门进行了碰撞
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
// 得到一套虚拟的布局方案,为了组装使用
BoundingBox ToiletAutomaticlDesign::GetVirtualBox(int inner_type)
{
	BoundingBox box;
	box.Inner_type = inner_type;
	box.flag = false;

	return box;
}
// 复制一套新的布局
set<BoundingBox> ToiletAutomaticlDesign::CopyBoundingBoxSet(BoundingBoxSet src_set)
{
	set<BoundingBox> result;
	for (auto &model : src_set)
	{
		result.insert(model);
	}

	return result;
}
