#include "MetisGeomancy.h"
#include "../Log/easylogging++.h"
#define   geomance_tolerance 5.f
MetisGeomancy::MetisGeomancy(const shared_ptr<AICase>& ai_case)
	:ai_case(ai_case)
{
	// 寻找有效门
	PickEffectiveDoors();
	// 把有效门进行分类
	InitDoors();
}

// 获得风水测评相关的数据
Json::Value MetisGeomancy::GetGeomancyJson()
{
	Json::Value geomantic;
	Json::Value reduceScore;
	Json::Value scoreList = GetGeomancyScoreList();
	if (scoreList.size() > 0)
	{
		reduceScore["scoreList"] = scoreList;
	}
	else
	{
		reduceScore["scoreList"].resize(0);
	}
	
	geomantic["version"] = 10001;
	geomantic["reduceScore"] = reduceScore;

	return geomantic;
}
// 寻找有效的门
void MetisGeomancy::PickEffectiveDoors()
{
	// 遍历房间
	for (auto& single_case : ai_case->single_case_list)
	{
		// 遍历房间中的门
		for (auto& door : single_case.door_list)
		{
			DoorData temp_door_data;
			temp_door_data.room_no = single_case.single_room.GetNo();
			temp_door_data.door = door;
			temp_door_data.room_door_type = GetRoomDoorType(single_case.single_room.getSpaceId(), door.door_type);
			// 入户门没有关联门
			if (temp_door_data.room_door_type == RUHUMEN)
			{
				temp_door_data.link_room_no = "";
			}
			else
			{
				vector<string>room_nos = ai_case->GetRoomsByDoorNo(door.GetNo());
				if (room_nos.size() != 2)
				{
					continue;
				}
				string no1 = room_nos[0];
				string no2 = room_nos[1];
				if (no1 == temp_door_data.room_no)
				{
					temp_door_data.link_room_no = no2;
				}
				else
				{
					temp_door_data.link_room_no = no1;
				}
			}

			effective_doors.push_back(temp_door_data);
		}
	}
}

//判断两个门的是否相交
bool MetisGeomancy::IsDoorIntersect(DoorData src_door, DoorData des_door)
{
	// 计算门到X轴投影的宽度
	float src_x_width = fabs(src_door.door.start_point.x - src_door.door.end_point.x);
	float des_x_width = fabs(src_door.door.start_point.x - src_door.door.end_point.x);

	// X轴方向进行判定
	if (src_x_width >= src_door.door.width / 2 && des_x_width >= des_door.door.width / 2)
	{
		float x0 = src_door.door.start_point.x;
		float x1 = src_door.door.end_point.x;
		float x2 = des_door.door.start_point.x;
		float x3 = des_door.door.end_point.x;
		if (x0 >= x2 && x1 >= x2&&x0 >= x3&&x1 >= x3)
		{
			return false;
		}

		if (x0 <= x2 && x1 <= x2&&x0 <= x3&&x1 <= x3)
		{
			return false;
		}

		return true;
	}
	else if (src_x_width < src_door.door.width / 2 && des_x_width < des_door.door.width / 2)
	{
		float y0 = src_door.door.start_point.y;
		float y1 = src_door.door.end_point.y;
		float y2 = des_door.door.start_point.y;
		float y3 = des_door.door.end_point.y;
		if (y0 >= y2 && y1 >= y2 && y0 >= y3 && y1 >= y3)
		{
			return false;
		}

		if (y0 <= y2 && y1 <= y2 && y0 <= y3 && y1 <= y3)
		{
			return false;
		}

		return true;
	}

	return false;
}

// 通过房间类型和门类型获得房间门类型
int MetisGeomancy::GetRoomDoorType(int space_id, int door_type)
{
	// 入户门
	if (door_type == D_SECURITY)
	{
		return RUHUMEN;
	}

	// 卧室门
	if (space_id == ZHUWO || space_id == CIWO || space_id == ERTONGFANG || space_id == KEWO)
	{
		return WOSHIMEN;
	}

	// 卫生间门
	if (space_id == WEISHENGJIAN || space_id == ZHUWEISHENGJIAN || space_id == KEWEISHENGJIAN )
	{
		return WEISHENGJIANMEN;
	}

	// 卫生间门
	if (space_id == CHUFANG )
	{
		return CHUFANGMEN;
	}

	return NOMEN;
}

// 初始化门相关数据
void MetisGeomancy::InitDoors()
{
	for (int i = 0; i < effective_doors.size(); i++)
	{
		DoorData tmp = effective_doors[i];
		if (tmp.room_door_type == RUHUMEN)
		{
			entrance_door = tmp;
		}
		else if (tmp.room_door_type == WOSHIMEN)
		{
			bedroom_doors.push_back(tmp);
		}
		else if (tmp.room_door_type == CHUFANGMEN)
		{
			kitchen_doors.push_back(tmp);
		}
		else if (tmp.room_door_type == WEISHENGJIANMEN)
		{
			toilet_doors.push_back(tmp);
		}
	}
}

// 入户门对卫生间门10
GeomancyData MetisGeomancy::EntranceToToilet()
{
	GeomancyData result;
	result.id = 10;
	result.count = 0;
	result.score = 3;
	for (int i = 0; i < toilet_doors.size(); i++)
	{
		DoorData tmp = toilet_doors[i];
		if (tmp.link_room_no == entrance_door.room_no)
		{
			if (IsDoorIntersect(entrance_door, tmp))
			{
				result.count++;
			}
		}
	}

	return result;
}
// 入户门对厨房门11
GeomancyData MetisGeomancy::EntranceToKitchen()
{
	GeomancyData result;
	result.id = 11;
	result.count = 0;
	result.score = 3;
	for (int i = 0; i < kitchen_doors.size(); i++)
	{
		DoorData tmp = kitchen_doors[i];
		if (tmp.link_room_no == entrance_door.room_no)
		{
			if (IsDoorIntersect(entrance_door, tmp))
			{
				result.count++;
			}
		}
	}

	return result;
}
// 卧室门对卫生间门12
GeomancyData MetisGeomancy::BedroomToToilet()
{
	GeomancyData result;
	result.id = 12;
	result.count = 0;
	result.score = 3;
	for (int i = 0; i < bedroom_doors.size(); i++)
	{
		DoorData tmp_bedroom = bedroom_doors[i];
		for (int j = 0; j < toilet_doors.size(); j++)
		{
			DoorData tmp = toilet_doors[j];
			if (tmp.link_room_no == tmp_bedroom.room_no || tmp.link_room_no == tmp_bedroom.link_room_no)
			{
				if (tmp_bedroom.door.GetNo() == tmp.door.GetNo())
				{
					continue;
				}
				if (IsDoorIntersect(tmp_bedroom, tmp))
				{
					result.count++;
				}
			}
		}
	}

	return result;
}
// 卧室门对厨房门13
GeomancyData MetisGeomancy::BedroomToKitchen()
{
	GeomancyData result;
	result.id = 13;
	result.count = 0;
	result.score = 3;
	for (int i = 0; i < bedroom_doors.size(); i++)
	{
		DoorData tmp_bedroom = bedroom_doors[i];
		for (int j = 0; j < kitchen_doors.size(); j++)
		{
			DoorData tmp = kitchen_doors[j];
			if (tmp.link_room_no == tmp_bedroom.room_no || tmp.link_room_no == tmp_bedroom.link_room_no)
			{
				if (tmp_bedroom.door.GetNo() == tmp.door.GetNo())
				{
					continue;
				}
				if (IsDoorIntersect(tmp_bedroom, tmp))
				{
					result.count++;
				}
			}
		}
	}

	return result;
}
// 卧室门对入户门14
GeomancyData MetisGeomancy::BedroomToEntrance()
{
	GeomancyData result;
	result.id = 14;
	result.count = 0;
	result.score = 3;
	for (int i = 0; i < bedroom_doors.size(); i++)
	{
		DoorData tmp_bedroom = bedroom_doors[i];
		if (entrance_door.link_room_no == tmp_bedroom.room_no)
		{
			if (IsDoorIntersect(tmp_bedroom, entrance_door))
			{
				result.count++;
			}
		}
	}

	return result;
}
// 卫生间门对厨房门15
GeomancyData MetisGeomancy::ToiletToKitchen()
{
	GeomancyData result;
	result.id = 15;
	result.count = 0;
	result.score = 2;
	for (int i = 0; i < toilet_doors.size(); i++)
	{
		DoorData tmp_toilet = toilet_doors[i];
		for (int j = 0; j < kitchen_doors.size(); j++)
		{
			DoorData tmp = kitchen_doors[j];
			if (tmp.link_room_no == tmp_toilet.room_no || tmp.link_room_no == tmp_toilet.link_room_no)
			{
				if (tmp_toilet.door.GetNo() == tmp.door.GetNo())
				{
					continue;
				}
				if (IsDoorIntersect(tmp_toilet, tmp))
				{
					result.count++;
				}
			}
		}
	}

	return result;
}

// 室内尖角16
GeomancyData MetisGeomancy::CheckSharpAngle()
{
	GeomancyData result;
	result.id = 16;
	result.count = 0;
	result.score = 1;

	for (auto& single_case : ai_case->single_case_list)
	{
		for (int i = 0; i < single_case.single_room.point_list.size(); i++)
		{
			int m = (i + 1) % single_case.single_room.point_list.size();
			int n = (i - 1 + static_cast<int>(single_case.single_room.point_list.size()) % single_case.single_room.point_list.size());
			Point3f current = single_case.single_room.point_list[i];
			Point3f next = single_case.single_room.point_list[m];
			Point3f previous = single_case.single_room.point_list[n];

			float angle = ComMath::GetAngleBetweenTwoLines(Point3f(next.x - current.x, next.y - current.y, 0), Point3f(previous.x - current.x, previous.y - current.y, 0));
			if (angle <= M_PI*0.44)
			{
				result.count++;
			}
		}
	}
	return result;
}

// 客厅沙发处出发，垂直方向不应看到房间门，否则有直捣黄龙之意 17
GeomancyData MetisGeomancy::CheckSoftToDoor()
{
	GeomancyData result;
	result.id = 17;
	result.count = 0;
	result.score = 1;
	// 处理客厅沙发-主位沙发（inner_type=111)
	/**1.获取客厅的SingleCase*/
	std::vector<SingleCase*>	case_list;
	for (auto &room : ai_case->single_case_list)
	{
 
		if (room.single_room.getSpaceId() == KETING)
			case_list.push_back(&room);
	}
	/**2.获取沙发判断*/
	for (auto pointer : case_list)
	{
		/**对于可能存在的主沙发*/
		for (auto &model:pointer->model_list)
		{
			if (model.inner_type != 111)
				continue;
			//获取构造沙发的最上,最下端点
			model.rotation.y = static_cast<float>((int)model.rotation.y % 360);
			if (model.rotation.y < 0)
				model.rotation.y += 360;
			Point3f max_point = Point3f(0, 0.5f*model.width, 0);
			Point3f min_point = Point3f(0, -0.5f*model.width, 0);
			auto angle = model.rotation.y / 180.f * M_PI ;
			//对沙发进行旋转
			auto temp = max_point.x*cosf(angle) - sinf(angle)*max_point.y;
			max_point.y = max_point.x*sinf(angle) + cosf(angle)*max_point.y;
			max_point.x = temp;

			temp = min_point.x*cosf(angle) - sinf(angle)*min_point.y;
			min_point.y = min_point.x*sinf(angle) + cosf(angle)*min_point.y;
			min_point.x = temp;
			//转到世界坐标系
			max_point  = max_point+model.location;
			min_point = min_point + model.location;
			auto direction = ComMath::Normalize(max_point - min_point);
			//direction
			for (auto door : pointer->door_list)
			{
		
				if (!ComMath::isParallelTwoLine(door.start_point, door.end_point, max_point, min_point))
					continue;

				auto temp_point = min_point;
				auto total_length = 0.f;
				while (total_length< model.width)
				{
					auto proj=ComMath::getPointToLinePedal(temp_point, door.start_point, door.end_point);
					//冲突 +1 break;
					if (ComMath::isPointOnLine(proj, door.start_point, door.end_point))
					{
						++result.count;
						break;
					}
					total_length += 20.f;
					temp_point = temp_point + direction * 20.f;
				}
			}
		}
	}
	return result;
}
// 餐桌不可只对大门，否则容易被煞气冲散、把财气带走，扣2分；18
GeomancyData MetisGeomancy::CheckTableToDoor()
{
	GeomancyData result;
	result.id = 18;
	result.count = 0;
	result.score = 1;
	// 处理餐桌（inner_type=116)
	std::vector<SingleCase*>	case_list;
	for (auto &room : ai_case->single_case_list)
	{

		if (room.single_room.getSpaceId() == KETING)
			case_list.push_back(&room);
	}
	for (auto &pointer : case_list)
	{
		/**对于可能存在的主沙发*/
		for (auto &model : pointer->model_list)
		{
			if (model.inner_type != 116)
				continue;
			/**确定入户门的方向*/
			Point3f normal = ComMath::getNormalByLineGenerally(entrance_door.door.start_point, entrance_door.door.end_point);
			auto centre = (entrance_door.door.start_point + entrance_door.door.end_point)*0.5f;
			if (!ComMath::PointInPolygon(centre+ normal*20.f, pointer->single_room.getPointList())
				&&!ComMath::PointInPolygon(centre - normal * 20.f, pointer->single_room.getPointList()))
				continue;

			//对于门的起点和终点分别判断 for begin point
		 
			auto model_centre = model.location;
			auto model_radius = sqrtf(model.width*model.width + model.length*model.length) / 2.f;

			auto proj_start = ComMath::getPointToLineDis(model_centre, centre, centre-normal);
 
			if (proj_start < model_radius)
			{
				++result.count;
			}
		}
	}
	return result;
}
// 卧室床头不对着房门；否则阴气较重，不利于主人健康，扣3分；19
GeomancyData MetisGeomancy::CheckBedHeadToDoor()
{
	GeomancyData result;
	result.id = 19;
	result.count = 0;
	result.score = 1;
	// 处理卧室床头（inner_type=100)
	std::vector<SingleCase*>	case_list;
	for (auto &room : ai_case->single_case_list)
	{
		if (room.single_room.getSpaceId() == ZHUWO|| room.single_room.getSpaceId() == CIWO)
			case_list.push_back(&room);
	}
	for (auto pointer : case_list)
	{
		/**对于可能存在的主沙发*/
		for (auto &model : pointer->model_list)
		{
			if (model.inner_type != 100)
				continue;

			//构造床头的四个边,
			std::vector<Point3f> bed_header;
			bed_header.push_back(Point3f(-0.5f*model.length, 0.5f*model.width, 0.f));
			bed_header.push_back(Point3f(0, 0.5f*model.width, 0.f));
			bed_header.push_back(Point3f(0, -0.5f*model.width, 0.f));
			bed_header.push_back(Point3f(-0.5f*model.length, -0.5f*model.width, 0.f));
			//世界坐标系
			auto angle = model.rotation.y / 180.f*M_PI;
			for (auto&point : bed_header)
			{
				auto x = point.x*cosf(angle) - sinf(angle)*point.y;
				point.y = point.x*sinf(angle) + cosf(angle)*point.y;
				point.x = x;
				point = point + model.location;
			}

			for (auto door : pointer->door_list) {
				bool intersect = false;
				if (!intersect)
				{
					for (int i = 0; i < bed_header.size(); ++i)
					{
						int j = (i + 1) % bed_header.size();
						auto beg = bed_header[i];
						auto end = bed_header[j];
						auto length = ComMath::getTwoPointDistance(beg, end);
						auto normal = ComMath::Normalize(end - beg);
						auto temp_length = 0.f;
						auto temp_point = beg;
						while (temp_length < length)
						{
							auto  proj = ComMath::getPointToLinePedal(temp_point, door.start_point, door.end_point);
							if (ComMath::isPointBetweenPoint(proj, door.start_point, door.end_point)) {
								++result.count;
								intersect=true;
								break;
							}
							temp_length += 20.f;
							temp_point = temp_point + normal * 20.f;
						}
					}
				}
			}
		}
 
	}
	return result;
}
// 镜子不能直对着枕头位，否则容易犯煞，扣3分；20
GeomancyData MetisGeomancy::CheckBedHeadToMirror()
{
	GeomancyData result;
	result.id = 20;
	result.count = 0;
	result.score = 1;

	// 处理卧室床头（inner_type=100)
	std::vector<SingleCase*>	case_list;
	for (auto &room : ai_case->single_case_list)
	{
		if (room.single_room.getSpaceId() == ZHUWO || room.single_room.getSpaceId() == CIWO)
			case_list.push_back(&room);
	}
	for (auto &pointer : case_list)
	{
		/**对于可能存在的主沙发*/
		for (auto &model : pointer->model_list)
		{
			if (model.inner_type != 100)
				continue;

			//构造床头的四个边,
			std::vector<Point3f> bed_header;
			bed_header.push_back(Point3f(-0.5f*model.length, 0.5f*model.width, 0.f));
			bed_header.push_back(Point3f(0, 0.5f*model.width, 0.f));
			bed_header.push_back(Point3f(0, -0.5f*model.width, 0.f));
			bed_header.push_back(Point3f(-0.5f*model.length, -0.5f*model.width, 0.f));
			//世界坐标系
			model.rotation.y = static_cast<float>(((int)model.rotation.y) % 360);
			if (model.rotation.y < 0)
				model.rotation.y += 360;

			auto angle =  model.rotation.y/180.f*M_PI;
			for (auto&point : bed_header)
			{
				auto x = point.x*cosf(angle) - sinf(angle)*point.y;
				point.y = point.x*sinf(angle) + cosf(angle)*point.y;
				point.x = x;
				point = point + model.location;
			}
			auto bed_direction = Point3f(0, 0, 0);
			if (ComMath::IsSameScalar(model.rotation.y, 0.f, geomance_tolerance))
				bed_direction = Point3f(1.f, 0.f, 0.f);
			if (ComMath::IsSameScalar(model.rotation.y, 90.f, geomance_tolerance))
				bed_direction = Point3f(0.f, 1.f, 0.f);
			if (ComMath::IsSameScalar(model.rotation.y, 180.f, geomance_tolerance))
				bed_direction = Point3f(-1.f, 0.f, 0.f);
			if (ComMath::IsSameScalar(model.rotation.y, 270.f, geomance_tolerance))
				bed_direction = Point3f(0.f, -1.f, 0.f);
	 

			for (auto &mirror : pointer->model_list)
			{
				if (mirror.inner_type != 108)
					continue;
				mirror.rotation.y = static_cast<float>(((int)mirror.rotation.y) % 360);
				if (model.rotation.y < 0)
					model.rotation.y += 360;
				Point3f direction = Point3f(1, 0, 0);
				if (ComMath::IsSameScalar(mirror.rotation.y, 0.f, geomance_tolerance))
					direction = Point3f(1.f, 0.f, 0.f);
				if (ComMath::IsSameScalar(mirror.rotation.y, 90.f, geomance_tolerance))
					direction = Point3f(0.f, 1.f, 0.f);
				if (ComMath::IsSameScalar(mirror.rotation.y, 180.f, geomance_tolerance))
					direction = Point3f(-1.f, 0.f, 0.f);
				if (ComMath::IsSameScalar(mirror.rotation.y, 270.f, geomance_tolerance))
					direction = Point3f(0.f, -1.f, 0.f);
				if (ComMath::Dot(bed_direction, direction) >= 0)
					continue;

				auto angle = mirror.rotation.y / 180.f*M_PI;
				auto mirror_beg = Point3f(0, 0.5f*mirror.width, 0.f);
				auto mirror_end = Point3f(0, -0.5f*mirror.width, 0.f);

				auto x = mirror_beg.x*cosf(angle) - sinf(angle)*mirror_beg.y;
				mirror_beg.y = mirror_beg.x*sinf(angle) + cosf(angle)*mirror_beg.y;
				mirror_beg.x = x;
				mirror_beg = mirror_beg + mirror.location;

				x = mirror_end.x*cosf(angle) - sinf(angle)*mirror_end.y;
				mirror_end.y = mirror_end.x*sinf(angle) + cosf(angle)*mirror_end.y;
				mirror_end.x = x;
				mirror_end = mirror_end + mirror.location;
				//获取
				bool intersect = false;
				for (int i = 0; i < bed_header.size(); ++i)
				{
					int j = (i + 1) % bed_header.size();
					auto beg = bed_header[i];
					auto end = bed_header[j];
					auto length = ComMath::getTwoPointDistance(beg, end);
					auto normal = ComMath::Normalize(end - beg);
					auto temp_length = 0.f;
					auto temp_point = beg;
					while (temp_length<length)
					{
						auto  proj = ComMath::getPointToLinePedal(temp_point, mirror_beg, mirror_end);
						if (ComMath::isPointBetweenPoint(proj, mirror_beg, mirror_end)) {
							intersect = true;
							break;
						}
						temp_length += 20.f;
						temp_point = temp_point + normal * 20.f;
					}
					if (intersect)
						break;

				}
				if (intersect)
					++result.count;
			}
		}
	}
	return result;
} 
// 书桌不能面朝窗户，否则容易望空，扣1分；21
GeomancyData MetisGeomancy::CheckDeskToWindow()
{
	GeomancyData result;
	result.id = 21;
	result.count = 0;
	result.score = 1;
	// 处理书桌（inner_type=106)
 
	for (auto &room : ai_case->single_case_list)
	{
		for (auto &model : room.model_list)
		{
			if (model.inner_type != 106)
				continue;
			//计算书桌的方向
			model.rotation.y = static_cast<float>(((int)model.rotation.y) % 360);
			if (model.rotation.y < 0)
				model.rotation.y += 360;
			auto direction = Point3f(1.f, 0.f, 0.f);
			if (ComMath::IsSameScalar(model.rotation.y, 0.f, geomance_tolerance))
				direction = Point3f(1.f, 0.f, 0.f);
			if (ComMath::IsSameScalar(model.rotation.y, 90.f, geomance_tolerance))
				direction = Point3f(0.f, 1.f, 0.f);
			if (ComMath::IsSameScalar(model.rotation.y, 180.f, geomance_tolerance))
				direction = Point3f(-1.f, 0.f, 0.f);
			if (ComMath::IsSameScalar(model.rotation.y, 270.f, geomance_tolerance))
				direction = Point3f(0.f, -1.f, 0.f);

			std::vector<Point3f> bed_header;

			bed_header.push_back(Point3f(-0.5f*model.length, 0.5f*model.width, 0.f));
			bed_header.push_back(Point3f(0.5f*model.length, 0.5f*model.width, 0.f));
			bed_header.push_back(Point3f(0.5f*model.length, -0.5f*model.width, 0.f));
			bed_header.push_back(Point3f(-0.5f*model.length, -0.5f*model.width, 0.f));
 

			auto angle = model.rotation.y / 180.f*M_PI;
			for (auto&point : bed_header)
			{
				auto x = point.x*cosf(angle) - sinf(angle)*point.y;
				point.y = point.x*sinf(angle) + cosf(angle)*point.y;
				point.x = x;
				point = point + model.location;
			}

			for (auto &windows : room.window_list)
			{
				//面向
				if (ComMath::Dot((windows.start_point + windows.end_point)*0.5f - model.location, direction) > 0)
					continue;
				//垂直
				if (ComMath::Dot(windows.end_point - windows.start_point, direction) != 0)
					continue;
				bool intersect = false;
			 
				for (int i = 0; i < bed_header.size(); ++i)
				{
					int j = (i + 1) % bed_header.size();
					auto beg = bed_header[i];
					auto end = bed_header[j];
					auto length = ComMath::getTwoPointDistance(beg, end);
					auto normal = ComMath::Normalize(end - beg);
					auto temp_length = 0.f;
					auto temp_point = beg;
					while (temp_length< length)
					{
						auto  proj = ComMath::getPointToLinePedal(temp_point, windows.start_point, windows.end_point);
						if (ComMath::isPointBetweenPoint(proj, windows.start_point, windows.end_point)) {
							++result.count;
							intersect = true;
							break;
						}
						temp_length += 20.f;
						temp_point = temp_point + normal * 20.f;
					}
					if (intersect)
						break;
				}
			}
		 }
	}
	return result;
}
// 书桌不能正对大门，否则容易受干扰，扣1分；22
GeomancyData MetisGeomancy::CheckDeskToDoor()
{
	GeomancyData result;
	result.id = 22;
	result.count = 0;
	result.score = 1;
	// 处理书桌（inner_type=106)

	for (auto &room : ai_case->single_case_list)
	{
		for (auto &model : room.model_list)
		{
			if (model.inner_type != 106)
				continue;
			//计算书桌的方向
			auto direction = Point3f(1.f, 0.f, 0.f);
			model.rotation.y = static_cast<float>(((int)model.rotation.y) % 360);
			if (model.rotation.y < 0)
				model.rotation.y += 360;
			if (ComMath::IsSameScalar(model.rotation.y, 0.f, geomance_tolerance))
				direction = Point3f(1.f, 0.f, 0.f);
			if (ComMath::IsSameScalar(model.rotation.y, 90.f, geomance_tolerance))
				direction = Point3f(0.f, 1.f, 0.f);
			if (ComMath::IsSameScalar(model.rotation.y, 180.f, geomance_tolerance))
				direction = Point3f(-1.f, 0.f, 0.f);
			if (ComMath::IsSameScalar(model.rotation.y, 270.f, geomance_tolerance))
				direction = Point3f(0.f, -1.f, 0.f);

			std::vector<Point3f> desk;

			desk.push_back(Point3f(-0.5f*model.length, 0.5f*model.width, 0.f));
			desk.push_back(Point3f(0.5f*model.length, 0.5f*model.width, 0.f));
			desk.push_back(Point3f(0.5f*model.length, -0.5f*model.width, 0.f));
			desk.push_back(Point3f(-0.5f*model.length, -0.5f*model.width, 0.f));


			auto angle = model.rotation.y / 180.f*M_PI;
			for (auto&point : desk)
			{
				auto x = point.x*cosf(angle) - sinf(angle)*point.y;
				point.y = point.x*sinf(angle) + cosf(angle)*point.y;
				point.x = x;
				point = point + model.location;
			}
			for (auto &doors : room.door_list)
			{
				if (ComMath::Dot(doors.end_point - doors.start_point, direction) != 0)
					continue;
				bool intersect = false;
				for (int i = 0; i < desk.size(); ++i)
				{
					int j = (i + 1) % desk.size();
					auto beg = desk[i];
					auto end = desk[j];
					auto length = ComMath::getTwoPointDistance(beg, end);
					auto normal = ComMath::Normalize(end - beg);
					auto temp_length = 0.f;
					auto temp_point = beg;
					while (temp_length < length)
					{
						auto  proj = ComMath::getPointToLinePedal(temp_point, doors.start_point, doors.end_point);
						if (ComMath::isPointBetweenPoint(proj, doors.start_point, doors.end_point)) {
							++result.count;
							intersect = true;
							break;
						}
						temp_length += 20.f;
						temp_point = temp_point + normal * 20.f;
					}
					if (intersect)
						break;
				}
			}
		}
	}
	return result;
}
// 书桌背后应靠墙，否则没有靠山，扣1分23
GeomancyData MetisGeomancy::CheckDeskToWall()
{
	GeomancyData result;
	result.id = 23;
	result.count = 0;
	result.score = 1;
	// 处理书桌（inner_type=106)
	for (auto &room : ai_case->single_case_list)
	{
		for (auto &model : room.model_list)
		{
			if (model.inner_type != 106)
				continue;
			auto direction = Point3f(1.f, 0.f, 0.f);
			model.rotation.y = static_cast<float>(((int)model.rotation.y) % 360);
			if (model.rotation.y < 0)
				model.rotation.y += 360;
			if (ComMath::IsSameScalar(model.rotation.y, 0.f, geomance_tolerance))
				direction = Point3f(1.f, 0.f, 0.f);
			if (ComMath::IsSameScalar(model.rotation.y, 90.f, geomance_tolerance))
				direction = Point3f(0.f, 1.f, 0.f);
			if (ComMath::IsSameScalar(model.rotation.y, 180.f, geomance_tolerance))
				direction = Point3f(-1.f, 0.f, 0.f);
			if (ComMath::IsSameScalar(model.rotation.y, 270.f, geomance_tolerance))
				direction = Point3f(0.f, -1.f, 0.f);

			auto location = model.location;

			bool lay_wall = true;

			for (auto &windows : room.window_list)
			{
				auto proj = ComMath::getPointToLinePedal(location, windows.start_point, windows.end_point);
				if (ComMath::isPointOnLine(proj, windows.start_point, windows.end_point)&&ComMath::Dot(proj-location, direction))
				{
					lay_wall = false;
				}
			}

			for (auto &door : room.door_list)
			{
				auto proj = ComMath::getPointToLinePedal(location, door.start_point, door.end_point);
				if (ComMath::isPointOnLine(proj, door.start_point, door.end_point) && ComMath::Dot(proj - location, direction)>0.f)
				{
					lay_wall = false;
				}
			}
 
			if (!lay_wall)
				++result.count;
		}
	}
	return result;
}
// 风水扣分结构体json化
Json::Value MetisGeomancy::GeomancyDataToJson(GeomancyData data)
{
	Json::Value geomancy;
	geomancy["id"] = data.id;
	geomancy["count"] = data.count;
	geomancy["score"] = data.score;

	return geomancy;
}

//获得分数列表
Json::Value MetisGeomancy::GetGeomancyScoreList()
{
	Json::Value  scoreList;
	// 入户门对卫生间门10
	GeomancyData entrance_to_toilet = EntranceToToilet();
	if (entrance_to_toilet.count > 0)
	{
		scoreList.append(GeomancyDataToJson(entrance_to_toilet));
	}
	// 入户门对厨房门11
	GeomancyData entrance_to_kitchen = EntranceToKitchen();
	if (entrance_to_kitchen.count > 0)
	{
		scoreList.append(GeomancyDataToJson(entrance_to_kitchen));
	}
	// 卧室门对卫生间门12
	GeomancyData bedroom_to_toilet = BedroomToToilet();
	if (bedroom_to_toilet.count > 0)
	{
		scoreList.append(GeomancyDataToJson(bedroom_to_toilet));
	}
	// 卧室门对厨房门13
	GeomancyData bedroom_to_kitchen = BedroomToKitchen();
	if (bedroom_to_kitchen.count > 0)
	{
		scoreList.append(GeomancyDataToJson(bedroom_to_kitchen));
	}
	// 卧室门对入户门14
	GeomancyData bedroom_to_entrance = BedroomToEntrance();
	if (bedroom_to_entrance.count > 0)
	{
		scoreList.append(GeomancyDataToJson(bedroom_to_entrance));
	}
	// 卫生间门对厨房门15
	GeomancyData toilet_to_kitchen = ToiletToKitchen();
	if (toilet_to_kitchen.count > 0)
	{
		scoreList.append(GeomancyDataToJson(toilet_to_kitchen));
	}
	// 室内尖角16
	GeomancyData sharp_angle = CheckSharpAngle();
	if (sharp_angle.count > 0)
	{
		scoreList.append(GeomancyDataToJson(sharp_angle));
	}
	GeomancyData sofa_to_door = CheckSoftToDoor();
	if (sofa_to_door.count > 0)
	{
		scoreList.append(GeomancyDataToJson(sofa_to_door));
	}
	GeomancyData table_door = CheckTableToDoor();
	if (table_door.count > 0)
	{
		scoreList.append(GeomancyDataToJson(table_door));
	}
	GeomancyData bed_header_door = CheckBedHeadToDoor();
	if (bed_header_door.count > 0)
	{
		scoreList.append(GeomancyDataToJson(bed_header_door));
	}
	GeomancyData bed_header_mirror = CheckBedHeadToMirror();
	if (bed_header_mirror.count > 0)
	{
		scoreList.append(GeomancyDataToJson(bed_header_mirror));
	}
	GeomancyData desk_windows = CheckDeskToWindow();
	if (desk_windows.count > 0)
	{
		scoreList.append(GeomancyDataToJson(desk_windows));
	}
	GeomancyData desk_door = CheckDeskToDoor();
	if (desk_door.count > 0)
	{
		scoreList.append(GeomancyDataToJson(desk_door));
	}
	GeomancyData desk_wall = CheckDeskToWall();
	if (desk_wall.count > 0)
	{
		scoreList.append(GeomancyDataToJson(desk_wall));
	}
	// 客厅沙发处出发，垂直方向不应看到房间门，否则有直捣黄龙之意 17
	// 餐桌不可只对大门，否则容易被煞气冲散、把财气带走，扣2分；18
	// 卧室床头不对着房门；否则阴气较重，不利于主人健康，扣3分；19
	// 镜子不能直对着枕头位，否则容易犯煞，扣3分；20
	// 书桌不能面朝窗户，否则容易望空，扣1分；21
	// 书桌不能正对大门，否则容易受干扰，扣1分；22
	// 书桌背后应靠墙，否则没有靠山，扣1分23

	return scoreList;
}