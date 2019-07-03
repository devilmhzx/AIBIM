#include "RectangleExtractor.h"


std::vector<Point3f> RectangleExtractor::getMaximumInscribedRectangleOnPolygon(const std::vector<Point3f>&point_list)
{
	const float granularity = 1.f;
	struct InternelWall
	{
		Point3f start_point;
		Point3f end_point;
		Point3f direction;
	};
	struct MaximumRect
	{
		float	area;
		float	length;
		InternelWall*wall;
	};
	auto get_wall_direction = [&point_list](InternelWall*wall)
	{
		//鏃嬭浆
		Point3f direction = wall->end_point - wall->start_point;
		auto X = direction.x*cos(M_PI / 2) - sin(M_PI / 2) *direction.y;
		auto Y = direction.x*sin(M_PI / 2) - cos(M_PI / 2)*direction.y;
		wall->direction.x = X;
		wall->direction.y = Y;
		wall->direction.z = 0.f;

		direction = ComMath::Normalize(wall->direction);
		Point3f centre = (wall->end_point + wall->start_point)*0.5f;
		centre = centre + direction * 30.f;
		if (!ComMath::PointInPolygon(centre, point_list))
			wall->direction = -wall->direction;
		return;
	};
	std::vector<MaximumRect> rect_container;
	std::vector<InternelWall> internel_wall;
	//鏁翠綋澧欎綋,鍏辩嚎澧欎綋鍚堝苟,澧炲姞鎬ц兘
	{
		std::vector<InternelWall> temp_wall;
		for (int i = 0; i < point_list.size(); ++i)
		{
			int j = (i + 1) % point_list.size();
			InternelWall wall;
			wall.start_point = point_list[i];
			wall.end_point = point_list[j];
			get_wall_direction(&wall);
			temp_wall.push_back(wall);
		}

		for (int i = 0; i < temp_wall.size(); ++i)
		{
			for (int j = 0; j < temp_wall.size(); ++j)
			{
				if (i == j)
					continue;
				//鍒ゆ柇鏄笉鏄叡绾?
				float angle_pi = ComMath::GetAngleBetweenTwoLines(temp_wall[i].start_point - temp_wall[i].end_point,
					temp_wall[j].start_point - temp_wall[j].end_point);
				if (!ComMath::IsSameScalar(0.f, angle_pi, 0.4f)
					&& ComMath::IsSameScalar(0.f, M_PI, 0.4f))
				{
					continue;
				}
				Point3f proj = ComMath::getPointToLinePedal(temp_wall[i].start_point, temp_wall[j].start_point, temp_wall[j].end_point);
				float dist = ComMath::getTwoPointDistance(proj, temp_wall[i].start_point);
				if (ComMath::IsSameScalar(dist, 0.f, 1.f))
				{
					//鎵╁睍澧?
					//鑾峰彇鍏辩嚎鏈€闀跨殑澧欎綋
					InternelWall wall;
					float V1 = ComMath::getTwoPointDistance(temp_wall[i].start_point, temp_wall[j].start_point);
					float V2 = ComMath::getTwoPointDistance(temp_wall[i].start_point, temp_wall[j].end_point);
					float V3 = ComMath::getTwoPointDistance(temp_wall[i].end_point, temp_wall[j].start_point);
					float V4 = ComMath::getTwoPointDistance(temp_wall[i].end_point, temp_wall[j].end_point);
					float max_f = max(V1, V2);
					 max_f = max(max_f, V3);
					 max_f = max(max_f, V4);
					if (max_f == V1)
					{
						wall.start_point = temp_wall[i].start_point;
						wall.end_point = temp_wall[j].start_point;
					}
					else  if (max_f == V2)
					{
						wall.start_point = temp_wall[i].start_point;
						wall.end_point = temp_wall[j].end_point;
					}
					else if (max_f == V3)
					{
						wall.start_point = temp_wall[i].end_point;
						wall.end_point = temp_wall[j].start_point;
					}
					else
					{
						wall.start_point = temp_wall[i].end_point;
						wall.end_point = temp_wall[j].end_point;
					}
					wall.direction = temp_wall[i].direction;
					internel_wall.push_back(wall);
				}
			}
		}
		//鎻掑叆闈炲叡绾垮
		for (int i = 0; i < temp_wall.size(); ++i)
		{
			internel_wall.push_back(temp_wall[i]);
		}
	}
	//Horizontal scan
	for (int i = 0; i < internel_wall.size(); ++i)
	{
		Point3f start_point = internel_wall[i].start_point;
		Point3f end_point = internel_wall[i].end_point;
		Point3f &direction = internel_wall[i].direction;
		bool break_point_on_start = false;
		while (true)
		{

			start_point = start_point + direction * granularity;
			end_point = end_point + direction * granularity;
			bool start_in_polygon = ComMath::PointInPolygon(start_point, point_list);
			bool end_in_polygon = ComMath::PointInPolygon(end_point, point_list);
			//璧峰鐐规埅鏂?
			if (!start_in_polygon)
			{
				break_point_on_start = true;
				start_point = start_point - direction * granularity;
				break;
			}
			//缁堟鐐规埅鏂?
			if (!end_in_polygon)
			{
				end_point = end_point - direction * granularity;
				break;
			}
		}
		if (break_point_on_start)
		{
			MaximumRect rect;
			float width = ComMath::getTwoPointDistance(internel_wall[i].start_point, internel_wall[i].end_point);
			float length = ComMath::getTwoPointDistance(start_point, internel_wall[i].start_point);
			rect.area = width * length;
			rect.length = length;
			rect.wall = &internel_wall[i];
			rect_container.push_back(rect);
		}
		else
		{
			MaximumRect rect;
			float width = ComMath::getTwoPointDistance(internel_wall[i].start_point, internel_wall[i].end_point);
			float length = ComMath::getTwoPointDistance(end_point, internel_wall[i].end_point);
			rect.area = width * length;
			rect.length = length;
			rect.wall = &internel_wall[i];
			rect_container.push_back(rect);
		}
	}

	float area = 0.f;
	MaximumRect *rect = nullptr;
	for (int i = 0; i < rect_container.size(); ++i)
	{
		if (rect_container[i].area > area)
		{
			rect = &rect_container[i];
			area = rect_container[i].area;
		}
	}
	//generator rect
	std::vector<Point3f> rect_result;
	rect_result.push_back(rect->wall->start_point);
	rect_result.push_back(rect->wall->end_point);
	rect_result.push_back(rect->wall->end_point + rect->wall->direction* rect->length);
	rect_result.push_back(rect->wall->start_point + rect->wall->direction* rect->length);
	return rect_result;
}

std::vector<Point3f> RectangleExtractor::getMaximumInscribedRectangleOnPolygonPrecisely(const std::vector<Point3f>&point_list, float length)
{
	const float granularity = 1.f;
	struct InternelWall
	{
		Point3f start_point;
		Point3f end_point;
		Point3f direction;
	};
	struct MaximumRect
	{
		float	area;
		float	length;
		InternelWall*wall;
	};
	auto get_wall_direction = [&point_list](InternelWall*wall)
	{
		//鏃嬭浆
		Point3f direction = wall->end_point - wall->start_point;
		auto X = direction.x*cos(M_PI / 2) - sin(M_PI / 2) *direction.y;
		auto Y = direction.x*sin(M_PI / 2) - cos(M_PI / 2)*direction.y;
		wall->direction.x = X;
		wall->direction.y = Y;
		wall->direction.z = 0.f;

		direction = ComMath::Normalize(wall->direction);
		Point3f centre = (wall->end_point + wall->start_point)*0.5f;
		centre = centre + direction * 30.f;
		if (!ComMath::PointInPolygon(centre, point_list))
			wall->direction = -wall->direction;
		return;
	};
	std::vector<MaximumRect> rect_container;
	std::vector<InternelWall> internel_wall;
	//鏁翠綋澧欎綋,鍏辩嚎澧欎綋鍚堝苟,澧炲姞鎬ц兘
	{
		std::vector<InternelWall> temp_wall;
		for (int i = 0; i < point_list.size(); ++i)
		{
			int j = (i + 1) % point_list.size();
			InternelWall wall;
			wall.start_point = point_list[i];
			wall.end_point = point_list[j];
			get_wall_direction(&wall);
			temp_wall.push_back(wall);
		}

		for (int i = 0; i < temp_wall.size(); ++i)
		{
			for (int j = 0; j < temp_wall.size(); ++j)
			{
				if (i == j)
					continue;
				//鍒ゆ柇鏄笉鏄叡绾?
				float angle_pi =ComMath::GetAngleBetweenTwoLines(temp_wall[i].start_point - temp_wall[i].end_point,
					temp_wall[j].start_point - temp_wall[j].end_point);
				if (!ComMath::IsSameScalar(0.f, angle_pi, 0.4f)
					&& ComMath::IsSameScalar(0.f, M_PI, 0.4f))
				{
					continue;
				}
				Point3f proj = ComMath::getPointToLinePedal(temp_wall[i].start_point, temp_wall[j].start_point, temp_wall[j].end_point);
				float dist = ComMath::getTwoPointDistance(proj, temp_wall[i].start_point);
				if (ComMath::IsSameScalar(dist, 0.f, 1.f))
				{
					//鎵╁睍澧?
					//鑾峰彇鍏辩嚎鏈€闀跨殑澧欎綋
					InternelWall wall;
					float V1 = ComMath::getTwoPointDistance(temp_wall[i].start_point, temp_wall[j].start_point);
					float V2 = ComMath::getTwoPointDistance(temp_wall[i].start_point, temp_wall[j].end_point);
					float V3 = ComMath::getTwoPointDistance(temp_wall[i].end_point, temp_wall[j].start_point);
					float V4 = ComMath::getTwoPointDistance(temp_wall[i].end_point, temp_wall[j].end_point);
					float max_f = max(V1, V2);
					 max_f = max(max_f, V3);
					 max_f = max(max_f, V4);
					if (max_f == V1)
					{
						wall.start_point = temp_wall[i].start_point;
						wall.end_point = temp_wall[j].start_point;
					}
					else  if (max_f == V2)
					{
						wall.start_point = temp_wall[i].start_point;
						wall.end_point = temp_wall[j].end_point;
					}
					else if (max_f == V3)
					{
						wall.start_point = temp_wall[i].end_point;
						wall.end_point = temp_wall[j].start_point;
					}
					else
					{
						wall.start_point = temp_wall[i].end_point;
						wall.end_point = temp_wall[j].end_point;
					}
					wall.direction = temp_wall[i].direction;
					internel_wall.push_back(wall);
				}
			}
		}
		//鎻掑叆闈炲叡绾垮
		for (int i = 0; i < temp_wall.size(); ++i)
		{
			internel_wall.push_back(temp_wall[i]);
		}
	}
	//Horizontal scan
	for (int i = 0; i < internel_wall.size(); ++i)
	{
		Point3f start_point = internel_wall[i].start_point;
		Point3f end_point = internel_wall[i].end_point;
		Point3f &direction = internel_wall[i].direction;

		auto wall_length = ComMath::getTwoPointDistance(start_point, end_point);
		auto normal = ComMath::Normalize(end_point - start_point);
		std::vector<Point3f>	intersection_point(wall_length / length);
		for (int i = 0; i < intersection_point.size(); ++i)
		{

			if (i == 0)
				intersection_point[i] = start_point + normal * 0.1f;
			if (i == intersection_point.size() - 1)
				intersection_point[i] = end_point - normal * 0.1f;
			else
				intersection_point[i] = start_point + normal * i*length;
		}
		std::vector<Point3f>   start_scan_container = intersection_point;
		int index = -1;
		while (true)
		{

			start_point = start_point + direction * granularity;
			end_point = end_point + direction * granularity;

			for (int i = 0; i < intersection_point.size(); ++i)
			{
				intersection_point[i] = intersection_point[i] + direction * granularity;
				if (!ComMath::PointInPolygon(start_point, point_list)) {
					index = i;
					intersection_point[index] = intersection_point[i] - direction * granularity;
				}
			}
			if (-1 != index)
				break;
		}

		MaximumRect rect;
		float width = ComMath::getTwoPointDistance(internel_wall[i].start_point, internel_wall[i].end_point);
		float length = ComMath::getTwoPointDistance(start_scan_container[index], intersection_point[index]);
		rect.area = width * length;
		rect.length = length;
		rect.wall = &internel_wall[i];
		rect_container.push_back(rect);

	}

	float area = 0.f;
	MaximumRect *rect = nullptr;
	for (int i = 0; i < rect_container.size(); ++i)
	{
		if (rect_container[i].area > area)
		{
			rect = &rect_container[i];
			area = rect_container[i].area;
		}
	}
	//generator rect
	std::vector<Point3f> rect_result;
	rect_result.push_back(rect->wall->start_point);
	rect_result.push_back(rect->wall->end_point);
	rect_result.push_back(rect->wall->end_point + rect->wall->direction* rect->length);
	rect_result.push_back(rect->wall->start_point + rect->wall->direction* rect->length);
	return rect_result;
}
#define  granularity 20.f
std::vector<Point3f>  RectangleExtractor::ExtractMaximumRectangleBySingleCase(const std::vector<Point3f>&point_list)
{
	std::vector<Point3f> points;

	/*细分到网格*/
	if(!SplitToGird(point_list))
		return points;

	/*水平扫描*/
	HorizontalScaningWithBottomUp();

	/*水平扫描*/
	HorizontalScaningWithUpBottom();

	/*垂直扫描*/
	VerticalScaningLeftRight();

	/*垂直扫描*/
	VerticalScaningRightLeft();

	/*水平计算*/
	CalculateVaildRectBottomUp();

	/*水平计算*/
	CalculateVaildRectUpBottom();

	/*垂直计算*/
	CalculateVaildRectLeftRight();

	/*垂直计算*/
	CalculateVaildRectRightLeft();

	/*修正曲线*/
	std::vector<Point3f> point_rect= CalculteRectangleBoundary();

	return CorrectPolygon(point_rect, point_list);
}

bool  RectangleExtractor::SplitToGird(const std::vector<Point3f>&point_list)
{
	/**计算A-B-Box*/
	box_min = Point3f(FLT_MAX, FLT_MAX, FLT_MAX);
	box_max = Point3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (auto &point : point_list)
	{
		box_min = ComMath::Point3fMin(box_min, point);
		box_max = ComMath::Point3fMax(box_max, point);
	}
	/**保证不会错过第一次有效扫描点*/
	box_max = box_max + Point3f(granularity, granularity, 0.f)*2;
	box_min = box_min - Point3f(granularity, granularity, 0.f)*2;

	Point3f box_size = box_max - box_min;
	max_row = int(box_size.y / granularity);
	max_column = int(box_size.x / granularity);
	/**初始化节点*/
	node_base.clear();
	node_base.resize(max_row);
	 
	for (auto &iter : node_base)
	{
		iter.resize(max_column);
	}
	/**细分节点*/
	for (int i = 0; i < max_row; ++i)
	{
		for (int j = 0; j < max_column; ++j)
		{
			Point3f point = GridToWorld(i, j);
			if (ComMath::PointInPolygon(point, point_list))
			{
				/**标记为内部点*/
				node_base[i][j] = 1;
			}
			else
			{
				/**标记为外部点*/
				node_base[i][j] = 0;
			}
		}
	}
	return true;
}

Point3f RectangleExtractor::GridToWorld(int x, int y)
{
	Point3f out = Point3f(y*granularity, x*granularity, 0.f);
	out = out + box_min;
	return out;
}

bool    RectangleExtractor::IsGoodIndex(int x, int y)
{
	return x >= 0&&x < max_row&&y >= 0 && y <= max_column;
}

void  RectangleExtractor::WorldToGrid(Point3f point, int *x, int *y)
{
	point = point - box_min;
	*x = int(point.y / granularity);
	*y = int(point.x / granularity);
}

void    RectangleExtractor::HorizontalScaningWithBottomUp()
{
	/**初始化第一次扫描数据column*/
	std::vector<char>	scan_segment;
	scan_segment.reserve(max_column);
	for (int i = 0; i < max_column; ++i)
	{
		scan_segment.push_back(0);
	}

	for (int i = 0; i < max_row; ++i)
	{
		bool on_start = true;
		int    first_point=0;
		int    second_point=0;
		std::vector<std::pair<int, int>> diff_node;
		/**扫描一行的所有数据*/
		for (int j = 0; j < max_column; ++j)
		{		 
			if (node_base[i][j] != scan_segment[j]) {
				/**第一次不等,记录起点*/
				if (on_start) 
				{
					on_start = false;
					first_point = j;
				}
				/**更新原始点*/
				scan_segment[j] = node_base[i][j];
			}	 

			else
			{
				/**第二次不等,记录终点,添加到不同节点记录*/
				if (!on_start) {
					on_start = true;
					second_point = j-1;
					diff_node.push_back(std::make_pair(first_point, second_point));
				}
			}
		}
		/**处理与原始行不同的节点,扩展可能受影响的点*/
		std::vector<std::pair<int, int>> extend_node;
		 
		for (int j = 0; j < diff_node.size(); ++j)
		{
			int  index = node_base[i][diff_node[j].second] ? i : i - 1;
			int left_boundary = diff_node[j].first;
			int right_boundary = diff_node[j].second;
			bool  death_left = true;
			while (IsGoodIndex(index, left_boundary)&&node_base[index][left_boundary] != 0)
			{
				--left_boundary;
			}
			while (IsGoodIndex(index, right_boundary)&&node_base[index][right_boundary] != 0)
			{
				++right_boundary;
			}
			++left_boundary;
			--right_boundary;
			if (left_boundary != diff_node[j].first||right_boundary != diff_node[j].second)
			{
				extend_node.push_back(std::make_pair(left_boundary, right_boundary));
			}
		}
		/**全部加入有效扫描片段*/
		for (auto &ref : diff_node) {
			Segment segment;
			if(node_base[i][ref.second])
				segment.common = i;
			else
				segment.common = i-1;
			segment.i = ref.first;
			segment.j = ref.second;
			segment.scores = 0.f;
			horizontal_bottom_up_segments.push_back(segment);
		}
		for (auto &ref : extend_node)
		{
			Segment segment;
			if (node_base[i][ref.second])
				segment.common = i;
			else
				segment.common = i - 1;
			segment.i = ref.first;
			segment.j = ref.second;
			segment.scores = 0.f;
			horizontal_bottom_up_segments.push_back(segment);
		}
	}
}

void    RectangleExtractor::HorizontalScaningWithUpBottom() {
	/**初始化第一次扫描数据column*/
	std::vector<char>	scan_segment;
	scan_segment.reserve(max_column);
	for (int i = 0; i < max_column; ++i)
	{
		scan_segment.push_back(0);
	}

	for (int i = max_row-1; i >= 0 ; --i)
	{
		bool on_start = true;
		int    first_point = 0;
		int    second_point = 0;
		std::vector<std::pair<int, int>> diff_node;
		/**扫描一行的所有数据*/
		for (int j = 0; j < max_column; ++j)
		{
			if (node_base[i][j] != scan_segment[j]) {
				/**第一次不等,记录起点*/
				if (on_start)
				{
					on_start = false;
					first_point = j;
				}
				/**更新原始点*/
				scan_segment[j] = node_base[i][j];
			}
			else
			{
				/**第二次不等,记录终点,添加到不同节点记录*/
				if (!on_start)
				{
					on_start = true;
					second_point = j-1;
					diff_node.push_back(std::make_pair(first_point, second_point));
				}
			}
		}
		/**处理与原始行不同的节点,扩展可能受影响的点*/
		std::vector<std::pair<int, int>> extend_node;
		for (int j = 0; j < diff_node.size(); ++j)
		{
			int index = node_base[i][diff_node[j].second] ? i : i + 1;
			int left_boundary = diff_node[j].first;
			int right_boundary = diff_node[j].second;
			while (IsGoodIndex(index, left_boundary)&&node_base[index][left_boundary] != 0)
			{
				--left_boundary;
			}
			while (IsGoodIndex(index, right_boundary)&&node_base[index][right_boundary] != 0)
			{
				++right_boundary;
			}
			++left_boundary;
			--right_boundary;
			if (left_boundary != diff_node[j].first&&right_boundary != diff_node[j].second)
			{
				extend_node.push_back(std::make_pair(left_boundary, right_boundary));
			}
		}
		/**全部加入有效扫描片段*/
		for (auto &ref : diff_node) {
			Segment segment;
			segment.common = node_base[i][ref.second] ? i : i + 1;
			segment.i = ref.first;
			segment.j = ref.second;
			segment.scores = 0.f;
			horizontal_up_bottom_segments.push_back(segment);
		}
		for (auto &ref : extend_node)
		{
			Segment segment;
			segment.common = node_base[i][ref.second] ? i : i + 1;
			segment.i = ref.first;
			segment.j = ref.second;
			segment.scores = 0.f;
			horizontal_up_bottom_segments.push_back(segment);
		}
	}
	 
}

void    RectangleExtractor::VerticalScaningLeftRight()
{

	/**初始化第一次扫描数据column*/
	std::vector<char>	scan_segment;
	scan_segment.reserve(max_row);
	for (int i = 0; i < max_row; ++i)
	{
		scan_segment.push_back(0);
	}

	for (int i = 0; i < max_column; ++i)
	{
		bool on_start = true;
		int    first_point = 0;
		int    second_point = 0;
		std::vector<std::pair<int, int>> diff_node;
		/**扫描一行的所有数据*/
		for (int j = 0; j < max_row; ++j)
		{
			if (node_base[j][i] != scan_segment[j]) {
				/**第一次不等,记录起点*/
				if (on_start)
				{
					on_start = false;
					first_point = j;
				}
				 
				/**更新原始点*/
				scan_segment[j] = node_base[j][i];
			}
			/**第二次不等,记录终点,添加到不同节点记录*/
			else
			{
				if (!on_start) {
					on_start = true;
					second_point =j-1;
					diff_node.push_back(std::make_pair(first_point, second_point));
				}
			}
		}
		/**处理与原始行不同的节点,扩展可能受影响的点*/
		std::vector<std::pair<int, int>> extend_node;
		for (int j = 0; j < diff_node.size(); ++j)
		{
			int index = node_base[diff_node[j].second][i] ? i : i - 1;
			int left_boundary = diff_node[j].first;
			int right_boundary = diff_node[j].second;
			while (IsGoodIndex(left_boundary, index)&&node_base[left_boundary][index] != 0)
			{
				--left_boundary;
			}
			while (IsGoodIndex(right_boundary, index)&&node_base[right_boundary][index] != 0)
			{
				++right_boundary;
			}
			if (++left_boundary != diff_node[j].first&&--right_boundary != diff_node[j].second)
			{
				extend_node.push_back(std::make_pair(left_boundary, right_boundary));
			}
		}
		/**全部加入有效扫描片段*/
		for (auto &ref : diff_node) {
			Segment segment;
			segment.common = node_base[ref.second][i] ? i : i -1;
			segment.i = ref.first;
			segment.j = ref.second;
			segment.scores = 0.f;
			vertical_left_right_segments.push_back(segment);
		}
		for (auto &ref : extend_node)
		{
			Segment segment;
			segment.common = node_base[ref.second][i] ? i : i - 1;
			segment.i = ref.first;
			segment.j = ref.second;
			segment.scores = 0.f;
			horizontal_up_bottom_segments.push_back(segment);
		}
	}
}

void    RectangleExtractor::VerticalScaningRightLeft()
{
	/**初始化第一次扫描数据column*/
	std::vector<char>	scan_segment;
	scan_segment.reserve(max_row);
	for (int i = 0; i < max_row; ++i)
	{
		scan_segment.push_back(0);
	}

	for (int i = max_column-1; i>=0; --i)
	{
		bool same = true;
		bool on_start = true;
		int    first_point = 0;
		int    second_point = 0;
		std::vector<std::pair<int, int>> diff_node;
		/**扫描一行的所有数据*/
		for (int j = 0; j < max_row; ++j)
		{
			if (node_base[j][i] != scan_segment[j]) {
				same = false;
				/**第一次不等,记录起点*/
				if (on_start)
				{
					on_start = false;
					first_point = j;
				}			 
				/**更新原始点*/
				scan_segment[j] = node_base[j][i];
			}
			/**第二次不等,记录终点,添加到不同节点记录*/
			else
			{
				if (!on_start) {
					on_start = true;
					second_point = j-1;
					diff_node.push_back(std::make_pair(first_point, second_point));
				}
			}
		}
		/**处理与原始行不同的节点,扩展可能受影响的点*/
		std::vector<std::pair<int, int>> extend_node;
		for (int j = 0; j < diff_node.size(); ++j)
		{
			int index = node_base[diff_node[j].second][i] ? i : i - 1;
			int left_boundary = diff_node[j].first;
			int right_boundary = diff_node[j].second;
			while (IsGoodIndex(left_boundary, index)&&node_base[left_boundary][index] != 0)
			{
				--left_boundary;
			}
			while (IsGoodIndex(right_boundary, index)&&node_base[right_boundary][index] != 0)
			{
				++right_boundary;
			}
			if (++left_boundary != diff_node[j].first&&--right_boundary != diff_node[j].second)
			{
				extend_node.push_back(std::make_pair(left_boundary, right_boundary));
			}
		}
		/**全部加入有效扫描片段*/
		for (auto &ref : diff_node) {
			Segment segment;
			segment.common = node_base[ref.second][i] ? i : i - 1;
			segment.i = ref.first;
			segment.j = ref.second;
			segment.scores = 0.f;
			vertical_right_left_segments.push_back(segment);
		}
		for (auto &ref : extend_node)
		{
			Segment segment;
			segment.common = node_base[ref.second][i] ? i : i - 1;
			segment.i = ref.first;
			segment.j = ref.second;
			segment.scores = 0.f;
			horizontal_up_bottom_segments.push_back(segment);
		}
	}
}

void   RectangleExtractor::CalculateVaildRectBottomUp()
{
	for (auto&ref : horizontal_bottom_up_segments)
	{
		int range = 0;
		for (int i = ref.common; i < max_row; ++i)
		{
			bool inside = true;
			for (int j = ref.i; j <=ref.j; ++j)
			{
				if (!IsGoodIndex(i, j) || !node_base[i][j]) {
				 	inside = false;
				 	break;
				 }
			}
			if (!inside)
				break;
			++range;
		}
		ref.range = range;
		ref.scores = (float)(ref.j - ref.i)*range;
	}
}

void   RectangleExtractor::CalculateVaildRectUpBottom()
{
	for (auto&ref : horizontal_up_bottom_segments)
	{
		int range = 0;
		for (int i = ref.common;i>=0; --i)
		{
			bool inside = true;
			for (int j = ref.i; j <= ref.j; ++j)
			{
				if (!IsGoodIndex(i, j) || !node_base[i][j]) {
					inside = false;
					break;
				}
			}
			if (!inside)
				break;
			++range;
		}
		ref.range = range;
		ref.scores = (float)(ref.j - ref.i)*range;
	}
}

void   RectangleExtractor::CalculateVaildRectLeftRight()
{
	for (auto&ref : vertical_left_right_segments)
	{
		int range = 0;
		for (int i = ref.common; i <max_column; ++i)
		{
			bool inside = true;
			for (int j = ref.i; j <= ref.j; ++j)
			{
				if (!IsGoodIndex(j, i) || !node_base[j][i]) {
					inside = false;
					break;
				}
			}
			if (!inside)
				break;
			++range;
		}
		ref.range = range;
		ref.scores = (float)(ref.j - ref.i)*range;
	}
}

void   RectangleExtractor::CalculateVaildRectRightLeft()
{

	for (auto&ref : vertical_right_left_segments)
	{
		int range = 0;
		for (int i = ref.common; i >=0; --i)
		{
			bool inside = true;
			for (int j = ref.i; j <= ref.j; ++j)
			{
				if (!IsGoodIndex(j, i) || !node_base[j][i]) {
					inside = false;
					break;
				}
			}
			if (!inside)
				break;
			++range;
		}
		ref.range = range;
		ref.scores = (float)(ref.j - ref.i)*range;
	}
}

std::vector<Point3f> RectangleExtractor::CalculteRectangleBoundary()
{

	int index = -1;
	float max_score = -FLT_MAX;
	Segment segment;
	for (auto ref : horizontal_bottom_up_segments)
	{
		if (ref.scores > max_score)
		{
			max_score = ref.scores;
			segment = ref;
			index = 1;
		}
	}
	for (auto ref : horizontal_up_bottom_segments)
	{
		break;
		if (ref.scores > max_score)
		{
			max_score = ref.scores;
			segment = ref;
			index = 2;
		}
	}
	for (auto ref : vertical_left_right_segments)
	{
		break;
		if (ref.scores > max_score)
		{
			max_score = ref.scores;
			segment = ref;
			index = 3;
		}
	}
	for (auto ref : vertical_right_left_segments)
	{
		break;
		if (ref.scores > max_score)
		{
			max_score = ref.scores;
			segment = ref;
			index = 4;
		}
	}
	//horizontal_bottom_up_segments;
	//horizontal_up_bottom_segments;
	//vertical_left_right_segments;
	//vertical_right_left_segments;
	std::vector<Point3f>	 points_list;
	if (index == 1)
	{
		Point3f V1 = GridToWorld(segment.common, segment.i);
		Point3f V2 = GridToWorld(segment.common, segment.j);
		Point3f V3 = GridToWorld(segment.common+ segment.range, segment.j);
		Point3f V4 = GridToWorld(segment.common+ segment.range, segment.i);
		points_list.push_back(V1);
		points_list.push_back(V2);
		points_list.push_back(V3);
		points_list.push_back(V4);
	}
	else if(index==2)
	{
		Point3f V1 = GridToWorld(segment.common, segment.i);
		Point3f V2 = GridToWorld(segment.common, segment.j);
		Point3f V3 = GridToWorld(segment.common - segment.range, segment.j);
		Point3f V4 = GridToWorld(segment.common - segment.range, segment.i);
		points_list.push_back(V1);
		points_list.push_back(V2);
		points_list.push_back(V3);
		points_list.push_back(V4);
	}
	else if (index == 3)
	{
		Point3f V1 = GridToWorld(segment.i, segment.common);
		Point3f V2 = GridToWorld(segment.j, segment.common);
		Point3f V3 = GridToWorld(segment.j,segment.common + segment.range);
		Point3f V4 = GridToWorld(segment.i, segment.common + segment.range);
		points_list.push_back(V1);
		points_list.push_back(V2);
		points_list.push_back(V3);
		points_list.push_back(V4);
	}
	else if (index == 4)
	{
		Point3f V1 = GridToWorld(segment.i, segment.common);
		Point3f V2 = GridToWorld(segment.j, segment.common);
		Point3f V3 = GridToWorld(segment.j, segment.common - segment.range);
		Point3f V4 = GridToWorld(segment.i, segment.common - segment.range);
		points_list.push_back(V1);
		points_list.push_back(V2);
		points_list.push_back(V3);
		points_list.push_back(V4);
	}
	return points_list;
}


std::vector<Point3f> RectangleExtractor::CorrectPolygon(std::vector<Point3f>&dest, const std::vector<Point3f> &src)
{
	/*分别构造矩形和墙体结构*/
	std::vector<std::pair<Point3f, Point3f>> wall;

	std::vector<std::pair<Point3f, Point3f>> rectangle;

	for (int i = 0; i < src.size(); ++i)
	{
		int  j = (i + 1) % src.size();
		wall.push_back(std::make_pair(src[i], src[j]));
	}
	for (int i = 0; i < dest.size(); ++i)
	{
		int j = (i + 1) % dest.size();
		rectangle.push_back(std::make_pair(dest[i], dest[j]));
	}


	for (int i = 0; i < rectangle.size(); ++i)
	{
		for (int j = 0; j < wall.size(); ++j)
		{
			if (!ComMath::isParallelTwoLine(rectangle[i].first, rectangle[i].second, wall[j].first, wall[j].second))
			{
				continue;
			}
			float dist = ComMath::getPointToLineDis(rectangle[i].first, wall[j].first, wall[j].second);
			if (dist > granularity)
			{
				continue;
			}
			/*修复平行墙体*/
			Point3f proj_beg = ComMath::getPointToLinePedal(rectangle[i].first, wall[j].first, wall[j].second);
			Point3f proj_end = ComMath::getPointToLinePedal(rectangle[i].second, wall[j].first, wall[j].second);
			rectangle[i].first = proj_beg;
			rectangle[i].second = proj_end;

			/**获取垂直点*/
			for (int k = 0; k < rectangle.size(); ++k)
			{
				if (k == j)
					continue;
				if (ComMath::isParallelTwoLine(rectangle[k].first, rectangle[k].second, wall[j].first, wall[j].second))
					continue;
				auto proj = ComMath::getPointToLinePedal(rectangle[k].first, wall[j].first, wall[j].second);
				/*修复垂直墙的四边形*/
				float dist_beg = ComMath::getTwoPointDistance(rectangle[k].first, proj);
				float dist_end= ComMath::getTwoPointDistance(rectangle[k].second, proj);
				if (dist_beg > dist_end)
					rectangle[k].second = proj;
				else
					rectangle[k].first = proj;
			}
		}
	}
	/*添加顶点坐标*/
	std::vector<Point3f> point_list;

	point_list.push_back(rectangle[0].first);

	point_list.push_back(rectangle[0].second);

	point_list.push_back(rectangle[1].second);

	point_list.push_back(rectangle[2].second);

	return point_list;
}