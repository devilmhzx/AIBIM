#include "KitchenAutomaticalDesign.h"
const float WaterWall_Benchmark = 40.f;
const float ChimneyWall_Benchmark = 40.f;
const float DoorOrientation_Benchmark = 40.f;

const float MinWallLength = 50.f;
const float  cabinetDeep = 60.f;
const float CookerWidth = 40;
const float MinWallInterval = 240;
const float MinimumCabinetLength = 60.f;
#define per_dinner_length		40.f
#define water_slot_length_max	80.f
#define water_slot_length_min  60.f
#define cooker_length_max		75.f
#define fridge_length				60.f
#define extend_length				10.f
//������Сȡֵ
enum
{
	WIDTH,
	DEPTH,
	HEIGHT
};

//����
const float WaterWall_Score = 10.f;
const float ChimneyWall_Score = 10.f;
const float DoorOrientation_Score = 10.f;

// �����Զ�����
bool KitchenDesign::AutoDesign(SingleCase*single_case)
{
	//  Ѱ�һ�׼ǽ��
	SearchBenchmarkWall(single_case);
	Wall*wall = ChooseBenckWall(single_case);
	if (!wall)
		return false;
	//��Ӻ�ѡǽ��
	AddOtherWallToCandidateWall(wall, single_case);
	if (!m_candidate_wall.size())
		return false;
	//��С���ų�ͻ��ǽ��(����������ǽ�ϣ��ͷ�����Ӱ�죩
	DetectCollisionWithDoor(single_case);
	//������Чǽ��
	ShrinkCabinetWall(single_case);
	//����ǽ�ķ���
	CalculateCabinetWallDirection(single_case);
	//���ַ���Ϊ��߻�׼
	CaseLayoutWithCoorker();
	//���ַ���Ϊ���ǽ���׼
	CaseLayoutWithMaxWall();
	//���ַ���Ϊ������׼
	CaseLayoutWithWindows();
	//û�з������󷵻�
	if (!m_all_case.size())
		return false;
	//���ֱ�������ɼҾ�position
	CompatibleLayout(single_case);
	//������������ϵ
	GenerateModeLayout(single_case);
	return true;
}

void KitchenDesign::GenerateModeLayout(SingleCase*single_case) {

	for (auto &ref : m_all_case) {
		ModelLayout layout;
		for (auto &sub : ref.real_case) {
			for (auto i = 0; i < sub.cabinetBeg.size(); ++i)
			{
				bool add = false;
				if (ComMath::getTwoPointDistance(sub.cabinetBeg[i], sub.cabinetEnd[i]) < 40.f)
					continue;
				Model model;
				model.product_id = E_Cabinet;
				model.SetNo(ComUtil::getGuuidNo());
				//model.layout_rule = CabinetRuleNo;
				model.width = cabinetDeep;
				model.id = GROUP_CABINET_LAND;
				model.length = ComMath::getTwoPointDistance(sub.cabinetBeg[i], sub.cabinetEnd[i]);
				auto centre = (sub.cabinetBeg[i] + sub.cabinetEnd[i])*0.5f;
				auto location = centre + sub.direction*(model.width*0.5f);
				model.location = location;
				centre = ComMath::Cross(Point3f(1.f, 0.f, 0.f), sub.direction);
				auto angle = ComMath::GetAngleBetweenTwoLines(Point3f(1.f, 0.f, 0.f), sub.direction);
				model.rotation = sub.direction;
				if (centre.z > 0) {
					model.rotation.y = angle / (3.1415926f * 2) * 360;
				}
				else {
					model.rotation.y = -angle / (3.1415926f * 2) * 360;
				}
				layout.model_list.push_back(model);
			}
			for (auto &subLayout : sub.subLayout)
			{
				for (auto i = 0; i < subLayout.furnitureBeg.size(); ++i)
				{
					Model model;
					model.product_id = subLayout.furnitureType[i];
					model.SetNo(ComUtil::getGuuidNo());
					/*if (model.id == E_Water_Solt)
						model.layout_rule = WatersoltRuleNo;
					if (model.id == E_Cooker)
						model.layout_rule = CookerRuleNo;*/

					if (model.product_id == E_Water_Solt)
						model.id = GROUP_SINK;
					if (model.product_id == E_Cooker)
						model.id = GROUP_STOVE;
					if (model.product_id == 0)
						continue;
					model.width = CookerWidth;
					model.length = ComMath::getTwoPointDistance(subLayout.furnitureBeg[i], subLayout.furnitureEnd[i]);
					auto centre = (subLayout.furnitureBeg[i] + subLayout.furnitureEnd[i])*0.5f;
					auto location = centre + sub.direction*(model.width*0.5f);
					model.location = location;
					model.rotation = sub.direction;
					centre = ComMath::Cross(Point3f(1.f, 0.f, 0.f), sub.direction);
					auto angle = ComMath::GetAngleBetweenTwoLines(Point3f(1.f, 0.f, 0.f), sub.direction);
					if (centre.z > 0) {
						model.rotation.y = angle / (3.1415926f * 2) * 360;
					}
					else {
						model.rotation.y = -angle / (3.1415926f * 2) * 360;
					}
					layout.model_list.push_back(model);
				}
				if (subLayout.fridgeType.size()) {
					Model model;
					model.product_id = E_Fridge;
					model.id = GROUP_REFIGERATOR;
					model.width = CookerWidth;
					model.SetNo(ComUtil::getGuuidNo());
					model.length = ComMath::getTwoPointDistance(subLayout.fridgeBeg, subLayout.fridgeEnd);
					auto centre = (subLayout.fridgeBeg + subLayout.fridgeEnd)*0.5f;
					auto location = centre + sub.direction*(model.width*0.5f);
					model.location = location;
					model.rotation = sub.direction;
					centre = ComMath::Cross(Point3f(1.f, 0.f, 0.f), sub.direction);
					auto angle = ComMath::GetAngleBetweenTwoLines(Point3f(1.f, 0.f, 0.f), sub.direction);
					if (centre.z > 0) {
						model.rotation.y = angle / (3.1415926f * 2) * 360;
					}
					else {
						model.rotation.y = -angle / (3.1415926f * 2) * 360;
					}
					layout.model_list.push_back(model);
				}

			}
		}
		single_case->layout_list.push_back(std::move(layout));
	}

}

//  Ѱ�Һ�ѡǽ��
void KitchenDesign::SearchBenchmarkWall(SingleCase*single_case)
{
	// ѡ����ˮλ��صĺ�ѡǽ��
	SearchWaterPointWall(single_case);
	// ѡ������̵���صĺ�ѡǽ��
	SearchChmineyWall(single_case);
	// ѡ�����ŷ�����صĺ�ѡǽ��
	SearchReverseWall(single_case);
}

// ѡ����ˮλ��صĺ�ѡǽ��
void KitchenDesign::SearchWaterPointWall(SingleCase*single_case)
{
	shared_ptr<FloorBox> floor_box;
	// ��ȡ��ˮ�������ǽ��
	for (auto &box : single_case->floor_box_list)
	{
		if (box.floor_box_type == FB_UP_WATER_GAP)
		{
			floor_box = make_shared<FloorBox>(box);
		}
	}
	if (!floor_box)
	{
		return;
	}
	Wall*benchmarkwall = NULL;
	auto Dist = FLT_MAX;
	for (auto &wall : single_case->wall_list)
	{
		if (wall.door_list.size())
			continue;
		auto dist = ComMath::getTwoPointDistance(wall.end_corner.point, wall.start_corner.point);
		if (dist < WaterWall_Benchmark)
			continue;
		dist = ComMath::getPointToLineDis(floor_box->center, wall.start_corner.point, wall.end_corner.point);
		if (dist < Dist) 
		{
			benchmarkwall = &wall;
			Dist = dist;
		}
	}
	if (!benchmarkwall)
		return;
	//�ж�����ǽ���ǲ���Ҳ�����
	for (auto &wall : single_case->wall_list) {
		if (benchmarkwall == &wall)
			continue;
		if (wall.door_list.size())
			continue;
		auto length = ComMath::ScaleOfVector(wall.end_corner.point - wall.start_corner.point);
		if (length < WaterWall_Benchmark)
			continue;
		auto dist = ComMath::getPointToLineDis(floor_box->center, wall.start_corner.point, wall.end_corner.point);
		if (ComMath::IsSameScalar(Dist, dist, 1.f))
		{
			auto length2 = ComMath::getTwoPointDistance(benchmarkwall->end_corner.point, benchmarkwall->start_corner.point);
			if (length > length2)
				benchmarkwall = &wall;
		}
	}
	auto iter = m_benckmark_wall.find(benchmarkwall);
	if (iter != m_benckmark_wall.end()) {
		iter->second.insert(E_WaterPointWall);
	}
	else {
		std::set<WallType> Set;
		Set.insert(E_WaterPointWall);
		m_benckmark_wall.insert(std::make_pair(benchmarkwall, Set));
	}
}

void	KitchenDesign::SearchChmineyWall(SingleCase*single_case) {
	FloorBox*Box = nullptr;
	//��ȡ�����ǽ��
	for (auto &box : single_case->floor_box_list) {
		if (box.floor_box_type == FB_CHIMNEY_PIPING)
			Box = &box;
	}
	if (!Box)
		return;
	Wall*benchmarkwall = NULL;
	auto Dist = FLT_MAX;
	for (auto &wall : single_case->wall_list) {
		if (wall.door_list.size())
			continue;
		auto dist = ComMath::getTwoPointDistance(wall.end_corner.point, wall.start_corner.point);
		if (dist < WaterWall_Benchmark)
			continue;
		dist = ComMath::getPointToLineDis(Box->center, wall.start_corner.point, wall.end_corner.point);
		if (dist < Dist) {
			benchmarkwall = &wall;
			Dist = dist;
		}
	}
	if (!benchmarkwall)
		return;
	//�ж�����ǽ���ǲ���Ҳ�����
	for (auto &wall : single_case->wall_list) {
		if (benchmarkwall == &wall)
			continue;
		if (wall.door_list.size())
			continue;
		auto length = ComMath::ScaleOfVector(wall.end_corner.point - wall.start_corner.point);
		if (length < WaterWall_Benchmark)
			continue;
		auto dist = ComMath::getPointToLineDis(Box->center, wall.start_corner.point, wall.end_corner.point);
		if (ComMath::IsSameScalar(Dist, dist, 1.f))
		{
			auto length2 = ComMath::getTwoPointDistance(benchmarkwall->end_corner.point, benchmarkwall->start_corner.point);
			if (length > length2)
				benchmarkwall = &wall;
		}
	}
	auto iter = m_benckmark_wall.find(benchmarkwall);
	if (iter != m_benckmark_wall.end()) {
		iter->second.insert(E_ChmineyWall);
	}
	else {
		std::set<WallType> Set;
		Set.insert(E_ChmineyWall);
		m_benckmark_wall.insert(std::make_pair(benchmarkwall, Set));
	}
}

void	KitchenDesign::SearchReverseWall(SingleCase*single_case) {
	std::vector<Point3f>	polygon;
	for (auto &corner : single_case->corner_list) {
		polygon.push_back(corner.point);
	}
	std::unordered_map<Wall*, float>	score_map;
	for (auto &wall : single_case->wall_list)
	{
		for (auto &door : wall.door_list) {
			//���ڲ�ͬǽ��Ĵ���
			auto door_extend = (door.start_point + door.end_point)*0.5f + door.direction*10.f;
			auto inside = ComMath::PointInPolygon(door_extend, polygon);
			if (door.door_type == D_NORMAL && inside) {
				//�ҵ��ǿ��ŷ����ǽ��
				for (auto &s_wall : single_case->wall_list) {
					if (&wall == &s_wall)
						continue;
					if (s_wall.door_list.size())
						continue;

					Point3f Line1 = s_wall.end_corner.point - s_wall.start_corner.point;
					Point3f Line2 = door.end_point - door.start_point;
					float angle = ComMath::GetAngleBetweenTwoLines(Line1, Line2);
					//45�ȵ�135�� ����ֱ��
					if (0.7853f > angle || angle > 2.3559f)
						continue;
					auto centre_point = (s_wall.end_corner.point + s_wall.start_corner.point) * 0.5f;
					auto door_centre = (door.start_point + door.end_point)*0.5;
					centre_point = ComMath::Cross(centre_point - door_centre, door.direction);
					if (door.right_open)
						if (centre_point.z < 0)
							continue;
					if (!door.right_open)
						if (centre_point.z > 0)
							continue;
					auto length = ComMath::getTwoPointDistance(s_wall.start_corner.point, s_wall.end_corner.point);
					if (length < DoorOrientation_Benchmark)
						continue;


					auto iter = score_map.find(&s_wall);
					if (iter == score_map.end())
						score_map.insert(std::make_pair(&s_wall, 1.f));
					else
						iter->second += 1.f;
				}
			}
			else
			{
				//����Զ��ǽ��
				float min_dist = FLT_MIN;
				Wall*t_wall = nullptr;
				for (auto &s_wall : single_case->wall_list)
				{
					Point3f Line1 = s_wall.end_corner.point - s_wall.start_corner.point;
					Point3f Line2 = door.end_point - door.start_point;
					float angle = ComMath::GetAngleBetweenTwoLines(Line1, Line2);
					//45�ȵ�135�� ����ֱ��
					if (0.7853f > angle || angle > 2.3559f)
						continue;
					auto centre = (door.start_point + door.end_point)*0.5f;
					auto proj = ComMath::getPointToLinePedal(centre, s_wall.start_corner.point, s_wall.end_corner.point);
					auto dist = ComMath::getTwoPointDistance(proj, centre);
					if (dist > min_dist) {
						t_wall = &s_wall;
						min_dist = dist;
					}
				}
				if (t_wall) {
					auto iter = score_map.find(t_wall);
					if (iter == score_map.end())
						score_map.insert(std::make_pair(t_wall, 1.f));
					else
						iter->second += 1.f;
				}
			}
		}
	}
	float max_score = 0;
	Wall*benchmarkwall = nullptr;
	for (auto score : score_map)
	{
		if (score.second > max_score) {
			max_score = score.second;
			benchmarkwall = score.first;
		}
		else if (score.second == max_score)
		{
			auto  dist = ComMath::getTwoPointDistance(score.first->start_corner.point, score.first->end_corner.point);
			auto  dist_2 = ComMath::getTwoPointDistance(benchmarkwall->start_corner.point, benchmarkwall->end_corner.point);
			if (dist > dist_2)
			{
				max_score = score.second;
				benchmarkwall = score.first;
			}
		}
	}
	if (benchmarkwall) {
		auto iter = m_benckmark_wall.find(benchmarkwall);
		if (iter != m_benckmark_wall.end()) {
			iter->second.insert(E_DoorReverseWall);
		}
		else {
			std::set<WallType> Set;
			Set.insert(E_DoorReverseWall);
			m_benckmark_wall.insert(std::make_pair(benchmarkwall, Set));
		}
	}
}

float	KitchenDesign::CalculateBenchmarkWall(std::set<WallType>&wallType) {
	float score = 0.f;

	for (auto type : wallType) {
		switch (type)
		{
		case KitchenDesign::E_WaterPointWall:
			score += WaterWall_Score;
			break;
		case KitchenDesign::E_ChmineyWall:
			score += ChimneyWall_Score;
			break;
		case KitchenDesign::E_DoorReverseWall:
			score += DoorOrientation_Score;
			break;
		default:
			break;
		}
	}
	return score;
}

Wall*KitchenDesign::ChooseBenckWall(SingleCase*single_case) {
	//û���ҵ���׼ǽ��
	Wall* wall = NULL;
	if (!m_benckmark_wall.size()) {
		//ѡ�����ǽ��
		float max = -FLT_MAX;
		for (auto &swall : single_case->wall_list) {
			auto dist = ComMath::getTwoPointDistance(swall.start_corner.point, swall.end_corner.point);
			if (dist > max) {
				wall = &swall;
				max = dist;
			}
		}
	}
	else {
		float max = -FLT_MAX;
		//����������ǽ��
		for (auto &benck : m_benckmark_wall) {
			auto score = CalculateBenchmarkWall(benck.second);
			if (score > max) {
				wall = benck.first;
				max = score;
			}
		}
	}
	return wall;
}

void	KitchenDesign::AddOtherWallToCandidateWall(Wall* wall, SingleCase*single_case) {
	/*
	std::set<Wall*>	Temp;
	Temp.insert(wall);

	for (auto &wall : single_case->wall_list)
	{
		bool Insert = false;
		bool exchange = false;
		if (wall.door_list.size())
			continue;
		Wall* exchange_wall =nullptr;
		for (auto can_wall : Temp) {
			if (can_wall == &wall)
				continue;
			//���㳤��
			auto dist = ComMath::getTwoPointDistance(wall.start_corner.point, wall.end_corner.point);
			if (dist < MinWallLength)
				break;
			//����ƽ�г���
			Point3f Line1 = wall.end_corner.point - wall.start_corner.point;
			Point3f Line2 = can_wall->start_corner.point - can_wall->end_corner.point;
			float angle = ComMath::GetAngleBetweenTwoLines(Line1, Line2);
			if (0.7853f < angle && angle < 2.3559f)
			{
				Insert =true;
				continue;
			}
			auto proj = ComMath::getPointToLinePedal(wall.end_corner.point, can_wall->start_corner.point, can_wall->end_corner.point);
			auto length = ComMath::getTwoPointDistance(proj, wall.end_corner.point);

			//ƽ��ǽ�� ����ͶӰ���ص�
			//����ǽ�巽��һ�£�240cm ������
			auto wall_direction = ComMath::Normalize(wall.end_corner.point - wall.start_corner.point);
			auto x = wall_direction.x*cos(M_PI / 2) + wall_direction.y*sin(M_PI / 2);
			wall_direction.y= wall_direction.x*sin(M_PI / 2) - wall_direction.y*cos(M_PI / 2);
			wall_direction.x = x;
			auto wall_2_direction = ComMath::Normalize(can_wall->end_corner.point - can_wall->start_corner.point);
			x = wall_2_direction.x*cos(M_PI / 2) + wall_2_direction.y*sin(M_PI / 2);
			wall_2_direction.y= wall_2_direction.x*sin(M_PI / 2) - wall_2_direction.y*cos(M_PI / 2);
			wall_2_direction.x = x;
			bool is_same_direction = ComMath::Dot(wall_direction, wall_2_direction)>0;
			if (!is_same_direction) {
				if (length < 240)
				{
					auto wall_dist2 = ComMath::getTwoPointDistance(can_wall->start_corner.point, can_wall->end_corner.point);
					if (dist < wall_dist2) {
						exchange = true;
						Insert = false;
					}
				}
				else
				{
					Insert = true;
				}
			}
			else
				Insert = true;
		}
		if (Insert) {
			if (Temp.find(&wall) == Temp.end())
				Temp.insert(&wall);
		}
		if (exchange) {
			Temp.erase(&wall);
			Temp.insert(exchange_wall);
		}
	}
	*/


	std::set<Wall*>	TempSet;
	TempSet.insert(wall);
	std::unordered_map<Wall*, float> temp_map;
	for (auto &wall : single_case->wall_list) {
		if (wall.door_list.size())
			continue;
		temp_map.insert(std::make_pair(&wall, ComMath::getTwoPointDistance(wall.start_corner.point, wall.end_corner.point)));
	}
	while (temp_map.size())
	{
		Wall*wall = nullptr;
		float max = -FLT_MIN;
		for (auto ref : temp_map) {
			if (ref.second > max) {
				wall = ref.first;
				max = ref.second;
			}
		}
		bool insert = true;
		for (auto pwall : TempSet) {

			Point3f Line1 = pwall->end_corner.point - pwall->start_corner.point;
			Point3f Line2 = wall->start_corner.point - wall->end_corner.point;
			float angle = ComMath::GetAngleBetweenTwoLines(Line1, Line2);
			if (0.7853f < angle && angle < 2.3559f)
			{
				continue;
			}
			auto proj = ComMath::getPointToLinePedal(wall->end_corner.point, pwall->start_corner.point, pwall->end_corner.point);
			auto length = ComMath::getTwoPointDistance(proj, wall->end_corner.point);


			//ƽ��ǽ�� ����ͶӰ���ص�
			//����ǽ�巽��һ�£�240cm ������
			auto wall_direction = ComMath::Normalize(wall->end_corner.point - wall->start_corner.point);
			auto x = wall_direction.x*cos(M_PI / 2) + wall_direction.y*sin(M_PI / 2);
			wall_direction.y = wall_direction.x*sin(M_PI / 2) - wall_direction.y*cos(M_PI / 2);
			wall_direction.x = x;
			auto wall_2_direction = ComMath::Normalize(pwall->end_corner.point - pwall->start_corner.point);
			x = wall_2_direction.x*cos(M_PI / 2) + wall_2_direction.y*sin(M_PI / 2);
			wall_2_direction.y = wall_2_direction.x*sin(M_PI / 2) - wall_2_direction.y*cos(M_PI / 2);
			wall_2_direction.x = x;
			bool is_same_direction = ComMath::Dot(wall_direction, wall_2_direction) > 0;
			if (!is_same_direction) {
				if (length < MinWallInterval)
					insert = false;
			}
			else if (ComMath::getTwoPointDistance(wall->start_corner.point, wall->end_corner.point) < MinWallLength)
			{
				insert = false;
			}
		}
		if (insert) {
			TempSet.insert(wall);
		}
		temp_map.erase(wall);
	}

	for (auto wall : TempSet)
		m_candidate_wall.push_back(wall);
}

bool KitchenDesign::DetectCollisionWithDoor(SingleCase*single_case) {
	//find door
	std::vector<Door*>door_list;
	for (auto &p_wall : single_case->wall_list)
	{
		for (auto &door : p_wall.door_list)
		{
			door_list.push_back(&door);
		}
	}
	//���š�������� ���ش���
	if (!door_list.size())
		return false;
	for (auto p_wall : m_candidate_wall) {
		if (p_wall->door_list.size())
			WallWithDoorCollision(p_wall, single_case);
		else
			WallWithoutDoorCollision(p_wall, door_list, single_case);
	}
	return true;
}

void	KitchenDesign::WallWithDoorCollision(Wall*wall, SingleCase*single_case) {
	auto door = wall->door_list[0];
	auto &start = wall->start_corner.point;
	auto &end = wall->end_corner.point;
	auto min1 = ComMath::getTwoPointDistance(start, door.start_point);
	auto min2 = ComMath::getTwoPointDistance(start, door.end_point);
	CabinetWall cabinetWall;

	if (min1 < min2) {
		if (min1 > MinWallLength)
		{
			cabinetWall.p_wall = wall;
			cabinetWall.cabinetBeg.push_back(start);
			cabinetWall.subLayout.push_back(Container());
			cabinetWall.cabinetEnd.push_back(door.start_point);
		}
		min1 = ComMath::getTwoPointDistance(end, door.end_point);
		if (min1 > MinWallLength)
		{
			cabinetWall.p_wall = wall;
			cabinetWall.cabinetBeg.push_back(door.end_point);
			cabinetWall.subLayout.push_back(Container());
			cabinetWall.cabinetEnd.push_back(end);
		}
	}
	else
	{
		if (min2 > MinWallLength)
		{
			cabinetWall.p_wall = wall;
			cabinetWall.cabinetBeg.push_back(start);
			cabinetWall.subLayout.push_back(Container());
			cabinetWall.cabinetEnd.push_back(door.end_point);
		}
		min2 = ComMath::getTwoPointDistance(end, door.start_point);
		if (min2 > MinWallLength)
		{
			cabinetWall.p_wall = wall;
			cabinetWall.cabinetBeg.push_back(end);
			cabinetWall.subLayout.push_back(Container());
			cabinetWall.cabinetEnd.push_back(door.start_point);
		}
	}
	if (cabinetWall.cabinetBeg.size()) {
		m_cabinet_wall.push_back(std::move(cabinetWall));
	}
}

void KitchenDesign::WallWithoutDoorCollision(Wall*wall, std::vector<Door*>&door_list, SingleCase*single_case)
{
	auto beg = wall->start_corner.point;
	auto end = wall->end_corner.point;

	auto centre = (wall->start_corner.point + wall->end_corner.point)*0.5f;
	auto vec = beg - end;
	auto x = cosf(M_PI / 2)*vec.x - sinf(M_PI / 2)*vec.y;
	auto y = sinf(M_PI / 2)*vec.x + cosf(M_PI / 2)*vec.y;
	auto wall_direction = ComMath::Normalize(Point3f(x, y, 0.f));


	if (!ComMath::PointInPolygon(centre + wall_direction * 20.f, single_case->single_room.getPointList()))
		wall_direction = -wall_direction;


	for (auto door : door_list)
	{
		//��չ�ŵķ�����������⿪���е�ͷ��������ɵ������ڷ�������
		//��ʱ���ῼ����ײ����
		auto centre = (door->start_point + door->end_point)*0.5f;
		auto door_extend = centre + door->direction*70.f;
		auto inside = ComMath::PointInPolygon(door_extend, single_case->single_room.getPointList());
		//�����ŵ�AABB bounding box
		float length = 70.f;
		if (door->door_type == DoorType::D_NORMAL)
			length = (float)door->length;
		else if (door->door_type == DoorType::D_SLIDING)
			continue;

		if (inside)
			door_extend = door->direction;
		else {
			door_extend = -door->direction;
			length = 70.f;
		}


		Point3f Point1 = door->start_point + door_extend * length;
		Point3f Point2 = door->end_point + door_extend * length;
		std::vector<Point3f> point_list;
		point_list.push_back(door->start_point);
		point_list.push_back(door->end_point);
		point_list.push_back(Point2);
		point_list.push_back(Point1);
		auto box_door = ComMath::GetVertsBoundingBox(point_list);
		//���ڿ�
		//���������������ĵ�
		float dist = ComMath::getTwoPointDistance(beg, centre);
		float dist2 = ComMath::getTwoPointDistance(end, centre);
		if (dist < dist2) {
			Point3f direction = ComMath::Normalize(end - beg);
			while (MinimumCabinetLength < ComMath::getTwoPointDistance(beg, end))
			{
				auto cabinet_beg = beg + wall_direction * cabinetDeep;
				auto cabinet_end = beg + wall_direction * cabinetDeep;
				std::vector<Point3f> point_list_door;
				point_list_door.push_back(cabinet_beg);
				point_list_door.push_back(cabinet_end);
				point_list_door.push_back(end);
				point_list_door.push_back(beg);
				if (ComMath::IsCollisionRect(point_list, point_list_door))
					beg = beg + direction * 20;
				else
					break;
			}
		}
		else
		{
			Point3f direction = ComMath::Normalize(beg - end);
			while (MinimumCabinetLength < ComMath::getTwoPointDistance(beg, end))
			{
				auto cabinet_beg = beg + wall_direction * cabinetDeep;
				auto cabinet_end = beg + wall_direction * cabinetDeep;
				std::vector<Point3f> point_list_door;
				point_list_door.push_back(cabinet_beg);
				point_list_door.push_back(cabinet_end);
				point_list_door.push_back(end);
				point_list_door.push_back(beg);
				if (ComMath::IsCollisionRect(point_list, point_list_door))
					end = end + direction * 20;
				else
					break;
			}
		}
	}
	if (MinimumCabinetLength < ComMath::getTwoPointDistance(beg, end))
	{
		CabinetWall cabinet_wall;
		cabinet_wall.p_wall = wall;
		cabinet_wall.cabinetBeg.push_back(beg);
		cabinet_wall.cabinetEnd.push_back(end);
		cabinet_wall.subLayout.push_back(Container());
		m_cabinet_wall.push_back(std::move(cabinet_wall));
	}
}
/*
void KitchenDesign::WallWithoutDoorCollision(Wall*wall,Door*door, SingleCase*single_case) {
	auto beg = wall->start_corner.point;
	auto end = wall->end_corner.point;
	if (door) {
		std::vector<Point3f>	Polygon;
		for (auto &ref : single_case->corner_list) {
			Polygon.push_back(ref.point);
		}
		auto door_extend = (door->start_point + door->end_point)*0.5f + door->direction*10.f;
		auto inside = ComMath::PointInPolygon(door_extend, Polygon);
		//
		CabinetWall Wall;
		auto centre = (door->start_point + door->end_point)*0.5;
		auto min_1 = ComMath::getTwoPointDistance(centre, beg);
		auto min_2 = ComMath::getTwoPointDistance(centre, end);
		float Min = FLT_MAX;
		Min = min(min_1, Min);
		Min = min(min_2, Min);
		if (Min < (float)door->length + 10.f)
			if (Min == min_1)
				beg = beg + ComMath::Normalize(end - beg)*((float)door->length + 10.f);
			else if (Min == min_2)
				end = end + ComMath::Normalize(beg - end)*(float)door->length;

		auto dist = ComMath::getTwoPointDistance(beg, end);
		if (dist > MinWallLength) {
			Wall.p_wall = wall;
			Wall.cabinetBeg.push_back(beg);
			Wall.cabinetEnd.push_back(end);
			Wall.subLayout.push_back(Container());
			m_cabinet_wall.push_back(std::move(Wall));
		}

		/*
		if (inside&&door->door_type == D_NORMAL) {
			CabinetWall Wall;
			auto centre = (door->start_point + door->end_point)*0.5;
			auto min_1 = ComMath::getTwoPointDistance(centre, beg);
			auto min_2 = ComMath::getTwoPointDistance(centre, end);
			float Min = FLT_MAX;
			Min = min(min_1, Min);
			Min = min(min_2, Min);
			if (Min < (float)door->length + 10.f)
				if (Min == min_1)
					beg = beg + ComMath::Normalize(end - beg)*((float)door->length + 10.f);
				else if (Min == min_2)
					end = end + ComMath::Normalize(beg - end)*(float)door->length;

			auto dist = ComMath::getTwoPointDistance(beg, end);
			if (dist > MinWallLength) {
				Wall.p_wall = wall;
				Wall.cabinetBeg.push_back(beg);
				Wall.cabinetEnd.push_back(end);
				Wall.subLayout.push_back(Container());
				m_cabinet_wall.push_back(std::move(Wall));
			}
		}
		else
		{
			CabinetWall Wall;
			Wall.p_wall = wall;
			Wall.cabinetBeg.push_back(beg);
			Wall.cabinetEnd.push_back(end);
			Wall.subLayout.push_back(Container());
			m_cabinet_wall.push_back(std::move(Wall));
		}

	}
}
*/
void KitchenDesign::ShrinkCabinetWall(SingleCase*single_case) {
	//��ʱ�ṹ��������¼�Ѿ�ή����ǽ��
	struct Shrink_info
	{
		CabinetWall*	wall1;
		CabinetWall*	wall2;
		bool					bshrink;
	};
	//�ҵ����õ�ǽ�ǣ�����ǽ��֮�䣩
	std::unordered_map<Corner*, Shrink_info >shrink_info;
	for (auto &corner : single_case->corner_list)
	{
		std::vector<CabinetWall*> set;
		for (auto &cabinet_wall : m_cabinet_wall) {
			bool in_corner = false;
			if (ComMath::comTwoPoint3f(corner.point, cabinet_wall.p_wall->start_corner.point))
				in_corner = true;
			if (ComMath::comTwoPoint3f(corner.point, cabinet_wall.p_wall->end_corner.point))
				in_corner = true;
			if (in_corner) {
				set.push_back(&cabinet_wall);
			}
		}
		if (set.size() == 2) {
			Shrink_info info;
			info.wall1 = set[0];
			info.wall2 = set[1];
			info.bshrink = false;
			set[0]->Corner.push_back(&corner);
			set[1]->Corner.push_back(&corner);
			shrink_info.insert(std::make_pair(&corner, info));
		}
	}
	//ή��һ��ǽ�Ĺ��壬һ���������ȣ���Чǽ��ͬʱ����

	for (auto &cabinetWall : m_cabinet_wall) {
		for (size_t i = 0; i < cabinetWall.cabinetBeg.size(); ++i) {
			Point3f beg = cabinetWall.cabinetBeg[i];
			Point3f end = cabinetWall.cabinetEnd[i];
			//�ٴδ˴��ж�ǽ�ǵĵ㣬һ��ǽ����ĳ��Ȼ��ȥ�������ȣ�һ������
			for (size_t j = 0; j < cabinetWall.Corner.size(); ++j) {
				if (ComMath::comTwoPoint3f(beg, cabinetWall.Corner[j]->point)) {
					auto normal = ComMath::Normalize(end - beg);
					beg = beg + normal * cabinetDeep;
				}
				if (ComMath::comTwoPoint3f(end, cabinetWall.Corner[j]->point)) {
					auto normal = ComMath::Normalize(beg - end);
					end = end + normal * cabinetDeep;
				}
				auto iter = shrink_info.find(cabinetWall.Corner[j]);

				auto other_wall = iter->second.wall1 == &cabinetWall ? iter->second.wall2 : iter->second.wall1;
				auto dist = ComMath::getTwoPointDistance(other_wall->p_wall->start_corner.point, other_wall->p_wall->end_corner.point);
				auto dist2 = ComMath::getTwoPointDistance(cabinetWall.p_wall->start_corner.point, cabinetWall.p_wall->end_corner.point);

				if (dist2 < dist) {
					cabinetWall.cabinetBeg[i] = beg;
					cabinetWall.cabinetEnd[i] = end;
				}
			}
			//��Ч���ֳ��ȣ�����ǽ�嶼����
			cabinetWall.furnitureBeg.push_back(beg);
			cabinetWall.furnitureEnd.push_back(end);
		}
	}
}

void KitchenDesign::CalculateCabinetWallDirection(SingleCase*single_case) {
	//����ǽ�ķ���ָ��������ָ�򷿼䴹ֱ����ǽ�ķ���
	std::vector<Point3f> polygon;
	for (auto &cor : single_case->corner_list) {
		polygon.push_back(cor.point);
	}
	for (auto &canbinet : m_cabinet_wall)
	{
		//����ǽ����ת90*�������0
		auto beg = canbinet.p_wall->start_corner.point - canbinet.p_wall->end_corner.point;
		auto centre = (canbinet.p_wall->start_corner.point + canbinet.p_wall->end_corner.point)*0.5f;
		auto rotate = 90.f / 360.f*6.283185307f;
		auto x = cosf(rotate)*beg.x - sinf(rotate)*beg.y;
		auto y = sinf(rotate)*beg.x + cosf(rotate)*beg.y;
		auto z = canbinet.p_wall->end_corner.point.z;
		auto direction = Point3f(x, y, z);
		direction = ComMath::Normalize(direction);
		//ǽ���е���Ϸ���������ڶ�����ڣ��������Ϊǽ�ķ���
		auto point = direction + centre;
		if (ComMath::PointInPolygon(point, polygon))
			canbinet.direction = direction;
		else
			canbinet.direction = -direction;
	}
}

void KitchenDesign::FindMaxWall(CabinetWall**Outwall, size_t* index, float *length, std::vector<CabinetWall>&Case)
{
	//�������ǽ��
	for (auto &wall : Case) {
		for (size_t i = 0; i < wall.furnitureBeg.size(); ++i) {
			if (wall.subLayout[i].furnitureType.size())
				continue;
			auto dist = ComMath::getTwoPointDistance(wall.furnitureBeg[i], wall.furnitureEnd[i]);
			if (dist > *length) {
				*length = dist;
				*Outwall = &wall;
				*index = i;
			}
		}
	}
}

void KitchenDesign::FindCompatible(CabinetWall**Outwall, size_t *index, std::vector<CabinetWall>&Case, float min, float max) {
	for (auto &wall : Case) {
		for (size_t i = 0; i < wall.furnitureBeg.size(); ++i) {
			if (wall.subLayout[i].furnitureType.size())
				continue;
			auto dist = ComMath::getTwoPointDistance(wall.furnitureBeg[i], wall.furnitureEnd[i]);
			if (min<dist&&max>dist) {
				*Outwall = &wall;
				*index = i;
			}
		}
	}
}

void KitchenDesign::CaseLayoutWithWindows() {
	//��a��Ѱ�Ҵ������ڵ�ǽ�����wall_a,���wall_a<600,���Դ˹���
	float length_a = 0.f;
	size_t index_a = 0;
	CabinetWall*wall_a = nullptr;
	auto temp_case = m_cabinet_wall;
	for (auto &wall : temp_case) {
		if (wall.p_wall->window_list.size()) {
			for (size_t i = 0; i < wall.furnitureBeg.size(); ++i) {
				auto dist = ComMath::getTwoPointDistance(wall.furnitureBeg[i], wall.furnitureEnd[i]);
				if (dist > length_a&&dist > 60.f) {
					length_a = dist;
					wall_a = &wall;
					index_a = i;
				}
			}
		}
	}
	if (!wall_a)//wall_a<600
		return;
	/*
	��b�����wall_a > 600����wall_a < 1200, ��wall_a�ϲ���ˮ�ۣ�ͬʱѰ��wall_a��������Чǽ����wall_b,
			 ���wall_b < 1350, ���Դ˹������wall_b>1350�ڴ�ǽ���ϲ������ + ���ͣ�
   */
	if (length_a >= 60.f && length_a < 120.f) {
		wall_a->subLayout[index_a].furnitureType.push_back(E_Water_Solt);
		auto length_b = -FLT_MAX;
		CabinetWall* wall_b = nullptr;
		size_t index_b = 0;
		FindMaxWall(&wall_b, &index_b, &length_b, temp_case);
		if (length_b > 135.f) {
			wall_b->subLayout[index_b].furnitureType.push_back(E_Predinner);
			wall_b->subLayout[index_b].furnitureType.push_back(E_Cooker);
			OneKitchenCase one_case;
			one_case.real_case = std::move(temp_case);
			one_case.regular_description = Windows_Kitchen_Case;
			m_all_case.push_back(std::move(one_case));
		}
	}
	/*
	��c�����wall_a > 1200����wall_a < 2150, ��wall_a�ϲ���ˮ�� + ���ͣ�
			ͬʱѰ��wall_a��������Чǽ����wall_b, ���wall_b < 750, ���Դ˹������wall_b>750�ڴ�ǽ���ϲ�����ߣ�
	*/
	if (length_a >= 120.f&&length_a < 215.f) {
		wall_a->subLayout[index_a].furnitureType.push_back(E_Water_Solt);
		wall_a->subLayout[index_a].furnitureType.push_back(E_Predinner);
		auto length_b = -FLT_MAX;
		CabinetWall* wall_b = nullptr;
		size_t index_b = 0;
		FindMaxWall(&wall_b, &index_b, &length_b, temp_case);
		if (length_b > 75.f) {
			wall_b->subLayout[index_b].furnitureType.push_back(E_Cooker);
			OneKitchenCase one_case;
			one_case.real_case = std::move(temp_case);
			one_case.regular_description = Windows_Kitchen_Case;
			m_all_case.push_back(std::move(one_case));
		}
	}
	/*
	��d�����wall_a>2150,��wall_a�ϲ���ˮ��+����+��ߣ����ֲ���˳��
	*/
	if (length_a > 215.f) {
		wall_a->subLayout[index_a].furnitureType.push_back(E_Water_Solt);
		wall_a->subLayout[index_a].furnitureType.push_back(E_Predinner);
		wall_a->subLayout[index_a].furnitureType.push_back(E_Cooker);
		OneKitchenCase one_case;
		one_case.real_case = std::move(temp_case);
		one_case.regular_description = Windows_Kitchen_Case;
		m_all_case.push_back(std::move(one_case));
	}
}

void KitchenDesign::CaseLayoutWithMaxWall() {

	auto temp_case = m_cabinet_wall;
	float length_a = 0.f;
	size_t index_a = 0;
	CabinetWall*wall_a = nullptr;
	FindMaxWall(&wall_a, &index_a, &length_a, temp_case);
	/*
		(a��
			Ѱ�����ǽ�壬����wall_a,���wall_a<750,���Դ˹���
	*/
	if (length_a < 60.f)//wall_a<600
		return;
	/*
	��b��
			���wall_a>750����wall_a<1200,��wall_a�ϲ�����ߣ�
			 ���wall_b<600,���Դ˹������wall_b>600�ڴ�ǽ���ϲ�����ߣ�
			 ���wall_c,���wall_c<400,���Դ˹������wall_c>400�ڴ�ǽ���ϲ��ֱ��ͣ�
	*/
	if (length_a > 75.f&&length_a < 120.f) {
		wall_a->subLayout[index_a].furnitureType.push_back(E_Cooker);

		float length_b = 0.f;
		size_t index_b = 0;
		CabinetWall*wall_b = nullptr;
		FindMaxWall(&wall_b, &index_b, &length_b, temp_case);
		if (length_b > 60.f) {
			wall_b->subLayout[index_b].furnitureType.push_back(E_Water_Solt);

			CabinetWall*wall_c = nullptr;
			float length_c = 0.f;
			size_t index_c = 0;

			FindMaxWall(&wall_c, &index_c, &length_c, temp_case);
			if (length_c > 40.f) {
				wall_c->subLayout[index_c].furnitureType.push_back(E_Predinner);
				OneKitchenCase one_case;
				one_case.real_case = std::move(temp_case);
				one_case.regular_description = MaxWall_Kitchen_Case;
				m_all_case.push_back(std::move(one_case));
			}
		}
	}
	/*
		c�����wall_a����1200����wall_aС��1350����wall_a�ϲ���ˮ��+���ͣ�
			 ͬʱѰ��wall_a��������Чǽ����wall_b,���wall_b<750,
			 ���Դ˹������wall_b>750�ڴ�ǽ���ϲ�����ߣ�
	*/
	if (length_a >= 120.f&&length_a < 135.f) {
		wall_a->subLayout[index_a].furnitureType.push_back(E_Water_Solt);
		wall_a->subLayout[index_a].furnitureType.push_back(E_Predinner);

		float length_b = 0.f;
		size_t index_b = 0;
		CabinetWall*wall_b = nullptr;
		FindMaxWall(&wall_b, &index_b, &length_b, temp_case);

		if (length_b > 75.f) {
			wall_b->subLayout[index_b].furnitureType.push_back(E_Cooker);
			OneKitchenCase one_case;
			one_case.real_case = std::move(temp_case);
			one_case.regular_description = MaxWall_Kitchen_Case;
			m_all_case.push_back(std::move(one_case));
		}
	}
	/*
			d�����wall_a����1350����wall_aС��2150����wall_a�ϲ������+���ͣ�
			 ͬʱѰ��wall_a��������Чǽ����wall_b,���wall_b<600,���Դ˹���
			 ���wall_b>600�ڴ�ǽ���ϲ���ˮ�ۣ�
	*/
	if (length_a >= 135.f&&length_a < 215.f) {
		wall_a->subLayout[index_a].furnitureType.push_back(E_Cooker);
		wall_a->subLayout[index_a].furnitureType.push_back(E_Predinner);


		float length_b = 0.f;
		size_t index_b = 0;
		CabinetWall*wall_b = nullptr;
		FindMaxWall(&wall_b, &index_b, &length_b, temp_case);
		if (length_b > 60.f) {
			wall_b->subLayout[index_b].furnitureType.push_back(E_Water_Solt);
			OneKitchenCase one_case;
			one_case.real_case = std::move(temp_case);
			one_case.regular_description = MaxWall_Kitchen_Case;
			m_all_case.push_back(std::move(one_case));
		}
	}
	/*
			e�����wall_a>2150,��wall_a�ϲ���ˮ��+����+��ߣ����ֲ���˳��
	*/
	if (length_a >= 215.f) {
		wall_a->subLayout[index_a].furnitureType.push_back(E_Water_Solt);
		wall_a->subLayout[index_a].furnitureType.push_back(E_Predinner);
		wall_a->subLayout[index_a].furnitureType.push_back(E_Cooker);
		OneKitchenCase one_case;
		one_case.real_case = std::move(temp_case);
		one_case.regular_description = MaxWall_Kitchen_Case;
		m_all_case.push_back(std::move(one_case));
	}
}

void KitchenDesign::CaseLayoutWithCoorker() {
	/*
	��3�������Ϊ��׼���в���
			ѡ����Чǽ��wall_a,wall_a���㳤�ȴ���750��
			����wall_a����С��1350����wall_a���沼����ߣ�
			ͬʱѰ��wall_a��������Чǽ����wall_b,
			���wall_b<1200,���Դ˹���
			���wall_b>1200�ڴ�ǽ���ϲ���ˮ��+���ͣ�
	*/
	auto temp_case = m_cabinet_wall;
	float length_a = 0.f;
	size_t index_a = 0;
	CabinetWall*wall_a = nullptr;
	FindCompatible(&wall_a, &index_a, temp_case, 75.f, 135.f);
	if (!wall_a)
		return;
	wall_a->subLayout[index_a].furnitureType.push_back(E_Cooker);

	float length_b = 0.f;
	size_t index_b = 0;
	CabinetWall*wall_b = nullptr;
	FindMaxWall(&wall_b, &index_b, &length_b, temp_case);
	if (length_b >= 120.f) {
		wall_b->subLayout[index_b].furnitureType.push_back(E_Predinner);
		wall_b->subLayout[index_b].furnitureType.push_back(E_Water_Solt);
		OneKitchenCase one_case;
		one_case.real_case = std::move(temp_case);
		one_case.regular_description = Coorker_Kitchen_Case;
		m_all_case.push_back(std::move(one_case));
	}
}

void KitchenDesign::CompatibleLayout(SingleCase*single_case) {
	bool find = false;
	for (auto model : single_case->model_list) {
		if (model.id == E_Fridge)
			find = true;
	}
	if (find)
		for (auto &Ref : m_all_case)
			CompatibleLayoutWithFridge(Ref, single_case);
	SolveAllFruniturePostion(single_case);
}

void KitchenDesign::CompatibleLayoutWithFridge(OneKitchenCase &Case, SingleCase*single_case) {
	//�ҵ����ʵı����λ,�ڹ���Ľ����㣬���߿�ʼ��
	std::vector<CabinetWall*>	cab_wall;
	std::vector<size_t>				sub_index;
	for (auto &ref : Case.real_case) {
		if (ref.Corner.size() == 2 && ref.cabinetBeg.size() == 1)
			continue;
		for (size_t i = 0; i < ref.furnitureBeg.size(); ++i)
		{
			auto &beg = ref.furnitureBeg[i];
			auto &end = ref.furnitureEnd[i];
			auto length = CalculateMinimumLength(ref.subLayout[i].furnitureType);
			if (ComMath::getTwoPointDistance(beg, end) - length >= fridge_length) {
				cab_wall.push_back(&ref);
				sub_index.push_back(i);
			}
		}
	}


	if (!cab_wall.size())
		return;

	Door*door = nullptr;
	for (auto &wall : single_case->wall_list) {
		if (wall.door_list.size())
			door = &wall.door_list[0];
	}
	if (!door)
		return;
	float Min = FLT_MAX;
	auto centre = door->start_point + door->start_point;
	centre = centre * 0.5;
	CabinetWall*Wall = NULL;
	size_t Index = 0;
	for (auto i = 0; i < cab_wall.size(); ++i) {
		auto index = sub_index[i];
		auto &beg = cab_wall[i]->cabinetBeg[index];
		auto &end = cab_wall[i]->cabinetBeg[index];
		//�޹�������
		if (cab_wall[i]->Corner.size() == 0) {
			float dist = ComMath::getTwoPointDistance(centre, beg);
			float dist1 = ComMath::getTwoPointDistance(centre, end);
			dist = min(dist, dist1);
			if (dist < Min) {
				Index = index;
				Wall = cab_wall[i];
				Min = dist;
			}
		}
		else
		{
			float length = 0.f;
			float dist = ComMath::getTwoPointDistance(cab_wall[i]->Corner[0]->point, beg);
			float dist1 = ComMath::getTwoPointDistance(cab_wall[i]->Corner[0]->point, end);
			if (dist < dist1)
				length = ComMath::getTwoPointDistance(end, centre);
			else
				length = ComMath::getTwoPointDistance(beg, centre);

			if (length < Min) {
				Index = index;
				Wall = cab_wall[i];
				Min = length;
			}
		}
	}

	//�޹�����,�����������
	if (!Wall->Corner.size())
	{
		auto dist = ComMath::getTwoPointDistance(centre, Wall->cabinetBeg[Index]);
		auto dist1 = ComMath::getTwoPointDistance(centre, Wall->cabinetEnd[Index]);
		if (dist < dist1) {
			Wall->subLayout[Index].fridgeType.insert(E_Fridge);
			Wall->subLayout[Index].fridgeBeg = Wall->cabinetBeg[Index];
			auto normal = ComMath::Normalize(Wall->cabinetEnd[Index] - Wall->cabinetBeg[Index]);
			Wall->cabinetBeg[Index] = Wall->cabinetBeg[Index] + normal * fridge_length;
			Wall->subLayout[Index].fridgeEnd = Wall->cabinetBeg[Index];
			Wall->furnitureBeg[Index] = Wall->furnitureBeg[Index] + normal * fridge_length;
		}
		else
		{
			Wall->subLayout[Index].fridgeType.insert(E_Fridge);
			Wall->subLayout[Index].fridgeBeg = Wall->cabinetEnd[Index];
			auto normal = ComMath::Normalize(Wall->cabinetBeg[Index] - Wall->cabinetEnd[Index]);
			Wall->cabinetEnd[Index] = Wall->cabinetEnd[Index] + normal * fridge_length;
			Wall->subLayout[Index].fridgeEnd = Wall->cabinetEnd[Index];
			Wall->cabinetEnd[Index] = Wall->cabinetEnd[Index] + normal * fridge_length;
		}
	}
	else
	{
		float dist = ComMath::getTwoPointDistance(Wall->Corner[0]->point, Wall->cabinetBeg[Index]);
		float dist1 = ComMath::getTwoPointDistance(Wall->Corner[0]->point, Wall->cabinetEnd[Index]);
		if (dist < dist1)
		{
			Wall->subLayout[Index].fridgeType.insert(E_Fridge);
			//cabinetBeg ���ܶ�
			Wall->subLayout[Index].fridgeBeg = Wall->cabinetEnd[Index];
			auto normal = ComMath::Normalize(Wall->cabinetBeg[Index] - Wall->cabinetEnd[Index]);
			Wall->cabinetEnd[Index] = Wall->cabinetEnd[Index] + normal * fridge_length;
			Wall->subLayout[Index].fridgeEnd = Wall->cabinetEnd[Index];
			Wall->furnitureEnd[Index] = Wall->furnitureEnd[Index] + normal * fridge_length;
		}
		else
		{
			//cabinetEnd ���ܶ�
			Wall->subLayout[Index].fridgeType.insert(E_Fridge);
			Wall->subLayout[Index].fridgeBeg = Wall->cabinetBeg[Index];
			auto normal = ComMath::Normalize(Wall->cabinetEnd[Index] - Wall->cabinetBeg[Index]);
			Wall->cabinetBeg[Index] = Wall->cabinetBeg[Index] + normal * fridge_length;
			Wall->subLayout[Index].fridgeEnd = Wall->cabinetBeg[Index];
			Wall->furnitureBeg[Index] = Wall->furnitureBeg[Index] + normal * fridge_length;
		}
	}
}

void KitchenDesign::SolveAllFruniturePostion(SingleCase*single_case) {

	for (auto &ref_case : m_all_case) {
		for (auto &Wall : ref_case.real_case) {
			for (size_t i = 0; i < Wall.furnitureBeg.size(); ++i)
			{
				auto &beg = Wall.furnitureBeg[i];
				auto &end = Wall.furnitureEnd[i];
				auto normal = ComMath::Normalize(end - beg);
				if (Wall.subLayout[i].furnitureType.size() == 0)
					continue;
				//�����Ч�߳����������� + ��ʵ���ֳ��ȣ���ʵ���ֱ��Ҿ� ƽ������
				auto require_max = CalculateMaximumLength(Wall.subLayout[i].furnitureType);
				auto dist = ComMath::getTwoPointDistance(beg, end);
				bool max = require_max < dist;
				if (max)
				{
					dist = (dist - require_max) / 2.f;
					auto _beg = beg + normal * dist;
					auto _interval = CalculateMaxRealLength(Wall.subLayout[i].furnitureType);
					_interval = (require_max - _interval) / (Wall.subLayout[i].furnitureType.size() + 1);
					_beg = _beg + normal * _interval;
					for (auto type : Wall.subLayout[i].furnitureType) {
						Wall.subLayout[i].furnitureBeg.push_back(_beg);
						_beg = _beg + normal * CalculateMaximumLength(type);
						Wall.subLayout[i].furnitureEnd.push_back(_beg);
						_beg = _beg + normal * _interval;
					}
				}
				else
				{
					require_max = CalculateMinimumLength(Wall.subLayout[i].furnitureType);
					dist = (dist - require_max) / 2.f;
					auto _beg = beg + normal * dist;
					auto _interval = CalculateMinRealLength(Wall.subLayout[i].furnitureType);
					_interval = (require_max - _interval) / (Wall.subLayout[i].furnitureType.size() + 1);
					_beg = _beg + normal * _interval;
					for (auto type : Wall.subLayout[i].furnitureType) {
						Wall.subLayout[i].furnitureBeg.push_back(_beg);
						_beg = _beg + normal * CalculateMinimumLength(type);
						Wall.subLayout[i].furnitureEnd.push_back(_beg);
						_beg = _beg + normal * _interval;
					}
				}
			}
		}
	}
}

float KitchenDesign::CalculateMinimumLength(std::vector<FurnitureType>&Ref) {
	float length = 0.f;
	if (!Ref.size())
		return  length;
	if (Ref.size() == 1) {
		switch (Ref[0])
		{
		case KitchenDesign::E_Predinner:
			length = per_dinner_length;
			break;
		case KitchenDesign::E_Water_Solt:
			length = water_slot_length_min;
			break;
		case KitchenDesign::E_Cooker:
			length = cooker_length_max;
			break;
		case KitchenDesign::E_Fridge:
			length = fridge_length;
			break;
			break;
		default:
			break;
		}
		return length;
	}
	if (Ref.size() == 2) {
		if (Ref[0] == E_Water_Solt || Ref[1] == E_Water_Solt) {
			//2������,����������չ����
			return per_dinner_length + water_slot_length_min + extend_length * 2;
		}
		if (Ref[0] == E_Cooker || Ref[1] == E_Cooker) {
			//2������,����������չ����
			return cooker_length_max + per_dinner_length + extend_length * 2;
		}
	}
	if (Ref.size() == 3) {
		//3������,����������չ����
		return water_slot_length_min + per_dinner_length + cooker_length_max + extend_length * 2;
	}
	return length;
}

float KitchenDesign::CalculateMaximumLength(std::vector<FurnitureType>&Ref) {
	float length = 0.f;
	if (!Ref.size())
		return  length;
	if (Ref.size() == 1) {
		switch (Ref[0])
		{
		case KitchenDesign::E_Predinner:
			length = per_dinner_length;
			break;
		case KitchenDesign::E_Water_Solt:
			length = water_slot_length_max;
			break;
		case KitchenDesign::E_Cooker:
			length = cooker_length_max;
			break;
		case KitchenDesign::E_Fridge:
			length = fridge_length;
			break;
		default:
			break;
		}
		return length;
	}
	if (Ref.size() == 2) {
		if (Ref[0] == E_Water_Solt || Ref[1] == E_Water_Solt) {
			//2������,����������չ����,���б�����
			return water_slot_length_max + per_dinner_length + extend_length * 2;
		}
		if (Ref[0] == E_Cooker || Ref[1] == E_Cooker) {
			//2������,����������չ����,���б�����
			return cooker_length_max + per_dinner_length + extend_length * 2;
		}
	}
	if (Ref.size() == 3) {
		//3������,����������չ����
		return water_slot_length_max + per_dinner_length + cooker_length_max + extend_length * 2;
	}
	return length;
}

float KitchenDesign::CalculateMinimumLength(FurnitureType Ref) {
	float length = 0.f;

	switch (Ref)
	{
	case KitchenDesign::E_Predinner:
		length = per_dinner_length;
		break;
	case KitchenDesign::E_Water_Solt:
		length = water_slot_length_min;
		break;
	case KitchenDesign::E_Cooker:
		length = cooker_length_max;
		break;
	case KitchenDesign::E_Fridge:
		length = fridge_length;
		break;
	default:
		break;
	}
	return length;
}

float KitchenDesign::CalculateMaximumLength(FurnitureType Ref) {
	float length = 0.f;

	switch (Ref)
	{
	case KitchenDesign::E_Predinner:
		length = per_dinner_length;
		break;
	case KitchenDesign::E_Water_Solt:
		length = water_slot_length_max;
		break;
	case KitchenDesign::E_Cooker:
		length = cooker_length_max;
		break;
	case KitchenDesign::E_Fridge:
		length = fridge_length;
		break;
	default:
		break;
	}
	return length;
}

float KitchenDesign::CalculateMinRealLength(std::vector<FurnitureType>&Ref) {
	float length = 0.f;
	for (auto type : Ref) {
		switch (type)
		{
		case KitchenDesign::E_Predinner:
			length += per_dinner_length;
			break;
		case KitchenDesign::E_Water_Solt:
			length += water_slot_length_min;
			break;
		case KitchenDesign::E_Cooker:
			length += cooker_length_max;
			break;
		case KitchenDesign::E_Fridge:
			length += fridge_length;
			break;
		default:
			break;
		}
	}
	return length;
}

float KitchenDesign::CalculateMaxRealLength(std::vector<FurnitureType>&Ref) {
	float length = 0.f;
	for (auto type : Ref) {
		switch (type)
		{
		case KitchenDesign::E_Predinner:
			length += per_dinner_length;
			break;
		case KitchenDesign::E_Water_Solt:
			length += water_slot_length_max;
			break;
		case KitchenDesign::E_Cooker:
			length += cooker_length_max;
			break;
		case KitchenDesign::E_Fridge:
			length += fridge_length;
			break;
		default:
			break;
		}
	}
	return length;
}