/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:ComMath.h
*  简要描述:数学库
*
*  当前版本:
*  作者:常远
*  创建日期:2018-08-29
*  说明:
**************************************************/
#pragma once

#include <math.h>
#include "Point3f.h"
#include "Line.h"
#include "Box2D.h"
#include "CommonSettings.h"
#include <algorithm>
#include <vector>
#include <memory>

class ComMath
{
public:
	ComMath();
	~ComMath();

public:
	//  两线段是否重合
	static bool IsCoincideOfTwoLine(Point3f A, Point3f B, Point3f C, Point3f D);
	//检查两条线段是否相交,如果相交返回交点
	static bool bIntersectOnTwoLines(Point3f L1Beg, Point3f L1End, Point3f L2Beg, Point3f L2End,Point3f*Out);
	
	// 计算两点之间的距离
	static float getTwoPointDistance(Point3f start, Point3f end); 

	// 计算点到两点构成的直线的距离
	static float getPointToLineDis(Point3f point, Point3f start,Point3f end);
	// 计算点到两点构成的线段的距离
	static float getPointToSegDis(Point3f point, Point3f start, Point3f end);

	// 计算点到两点构成的直线的垂足
	static Point3f getPointToLinePedal(Point3f point, Point3f start, Point3f end);

	// 点是否在线段上
	static bool isPointOnLine(Point3f point, Point3f start, Point3f end);

	// 点是否在两点之间
	static bool isPointBetweenPoint(Point3f point, Point3f start, Point3f end);

	// 判断两个浮点数是否相等
	static bool comTwoFloat(float src1, float src2);

	// 判断两点是否相等
	static bool comTwoPoint3f(Point3f src_point, Point3f des_point);

	// 直线与圆的交点
	static vector<Point3f> getMovePointOnLine(Point3f src_point, Point3f start, Point3f end,float dis);

	// 求点到直线的垂点，并且距离垂点一定距离的在线段上的两点
	static bool getPointOnLine(Point3f src_point, Point3f start, Point3f end, float dis,vector<Point3f> &points);

	// 两直线是否平行
	static bool isParallelTwoLine(Point3f fisrt_start, Point3f fisrt_end, Point3f sec_start, Point3f sec_end);

	// 两直线是否近似平行
	static bool isParallelTwoLine2(Point3f fisrt_start, Point3f fisrt_end, Point3f sec_start, Point3f sec_end);

	// 已知线段AB ,其中点P在线段对应的直线上，求距离点P一定距离在线段AB上的点
	static Point3f getEffectPointOnLine(Point3f src_point, Point3f start, Point3f end, float dis);
	// 点是否在多边形内部
	static bool  PointInPolygon2(const Point3f& src_point, const vector<Point3f>&points);

	// 点是否在多边形内部2
	static bool  PointInPolygon(const Point3f& src_point, const vector<Point3f>&points);

	/** 多边形在多边形内部 */
	static bool PolygonInPolygon(const vector<Point3f> &inside_points, const vector<Point3f> &outside_points);

	/** 多边形在多边形内部2 */
	static bool PolygonInPolygon2(const vector<Point3f> &inside_points, const vector<Point3f> &outside_points);

	/**Bounding box碰撞*/
	static bool CollisionDetection(const Point3f &Bmin, const Point3f &Bmax, const Point3f &Cmin, const Point3f &Cmax);
	/**Bounding box碰撞*/
	static bool IsCollisionRect(vector<Point3f> src_points, vector<Point3f> des_points);

	/** 多边形是否重叠 */
	//static bool PolygonOverlap(vector<Point3f> inside_points, vector<Point3f> outside_points);

	/** 获取包围盒 */
	static Box2D GetVertsBoundingBox(const vector<Point3f> &Verts);
	
	//叉乘
	static Point3f Cross(const Point3f&left, const Point3f&right);

	//normalize
	static Point3f Normalize(const Point3f&V);

	//点乘
	static float Dot(const Point3f&left, const Point3f&right);

	//获取两个直线的夹角
	static float GetAngleBetweenTwoLines(const Point3f&Line1, const Point3f&Line2);

	//模长
	static float  ScaleOfVector(const Point3f&Line);
	//数值比较
	static bool IsSameScalar(float left, float right, float tolerance);
	//单位向量对比
	static bool  IsSameVectorOnApproximatly(const Point3f&left, const Point3f&right,float tolerance);

	//向量最大值
	static Point3f Point3fMax(Point3f Left, Point3f Right);

	//向量最小值
	static Point3f Point3fMin(Point3f Left, Point3f Right);

	//四舍五入
	static int  RoundD(double number);

	// 获得多边形面积
	static float  GetPolygonArea(vector<Point3f>points);

	//检查两条线段是否相交(端点为交点，不认为相交,用来处理动线交叉问题
	static bool IsIntersectTwoLine(Line l1, Line l2);
	//获取法线，直线横平竖直的
	static Point3f getNormalByLine( Point3f& start,  Point3f& end);
	//获取法线
	//获取法线
	static Point3f getNormalByLineGenerally(Point3f& start, Point3f& end);

	/** 1.99舍2.00入 @纪仁泽 */
	static int Round23(float& number);
};

