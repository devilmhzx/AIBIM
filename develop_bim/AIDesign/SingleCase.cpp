#include "SingleCase.h"
#include "Log/easylogging++.h"


SingleCase::SingleCase()
{
}


SingleCase::~SingleCase()
{
}

void SingleCase::Init(SingleCase &single)
{
	
}

// 获得与墙角相关的墙体
vector<Wall*> SingleCase::GetWallsByCorner(Corner *corner)
{
	vector<Wall*> result;
	for (int i = 0; i < wall_list.size(); i++)
	{
		Wall *p_wall = &wall_list[i];
		if (p_wall->start_corner.GetNo() == corner->GetNo() || p_wall->end_corner.GetNo() == corner->GetNo())
		{
			result.push_back(p_wall);
		}
	}

	return result;
}

// 墙体是否与门平行
bool  SingleCase::IsWallFaceToDoor(Wall *wall)
{
	for (auto &door : door_list)
	{
		float angle = ComMath::GetAngleBetweenTwoLines(wall->end_corner.point- wall->start_corner.point, door.end_point-door.start_point) *180/M_PI;
		if ((angle >= 0 && angle <= 15) || (angle >= 165 && angle <= 180))
		{
			return true;
		}
	}

	return false;
}

// 区域转化为房间
SingleCase SingleCase::GetNewSingleCaseByRegion(RoomRegion region)
{
	SingleCase new_singlecase = SingleCase();

	// 处理入户门
	new_singlecase.entrance_door = this->entrance_door;
	// 处理房间
	Room newRoom = Room(region);
	new_singlecase.single_room = newRoom;

	// 生成corner点
	for (vector<Point3f>::iterator point = newRoom.point_list.begin(); point != newRoom.point_list.end(); point++)
	{
		Corner temp_corner;
		temp_corner.point = *point;
		temp_corner.SetNo(ComUtil::getGuuidNo());
		temp_corner.room_no = newRoom.GetNo();
		new_singlecase.corner_list.push_back(temp_corner);
	}

	// 生成虚拟墙体
	for (int i = 0; i < region.virtual_line_list.size(); i++)
	{
		Line tmpLine = region.virtual_line_list[i];
		Wall tmp_wall = Wall(&new_singlecase.GetCornerByPoint(tmpLine.start), &new_singlecase.GetCornerByPoint(tmpLine.end));
		tmp_wall.SetNo(tmpLine.no);
		tmp_wall.room_no = newRoom.GetNo();
		tmp_wall.is_virtual = true;
		new_singlecase.wall_list.push_back(tmp_wall);
	}

	// 生成真实墙体
	for (int i = 0; i < region.wall_line_list.size(); i++)
	{
		Line tmpLine = region.wall_line_list[i];
		Wall tmp_wall = Wall(&new_singlecase.GetCornerByPoint(tmpLine.start), &new_singlecase.GetCornerByPoint(tmpLine.end));
		tmp_wall.SetNo(tmpLine.no);
		tmp_wall.room_no = newRoom.GetNo();
		tmp_wall.is_virtual = false;
		new_singlecase.wall_list.push_back(tmp_wall);
	}

	// 处理门和窗相关数据
	for (int i = 0; i < new_singlecase.wall_list.size(); i++)
	{
		Wall &srcWall = new_singlecase.wall_list[i];
		// 虚墙不处理
		if (srcWall.is_virtual)
		{
			continue;
		}

		Wall tmpWall = GetWallByWall(srcWall);
		if (tmpWall.GetNo() == "")
		{
			continue;
		}

		// 获得此墙体上面的门
		for (int j = 0; j < tmpWall.door_list.size(); j++)
		{
			Door newDoor = tmpWall.door_list[j];
			if (ComMath::IsCoincideOfTwoLine(newDoor.start_point, newDoor.end_point, srcWall.start_corner.point, srcWall.end_corner.point))
			{
				srcWall.door_list.push_back(newDoor);
				new_singlecase.door_list.push_back(newDoor);
			}
		}

		// 获得此墙体上面的窗
		for (int j = 0; j < tmpWall.window_list.size(); j++)
		{
			Window newWindow = tmpWall.window_list[j];
			if (ComMath::IsCoincideOfTwoLine(newWindow.start_point, newWindow.end_point, srcWall.start_corner.point, srcWall.end_corner.point))
			{
				srcWall.window_list.push_back(newWindow);
				new_singlecase.window_list.push_back(newWindow);
			}
		}

		// 待布局的模型列表
		new_singlecase.model_list = this->model_list;

	}
	return new_singlecase;
}

// 是否是区域布局
bool SingleCase::IsRegionLayout()
{
	int not_virtual_count = 0;
	if(region_list.size() > 0)
	{
		for (auto tmpRegion : region_list)
		{
			if (!tmpRegion.getIsVirtual())
			{
				not_virtual_count++;
			}
		}

		if (not_virtual_count == 1)
		{
			return true;
		}
	}

	return false;
}
// 通过墙体获得有关的两个墙体
vector<Wall*> SingleCase::GetRelatedWallListByWall(Wall *p_wall)
{
	vector<Wall*> result;
	Point3f start_point = p_wall->start_corner.point;
	Point3f end_point = p_wall->end_corner.point;

	for (int i = 0; i < wall_list.size(); i++)
	{

		Wall *tmp_wall = &wall_list[i];
		if (tmp_wall->GetNo() == p_wall->GetNo())
		{
			continue;
		}

		if (tmp_wall->end_corner.GetNo() == p_wall->start_corner.GetNo() || tmp_wall->end_corner.GetNo() == p_wall->end_corner.GetNo())
		{
			result.push_back(tmp_wall);
			continue;
		}

		if (tmp_wall->start_corner.GetNo() == p_wall->start_corner.GetNo() || tmp_wall->start_corner.GetNo() == p_wall->end_corner.GetNo())
		{
			result.push_back(tmp_wall);
			continue;
		}

	}

	return result;
}

// 根据空间类型获得对应的region
RoomRegion SingleCase::GetRegionById(int region_id)
{
	for (int i = 0; i < region_list.size(); i++)
	{
		RoomRegion tmpRegion = region_list[i];
		if (tmpRegion.getRegionId() == region_id)
		{
			return tmpRegion;
		}
	}

	return RoomRegion();
}
// 模拟生成卫生间马桶点位
void SingleCase::CreateFloorboxOfStool()
{
	// 判断是否有马桶点位
	for (int i = 0; i < floor_box_list.size(); i++)
	{
		FloorBox floor_box = floor_box_list[i];
		if (floor_box.floor_box_type == FB_STOOL)
		{
			return;
		}
	}
	
	// 只考虑有一门的房间
	if (door_list.size()  !=1)
	{
		return;
	}

	LOG(INFO) << "Begin create location of stool";
	// 获得门所在的墙体
	Wall *p_wall_list = NULL;
	for (int i = 0; i < wall_list.size(); i++)
	{
		Wall *p_wall = &wall_list[i];
		if (p_wall->door_list.size() > 0)
		{
			p_wall_list = &wall_list[i];
			break;
		}
	}
	if (p_wall_list == NULL)
	{
		return;
	}
	// 获得与门有关系的墙体，最多2个
	Wall * layout_wall = NULL;
	vector<Wall*> tmp_list = GetRelatedWallListByWall(p_wall_list);
	// 关联墙体只有一个
	if (tmp_list.size()  ==1)
	{
		if (tmp_list[0]->door_list.size() > 0 || tmp_list[0]->window_list.size() > 0)
		{
			return;
		}

		if (ComMath::getTwoPointDistance(tmp_list[0]->start_corner.point, tmp_list[0]->end_corner.point) < 100)
		{
			return;
		}
		layout_wall = tmp_list[0];
	}
	else if (tmp_list.size() == 2)
	{
		Wall* p_tmp_wall0 = tmp_list[0];
		Wall* p_tmp_wall1 = tmp_list[1];
		bool flag0 = true;
		bool flag1 = true;

		// 判断0的有效性
		if (p_tmp_wall0->door_list.size() > 0 || p_tmp_wall0->window_list.size() > 0)
		{
			flag0 = false;
		}
		else
		{
			if (ComMath::getTwoPointDistance(p_tmp_wall0->start_corner.point, p_tmp_wall0->end_corner.point) < 100)
			{
				flag0 = false;
			}
		}

		// 判断1的有效性
		if (p_tmp_wall1->door_list.size() > 0 || p_tmp_wall1->window_list.size() > 0)
		{
			flag1 = false;
		}
		else
		{
			if (ComMath::getTwoPointDistance(p_tmp_wall1->start_corner.point, p_tmp_wall1->end_corner.point) < 100)
			{
				flag1 = false;
			}
		}

		if (flag1 == false && flag0 == false)
		{
			return;
		}

		if (flag0 == false)
		{
			layout_wall = p_tmp_wall1;
		}
		if (flag1 == false)
		{
			layout_wall = p_tmp_wall0;
		}

		if (flag1&&flag0)
		{
			// 离门远的墙体
			Point3f center0 = (p_tmp_wall0->start_corner.point + p_tmp_wall0->end_corner.point)*0.5;
			Point3f center1 = (p_tmp_wall1->start_corner.point + p_tmp_wall1->end_corner.point)*0.5;
			Point3f door_center = door_list[0].pos;
			if (ComMath::getTwoPointDistance(center0, door_center) > ComMath::getTwoPointDistance(center1, door_center))
			{
				layout_wall = p_tmp_wall0;
			}
			else
			{
				layout_wall = p_tmp_wall1;
			}
		}

	}
	else
	{
		return;
	}

	// 获得远离门的点
	Point3f door_center = door_list[0].pos;
	Point3f fast_point = layout_wall->start_corner.point;
	Point3f near_point = layout_wall->end_corner.point;
	if (ComMath::getTwoPointDistance(fast_point, door_center) < ComMath::getTwoPointDistance(near_point, door_center))
	{
		fast_point = layout_wall->end_corner.point;
		near_point = layout_wall->start_corner.point;
	}

	Point3f near_fast = ComMath::Normalize(fast_point - near_point);
	Point3f tmp_point = near_point + near_fast * 110;
	Point3f normal_point = CalculateWallNormal(layout_wall);
	Point3f floox_pos = tmp_point + normal_point * 40;
	if (ComMath::PointInPolygon(floox_pos, single_room.getPointList()))
	{
		FloorBox stool;
		stool.width = 11;
		stool.height = 11;
		stool.length = 11;
		stool.center = floox_pos;
		stool.floor_box_type = FB_STOOL;
		floor_box_list.push_back(stool);
		LOG(INFO) << "Create location of stool success.";
		return;
	}
}

// 创建模型的组合
void SingleCase::CreateCombinedModel()
{
	// 对模型进行包的组合
	this->package_list = CombinedGeneration::GetCacheModelListBySingleCase(this);
	for (PackageStruct &package : this->package_list)
	{
		vector<Model> tmp_model_list;
		// 移除在组合内的模型
		for (Model tmp_model : this->model_list)
		{
			bool is_detete = false;
			for (Model package_model : package.mode_list)
			{
				if (tmp_model.GetNo() == package_model.GetNo())
				{
					is_detete = true;
					break;
				}
			}
			if (!is_detete)
			{
				tmp_model_list.push_back(tmp_model);
			}
		}
		// 插入代表包的虚拟模型
		tmp_model_list.push_back(package.virtual_model.model);
		this->model_list = tmp_model_list;
		tmp_model_list.clear();
	}
}

// 解析模型的组合（布局后）
void SingleCase::AnalyzeCombinedModel()
{
	// 不存在组合
	if (this->package_list.size() < 1)
	{
		return;
	}

	// 遍历所有的布局
	for (auto&layout : this->layout_list)
	{
		vector<Model> result_models;
		for (Model tmp_model : layout.model_list)
		{
			bool is_virtual_model = false;

			// 当前布局是否存在组合
			for (PackageStruct &package : this->package_list)
			{
				if (tmp_model.id == package.virtual_model.model.id)
				{
					// 进行包的解析
					package.virtual_model.model = tmp_model;
					CombinedGeneration::GetModelGroupByVirtualModel(package.virtual_model, package.mode_list);
					for (Model tmp_analyze : package.mode_list)
					{
						// 把解析后的模型插入结果数组
						result_models.push_back(tmp_analyze);
					}
					// 此模型为代表组合的虚拟模型
					is_virtual_model = true;
					break;
				}
			}
			// 如果当前模型不是虚拟模型
			if (!is_virtual_model)
			{
				result_models.push_back(tmp_model);
			}
		}
		layout.model_list = result_models;
	}
}
// 计算墙体的朝向向量
Point3f SingleCase::CalculateWallNormal(Wall*wall)
{
	auto wall_direction = wall->start_corner.point - wall->end_corner.point;

	// 旋转90度
	auto rotate = 0.25f*6.283185307f;//2*pi/4
	auto x = cosf(rotate)*wall_direction.x - sinf(rotate)*wall_direction.y;
	wall_direction.y = sinf(rotate)*wall_direction.x + cosf(rotate)*wall_direction.y;
	wall_direction.x = x;

	wall_direction = ComMath::Normalize(wall_direction);
	auto extend = (wall->start_corner.point + wall->end_corner.point)*0.5f + wall_direction * 5.f;
	if (ComMath::PointInPolygon(extend, single_room.getPointList()))
		return wall_direction;
	return -wall_direction;
}

// 通过坐标点获得corner
Corner SingleCase::GetCornerByPoint(Point3f point)
{
	for (int i = 0; i < this->corner_list.size(); i++)
	{
		Corner tmpCorner = this->corner_list[i];
		if (ComMath::comTwoPoint3f(tmpCorner.point, point))
		{
			return tmpCorner;
		}
	}

	return Corner();
}

// 通过墙获得墙,仅供区域转换使用
Wall SingleCase::GetWallByWall(Wall srcWall)
{
	for (int i = 0; i < this->wall_list.size(); i++)
	{
		Wall tmpWall = this->wall_list[i];
		if (ComMath::IsCoincideOfTwoLine(tmpWall.start_corner.point, tmpWall.end_corner.point, srcWall.start_corner.point, srcWall.end_corner.point))
		{
			return tmpWall;
		}
	}
	
	return Wall() ;
}

// 获得布局的区域
RoomRegion SingleCase::GetLayoutRegion()
{
	
	for (auto &tmpRegion : region_list)
	{
		if (!tmpRegion.getIsVirtual())
		{
			return tmpRegion;
		}
	}

	return RoomRegion();
}
// 寻找重合的墙体
//vector<Wall> SingleCase::GetCoincidenceWalls(Point3f start_point, Point3f end_point)
//{
//	for (Wall wall : this->wall_list)
//	{
//		;
//	}
//}