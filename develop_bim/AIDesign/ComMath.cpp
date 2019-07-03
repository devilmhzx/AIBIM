#include "ComMath.h"



ComMath::ComMath()
{
}


ComMath::~ComMath()
{
}

//  两线段是否重合
bool ComMath::IsCoincideOfTwoLine(Point3f A, Point3f B, Point3f C, Point3f D)
{
	bool onLineA, onLineB, onLineC, onLineD;
	onLineA = isPointOnLine(A, C, D);
	onLineB = isPointOnLine(B, C, D);
	onLineC = isPointOnLine(C, A, B);
	onLineD = isPointOnLine(D, A, B);

	// AB在CD上或CD在AB上
	bool coincide01 = (onLineA && onLineB) || (onLineC && onLineD);
	bool coincide02 = (onLineA && onLineC && (A != C))
		|| (onLineA && onLineD && (A != D))
		|| (onLineB && onLineC && (B != C))
		|| (onLineB && onLineD && (B != D));

	return (coincide01 || coincide02);
}
bool ComMath::bIntersectOnTwoLines(Point3f L1Beg, Point3f L1End, Point3f L2Beg, Point3f L2End, Point3f*Out)
{
	L1Beg.z = 0.f;
	L1End.z = 0.f;
	L2Beg.z = 0.f;
	L2End.z = 0.f;
	{
		//第二条直线与第一条直线起始点叉乘
		auto line = L1End - L1Beg;
		auto one_side = L2Beg - L1Beg;
		auto another_size = L2End - L1Beg;
		auto cross1 = ComMath::Cross(one_side, line);
		auto cross2 = ComMath::Cross(another_size, line);
		//同向,没有相交
		if (cross1.z*cross2.z > 0)
			return false;
		line = L2End - L2Beg;
		one_side = L1Beg - L2Beg;
		another_size = L1End - L2Beg;
		cross1 = ComMath::Cross(one_side, line);
		cross2 = ComMath::Cross(another_size, line);
		//同向,没有相交
		if (cross1.z*cross2.z > 0)
			return false;
		//求交点
	}
	auto normalize = ComMath::Normalize(L1End - L1Beg);
	auto P1 = ComMath::Dot(normalize, L2Beg - L1Beg);
	auto P2 = ComMath::Dot(normalize, L2End - L1Beg);
	auto D1 = ComMath::getTwoPointDistance(L1Beg, L2Beg);
	auto D2 = ComMath::getTwoPointDistance(L2End, L1Beg);
	auto unNegative = D1 * D1 - P1 * P1;
	if (unNegative < 0.f)
		unNegative = 0.f;
	P1 = sqrtf(unNegative);
	unNegative = D2 * D2 - P2 * P2;
	if (unNegative < 0.f)
		unNegative = 0.f;
	P2 = sqrtf(unNegative);
	if (P1 == 0.f) {
		*Out = L2Beg;
		return true;
	}
	if (P2 == 0.f) {
		*Out = L2End;
		return true;
	}
	normalize = ComMath::Normalize(L2End - L2Beg);
	auto P3 = ComMath::getTwoPointDistance(L2End, L2Beg);
	P1 = P3 / (P1 + P2)*P1;
	*Out = L2Beg + normalize*P1;
	return true;
	//第一条直线与第二条直线起点叉乘

}

/*************************************************
Function:       getTwoPointDistance
Description:    求两点之间的距离
Input:          @param start 开始点
                @param end 结束点
Return:         距离
Others:         
*************************************************/
float ComMath::getTwoPointDistance(Point3f start, Point3f end)
{
	float x = start.x - end.x;
	float y = start.y - end.y;
	float z = start.z - end.z;

	return sqrtf(x*x + y*y + z*z);
}


bool ComMath::comTwoFloat(float src1, float src2)
{
	if (fabs(src1 - src2) < FLOAT_LIMIT)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 点到直线的距离
float ComMath::getPointToLineDis(Point3f point, Point3f start, Point3f end)
{
	

	float x1 = start.x;
	float y1 = start.y;
	float x2 = end.x;
	float y2 = end.y;
	float x0 = point.x;
	float y0 = point.y;


	// 直线公式x=x1
	if (comTwoFloat(x1, x2))
	{
		return fabs(x0 - x1);
	}

	// 直线公式y=y1
	if (comTwoFloat(y1, y2))
	{
		return fabs(y0 - y1);
	}

	// 直线公式  (y2-y1)x-(x2-x1)y-x1y2+x2y1=0
	// A = y2-y1  B= x1-x2  C = x2y1-x1y2
	// d = |(Ax0+By0+C)/sqrt(A*A + B*B)|

	float d = fabs(((y2 - y1)*x0 + (x1 - x2)*y0 + x2*y1 - x1*y2) / sqrtf((y2 - y1)*(y2 - y1) + (x1 - x2)*(x1 - x2)));

	return d;


}

// 计算点到两点构成的线段的距离
float ComMath::getPointToSegDis(Point3f point, Point3f start, Point3f end)
{
	float x1 = start.x;
	float y1 = start.y;
	float x2 = end.x;
	float y2 = end.y;
	float x = point.x;
	float y = point.y;

	double cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
	if (cross <= 0)
		return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));

	double d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	if (cross >= d2) 
		return sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));

	double r = cross / d2;
	double px = x1 + (x2 - x1) * r;
	double py = y1 + (y2 - y1) * r;

	return (float)sqrt((x - px) * (x - px) + (py - y) * (py - y));
}

// 点到直线的垂足
Point3f ComMath::getPointToLinePedal(Point3f point, Point3f start, Point3f end)
{
	float x1 = start.x;
	float y1 = start.y;
	float x2 = end.x;
	float y2 = end.y;
	float x0 = point.x;
	float y0 = point.y;


	// 直线公式x=x1
	if (comTwoFloat(x1, x2))
	{
		return Point3f(x1,y0,0.0f);
	}

	// 直线公式y=y1
	if (comTwoFloat(y1, y2))
	{
		return Point3f(x0, y1, 0.0f);
	}

	// 直线公式  (y2-y1)x-(x2-x1)y-x1y2+x2y1=0
	// A = y2-y1  B= x1-x2  C = x2y1-x1y2
	// x = (  B*B*x0  -  A*B*y0  -  A*C  ) / ( A*A + B*B );
	//  y  =  ( -A*B*x0 + A*A*y0 - B*C  ) / ( A*A + B*B );
	float A = y2 - y1;
	float B = x1 - x2;
	float C = x2*y1 - x1*y2;

	float x = (B*B*x0 - A*B*y0 - A*C) / (A*A + B*B);
	float y = (-A*B*x0 + A*A*y0 - B*C) / (A*A + B*B);

	return Point3f(x, y, 0.0f);
}

// 点是否在两点之间
bool ComMath::isPointBetweenPoint(Point3f point, Point3f start, Point3f end)
{
	float x1 = start.x;
	float y1 = start.y;
	float x2 = end.x;
	float y2 = end.y;
	float x0 = point.x;
	float y0 = point.y;

	float minx = min(x1, x2);
	float maxx = max(x1, x2);
	float miny = min(y1, y2);
	float maxy = max(y1, y2);

	if ((x0 >= minx&&x0 <= maxx) && (y0 >= miny && y0 <= maxy))
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 点是否在线段上
bool ComMath::isPointOnLine(Point3f point, Point3f start, Point3f end)
{
	float distance = getPointToLineDis(point, start, end);
	if (distance > FLOAT_LIMIT)
	{
		return false;
	}
	else
	{
		if (isPointBetweenPoint(point, start, end))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

// 判断两点是否相等
bool ComMath::comTwoPoint3f(Point3f src_point, Point3f des_point)
{
	if (comTwoFloat(src_point.x, des_point.x) && comTwoFloat(src_point.y, des_point.y) && comTwoFloat(src_point.z, des_point.z))
	{
		return true;
	}
	else
	{
		return false;
	}
}

vector<Point3f> ComMath::getMovePointOnLine(Point3f src_point, Point3f start, Point3f end, float dis)
{

	vector<Point3f> point_list;

	// 直线公式x=x1
	Point3f  vec = end - start;
	vec = ComMath::Normalize(vec);

	// 直线公式y=y1
	Point3f point1 = src_point + vec * dis;
	Point3f point2 = src_point + -vec * dis;


	// 求直线

	//列方程


	point_list.push_back(point1);
	point_list.push_back(point2);
	return point_list;
}

// 求点到直线的垂点，并且距离垂点一定距离的在线段上的两点
bool ComMath::getPointOnLine(Point3f src_point, Point3f start, Point3f end, float dis, vector<Point3f> &points)
{
	Point3f vertical_point = getPointToLinePedal(src_point, start, end);

	// 垂点不在线段上
	if (!isPointOnLine(vertical_point, start, end))
	{
		return false;
	}

	// 距离垂点一定距离的点
	vector<Point3f> point_list = getMovePointOnLine(vertical_point, start, end, dis);
	
	// 判断两点是否都在线段上
	for (int i = 0; i < point_list.size(); i++)
	{
		Point3f tmp_point = point_list[i];
		if (isPointBetweenPoint(tmp_point, start, end))
		{
			points.push_back(tmp_point);
			continue;
		}
		else
		{
			return false;
		}
	}

	return true;
}

// 两直线是否平行
bool ComMath::isParallelTwoLine(Point3f fisrt_start, Point3f fisrt_end, Point3f sec_start, Point3f sec_end)
{
	Point3f first_vector = Point3f(fisrt_end.x - fisrt_start.x, fisrt_end.y - fisrt_start.y, 0);
	Point3f sec_vector = Point3f(sec_end.x - sec_start.x, sec_end.y - sec_start.y, 0);
	float tmp = first_vector.x*sec_vector.y - sec_vector.x*first_vector.y;
	if (ComMath::comTwoFloat(tmp, 0))
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 两直线是否近似平行
 bool ComMath::isParallelTwoLine2(Point3f fisrt_start, Point3f fisrt_end, Point3f sec_start, Point3f sec_end)
{
	 if (isParallelTwoLine(fisrt_start, fisrt_end, sec_start, sec_end))
	 {
		 return true;
	 }

	 float angle = GetAngleBetweenTwoLines(fisrt_start- fisrt_end, sec_start- sec_end);
	 angle = angle / M_PI * 180;
	 if (angle < 5 || angle > 175)
	 {
		 return true;
	 }
	 else
	 {
		 return false;
	 }
}

// 已知线段AB ,其中点P在线段对应的直线上，求距离点P一定距离在线段AB上的点
Point3f ComMath::getEffectPointOnLine(Point3f src_point, Point3f start, Point3f end, float dis)
{
	Point3f vector_AB = Point3f(end.x - start.x, end.y - start.y, 0);
	// AB向量单位化
	if (ComMath::comTwoFloat(vector_AB.x, 0))
	{
		vector_AB.y = 1;
	}
	else if (ComMath::comTwoFloat(vector_AB.y, 0))
	{
		vector_AB.x = 1;
	}
	else
	{
		float tmp = sqrtf(vector_AB.x*vector_AB.x + vector_AB.y*vector_AB.y);
		vector_AB.x = vector_AB.x / tmp;
		vector_AB.y = vector_AB.y / tmp;
	}

	vector_AB.x = vector_AB.x*dis;
	vector_AB.y = vector_AB.y*dis;

	// 求对应的坐标
	Point3f resule_a = Point3f(src_point.x + vector_AB.x, src_point.y + vector_AB.y, 0);
	if (ComMath::isPointOnLine(resule_a, start, end))
	{
		return resule_a;
	}
	vector_AB.x = vector_AB.x*-1;
	vector_AB.y = vector_AB.y*-1;
	Point3f resule_b = Point3f(src_point.x + vector_AB.x, src_point.y + vector_AB.y, 0);
	if (ComMath::isPointOnLine(resule_b, start, end))
	{
		return resule_b;
	}
	
	return Point3f(0, 0, 0);

}
// 点在多边形内，认为点在多边形边上返回false
bool  ComMath::PointInPolygon2(const Point3f &src_point, const vector<Point3f>&points)
{
	int point_count = int(points.size());
	bool flag = false;

	for (int i = 0; i < point_count; i++)
	{
		const Point3f &start = points[i];
		int j = (i + 1) % point_count;
		const Point3f &end = points[j];
		if ((start.y < src_point.y && end.y >= src_point.y|| end.y < src_point.y&& start.y >= src_point.y)&& (start.x <= src_point.x || end.x <= src_point.x)) 
		{
			flag ^= (start.x + (src_point.y - start.y) / (end.y - start.y)*(end.x - start.x) < src_point.x);
		}
	}	 
	return flag;
}

// 点在多边形内，认为点在多边形边上返回true
bool  ComMath::PointInPolygon(const Point3f &src_point, const vector<Point3f>&points)
{
	int nCross = 0;
	int point_count = int(points.size());
	if (point_count < 3)
	{
		return false;
	}
	for (int i = 0; i < point_count; i++)
	{
		const Point3f &start = points[i];
		int j = (i + 1) % point_count;
		const Point3f &end = points[j];

		// 求解 y=p.y 与 p1p2 的交点
		if (comTwoFloat(start.y, end.y)) // p1p2 与 y=p0.y平行
			continue;

		if (src_point.y < min(start.y, end.y)) // 交点在p1p2延长线上
			continue;

		if (src_point.y >= max(start.y, end.y)) // 交点在p1p2延长线上
			continue;

		// 求交点的 X 坐标 --------------------------------------------------------------
		double x = (double)(end.y - start.y) * (double)(end.x - start.x) / (double)(end.y - start.y) + start.x;

		if (x > src_point.x)
			nCross++; // 只统计单边交点
	}

	// 单边交点为偶数，点在多边形之外 ---
	return (nCross % 2 == 1);
}


bool ComMath::CollisionDetection(const Point3f &Bmin, const Point3f &Bmax, const Point3f &Cmin, const Point3f &Cmax) {
	if ((Bmin.x >= Cmax.x||Bmin.y >= Cmax.y) || (Cmin.x >= Bmax.x||Cmin.y >= Bmax.y))
		return false;
	return true;
}

/**Bounding box碰撞*/
bool ComMath::IsCollisionRect(vector<Point3f> src_points, vector<Point3f> des_points)
{
	Box2D src_box= GetVertsBoundingBox(src_points);
	Box2D des_box = GetVertsBoundingBox(des_points);


	return CollisionDetection(Point3f(src_box.min_point.x, src_box.min_point.y,0), Point3f(src_box.max_point.x, src_box.max_point.y, 0),Point3f(des_box.min_point.x, des_box.min_point.y, 0), Point3f(des_box.max_point.x, des_box.max_point.y, 0));
}

// 多边形在多边形里面，对于边重合的返回false
bool ComMath::PolygonInPolygon2(const vector<Point3f> &inside_points, const vector<Point3f>& outside_points)
{
	for (auto& points : inside_points)
	{
		if (!ComMath::PointInPolygon2(points, outside_points))
		{
			return false;
		}
	}
	return true;
}

// 多边形在多边形里面，对于边重合的返回true
bool ComMath::PolygonInPolygon(const vector<Point3f> &inside_points, const vector<Point3f>& outside_points)
{
	for (auto& points : inside_points)
	{
		if (!ComMath::PointInPolygon(points, outside_points))
		{
			return false;
		}
	}
	return true;
}


/*bool ComMath::PolygonOverlap(vector<Point3f> inside_points, vector<Point3f> outside_points)
{
	for (auto& points : inside_points)
	{
		if (ComMath::PointInPolygon(points, outside_points))
		{
			return true;
			break;
		}
	}
	return false;
}*/

Box2D ComMath::GetVertsBoundingBox(const vector<Point3f>& Verts)
{
	Point2f max = Point2f(-9999999999.f, -9999999999.f);
	Point2f min = Point2f(9999999999.f, 9999999999.f);

	for (auto &It:Verts)
	{
		if (It.x > max.x)
			max.x = It.x;
		if (It.y > max.y)
			max.y = It.y;

		if (It.x < min.x)
			min.x = It.x;
		if (It.y < min.y)
			min.y = It.y;
	}
	return Box2D(min,max);
}

Point3f ComMath::Cross(const Point3f&left, const Point3f&right) {
	Point3f out = Point3f(
		left.y*right.z - left.z*right.y,
		left.z*right.x - left.x*right.z,
		left.x*right.y - left.y*right.x
	);
	return out;
}

Point3f ComMath::Normalize(const Point3f&V)
{
	float scale = ComMath::ScaleOfVector(V);
	Point3f point(V.x / scale, V.y / scale, V.z / scale);
	return point;
}

float ComMath::Dot(const Point3f&left, const Point3f&right)
{
	return left.x*right.x + left.y*right.y+ left.z*right.z;
}

float ComMath::GetAngleBetweenTwoLines(const Point3f&Line1, const Point3f&Line2)
{
	float theta = 0.f;
	float dot = ComMath::Dot(Line1, Line2);
	float line1Scale = ScaleOfVector(Line1);
	float line2Scale = ScaleOfVector(Line2);
	line1Scale = line1Scale* line2Scale;
	return acosf(dot / line1Scale);
}

float  ComMath::ScaleOfVector(const Point3f&Line)
{
	return sqrtf(Line.x*Line.x + Line.y*Line.y+ Line.z*Line.z);
}

bool ComMath::IsSameScalar(float left, float right, float tolerance) {
	if (abs(left - right) <= tolerance)
		return true;
	return false;
}

bool  ComMath::IsSameVectorOnApproximatly(const Point3f&left, const Point3f&right, float tolerance)
{
	bool same = true;
	if (abs(left.x - right.x) > tolerance)
		same = false;
	if (abs(left.y - right.y) > tolerance)
		same = false;
	if (abs(left.z - right.z) > tolerance)
		same = false;
	return same;
}

Point3f ComMath::Point3fMax(Point3f Left, Point3f Right) {
	Point3f out;
	out.x = max(Left.x, Right.x);
	out.y = max(Left.y, Right.y);
	out.z = max(Left.z, Right.z);
	return out;
}
 
Point3f ComMath::Point3fMin(Point3f Left, Point3f Right) {
	Point3f out;
	out.x = min(Left.x, Right.x);
	out.y = min(Left.y, Right.y);
	out.z = min(Left.z, Right.z);
	return out;
}

//矩形是否碰撞
//在水平方向上，判断两个矩形中点x坐标的距离是否小于两个矩形宽度一半之和
//在垂直方向上，判断两个矩形中点y坐标的距离是否小于两个矩形高度一半之和
/*bool  ComMath::IsCollision(vector<Point3f>src_points, vector<Point3f>des_points)
{
	// 判断是否是矩形
	if (src_points.size() != 4 && des_points.size() != 4)
	{
		return false;
	}

	// 获得矩形相关信息
	Point3f src_center;
	Point3f des_center;
	float src_width = 0;
	float src_length = 0;
	float des_width = 0;
	float des_length = 0;

	// 获得1矩形信息
	src_center.x = (src_points[0].x + src_points[2].x) / 2;
	src_center.y = (src_points[0].y + src_points[2].y) / 2;
	src_center.z = (src_points[0].z + src_points[2].z) / 2;
	src_width = fabsf(src_points[0].x - src_points[2].x);
	src_length = fabsf(src_points[0].y - src_points[2].y);
	// 获得2矩形信息
	des_center.x = (des_points[0].x + des_points[2].x) / 2;
	des_center.y = (des_points[0].y + des_points[2].y) / 2;
	des_center.z = (des_points[0].z + des_points[2].z) / 2;
	des_width = fabsf(des_points[0].x - des_points[2].x);
	des_length = fabsf(des_points[0].y - des_points[2].y);

	if (fabsf(src_center.x - des_center.x) > (src_width / 2 + des_width / 2))
	{
		return false;
	}
	else if (fabsf(src_center.y - des_center.y) > (src_length / 2 + des_length / 2))
	{
		return false;
	}
	else
	{
		return true;
	}
}*/

//四舍五入
int  ComMath::RoundD(double number)
{
	return (int)((number > 0.0) ? (number + 0.5) : (number - 0.5));
}

// 获得多边形面积
float  ComMath::GetPolygonArea(vector<Point3f>points)
{
	float area = 0.0;
	for (size_t i = 0; i != points.size(); ++i)
	{
		Point3f  start = points[i];
		Point3f  end = points[(i + 1) % points.size()];
		area = area + start.x*end.y - start.y*end.x;
	}

	return 0.5f * (area >= 0.f ? area : -area)/10000.f;
}

//检查两条线段是否相交(端点为交点，不认为相交,用来处理动线交叉问题
bool ComMath::IsIntersectTwoLine(Line l1, Line l2)
{
	// 排斥实验
	// p1-l1.start  p2 = l1.end  q1-l2.start q2=l2.end
	bool ret = min(l1.start.x, l1.end.x) <= max(l2.start.x, l2.end.x) &&min(l2.start.x, l2.end.x) <= max(l1.start.x, l1.end.x) &&min(l1.start.y, l1.end.y) <= max(l2.start.y, l2.end.y) &&min(l2.start.y, l2.end.y) <= max(l1.start.y, l1.end.y);
	if (!ret)
	{
		return false;
	}

	// 跨立实验
	// pFirst1 = l1.start pFirst2=l1.end  pSecond1 =l2.start pSecond2=l2.end
	long line1, line2;
	line1 = (long)(l1.start.x * (l2.start.y - l1.end.y) + l1.end.x * (l1.start.y - l2.start.y) + l2.start.x * (l1.end.y - l1.start.y));
	line2 = (long)(l1.start.x * (l2.end.y - l1.end.y) +l1.end.x * (l1.start.y - l2.end.y) + l2.end.x * (l1.end.y - l1.start.y));
	if (((line1 ^ line2) >= 0) && !(line1 == 0 && line2 == 0))
		return false;
	line1 = (long)(l2.start.x * (l1.start.y - l2.end.y) +l2.end.x * (l2.start.y - l1.start.y) +l1.start.x * (l2.end.y - l2.start.y));
	line2 = (long)(l2.start.x * (l1.end.y - l2.end.y) +l2.end.x * (l2.start.y - l1.end.y) +l1.end.x * (l2.end.y - l2.start.y));
	if (((line1 ^ line2) >= 0) && !(line1 == 0 && line2 == 0))
		return false;

	// 判断点是否在是另一条线段上
	if (isPointOnLine(l1.start, l2.start, l2.end))
	{
		return false;
	}

	if (isPointOnLine(l1.end, l2.start, l2.end))
	{
		return false;
	}

	if (isPointOnLine(l2.start, l1.start, l1.end))
	{
		return false;
	}

	if (isPointOnLine(l2.end, l1.start, l1.end))
	{
		return false;
	}

	return true;
}

//获取法线，直线横平竖直的
Point3f ComMath::getNormalByLine( Point3f& start,  Point3f& end)
{
	Point3f point;
	if (ComMath::IsSameScalar(start.x, end.x, 0.01f))
		point = Point3f(1.f, 0.f, 0.f);
	if (ComMath::IsSameScalar(start.y, end.y, 0.01f))
		point = Point3f(0.f, 1.f, 0.f);
	return point;
}
Point3f ComMath::getNormalByLineGenerally( Point3f& start,  Point3f& end)
{
	auto line = end - start;
	auto angle = M_PI / 2;
	auto x = line.x*cosf(angle) - sinf(angle)*line.y;
	line.y = line.x*sinf(angle) + cosf(angle)*line.y;
	line.x = x;
	return Normalize(line);
}

int ComMath::Round23(float& number)
{
	float number_f = static_cast<float>(number / 10 - 0.21);
	int num_out = int(ceil(number_f) * 10);
	return num_out;
}
