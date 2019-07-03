#pragma once
/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  Author:changyuan
*  Create Date:2018-08-29
*  Description:直线
**************************************************/
#include "Point3f.h"

class Line
{
public:
	Line(Point3f start, Point3f end);
	Line(Point3f start, Point3f end,Point3f direction);
	virtual ~Line();
	Line();

public:
	string no;
	Point3f start;
	Point3f end;
	Point3f direction;
	bool is_virtual;
};

