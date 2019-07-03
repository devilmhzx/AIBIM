#include "Line.h"



Line::Line()
{
	this->no = "";
	this->start = Point3f(0, 0, 0);
	this->end = Point3f(0, 0, 0);
	this->is_virtual = false;
}


Line::~Line()
{
}

Line::Line(Point3f start, Point3f end)
{
	this->is_virtual = false;
	this->start = start;
	this->end = end;
}

Line::Line(Point3f start, Point3f end, Point3f direction)
{
	this->start = start;
	this->end = end;
	this->direction = direction;
	this->is_virtual = false;
}