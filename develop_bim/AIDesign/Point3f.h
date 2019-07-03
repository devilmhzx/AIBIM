/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:三维点
*  简要描述:
*
*  当前版本:
*  作者:常远
*  创建日期:2018-08-29
*  说明:
**************************************************/

#include <string>

using namespace std;
#pragma once
class Point3f
{
public:
	Point3f();
	Point3f(float x, float y, float z);
	Point3f(const Point3f &point3f);

	virtual ~Point3f();
	Point3f operator+(const Point3f& src_point);
	Point3f operator=(const Point3f& src_point);
	Point3f operator-(const Point3f& src_point);
	Point3f operator*(float src);
	Point3f operator-(void);
	bool operator ==(const Point3f& src_point);
	bool operator!=(const Point3f& src_point);
	void   FromArray(float Array[3]);
	void   ToArray(float Array[3]);
	string ToString();

public:
	float x;
	float y;
	float z;
};

