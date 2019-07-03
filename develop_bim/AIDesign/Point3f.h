/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:��ά��
*  ��Ҫ����:
*
*  ��ǰ�汾:
*  ����:��Զ
*  ��������:2018-08-29
*  ˵��:
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

