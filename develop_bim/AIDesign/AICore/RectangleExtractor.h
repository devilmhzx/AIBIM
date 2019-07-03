
/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:最大矩形萃取类,从一般户型SingleCase多边形中提取最大矩形(注意是矩形)
*  简要描述:
*
*  当前版本:V1.0.0
*  作者: 冯小翼
*  创建日期:2018-12-3
*  说明:
**************************************************/
#pragma once
#include "../SingleCase.h"
#include "../Point3f.h"
class  RectangleExtractor
{
public:
	RectangleExtractor() = default;
	~RectangleExtractor() = default;

public:
	/*---------------------------------------------------------------------------------------------------------------------------
	*@getMaximumInscribedRectangleOnPolygon : 提取最大矩形
	* 输入:点的集合
	-----------------------------------------------------------------------------------------------------------------------------*/
	static std::vector<Point3f> getMaximumInscribedRectangleOnPolygon(const std::vector<Point3f>&point_list);
	/*---------------------------------------------------------------------------------------------------------------------------
	*@getMaximumInscribedRectangleOnPolygonPrecisely : 提取最大矩形(精确)
	* 输入:点的集合
	-----------------------------------------------------------------------------------------------------------------------------*/
	static std::vector<Point3f>	getMaximumInscribedRectangleOnPolygonPrecisely(const std::vector<Point3f>&point_list, float length);

	/*---------------------------------------------------------------------------------------------------------------------------
	*@getMaximumInscribedRectangleOnPolygonPrecisely : 提取最大矩形(精确)
	* 输入: 点的集合
	-----------------------------------------------------------------------------------------------------------------------------*/
	std::vector<Point3f>  ExtractMaximumRectangleBySingleCase(const std::vector<Point3f>&point_list);
	/**for test*/
	std::vector<Point3f> getBaseNode()
	{
		std::vector<Point3f> point_s;
		for (int i = 0; i < node_base.size(); ++i)
		{
			for (int j = 0; j < node_base[i].size(); ++j)
			{
				Point3f pit = GridToWorld(i, j); 
				pit.z = node_base[i][j];
				point_s.push_back(pit);
				 
			}
		 }
		return point_s;
	}
	
private:

	/**私有结构体*/
	struct Segment
	{
		int        common;
		int		i;
		int		j;
		int		range;
		float     scores;
	};

	/**区域离散到划分到网格*/
	bool    SplitToGird(const std::vector<Point3f>&point_list);
	
	/**网格坐标转世界坐标*/
	Point3f GridToWorld(int x, int y);
	
	bool     IsGoodIndex(int x, int y);

	/**世界坐标转网格坐标*/
	void	WorldToGrid(Point3f point, int *x, int *y);

	/**水平扫描*/
	void    HorizontalScaningWithBottomUp();

	/**水平扫描*/
	void    HorizontalScaningWithUpBottom();

	/**垂直扫描*/
	void    VerticalScaningLeftRight();

	/**垂直扫描*/
	void    VerticalScaningRightLeft();

	/**计算有效矩形范围和分数BottomUp*/
	void   CalculateVaildRectBottomUp();

	/**计算有效矩形范围和分数UpBottom*/
	void   CalculateVaildRectUpBottom();

	/**计算有效矩形范围和分数UpBottom*/
	void   CalculateVaildRectLeftRight();

	/**计算有效矩形范围和分数UpBottom*/
	void   CalculateVaildRectRightLeft();

	/**计算矩形边界*/
	std::vector<Point3f> CalculteRectangleBoundary();

	/**矫正多边形*/
	std::vector<Point3f>	CorrectPolygon(std::vector<Point3f>&dest,const std::vector<Point3f> &src);
private:
	Point3f								box_max;
	Point3f								box_min;
	int									max_row;
	int									max_column;
	std::vector<std::vector<char>>		node_base;
	std::vector<Segment>		horizontal_bottom_up_segments;
	std::vector<Segment>		horizontal_up_bottom_segments;
	std::vector<Segment>		vertical_left_right_segments;
	std::vector<Segment>		vertical_right_left_segments;

};