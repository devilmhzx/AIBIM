#include "Point2f.h"

Point2f::Point2f(const Point2f & point2f)
{
	this->x = point2f.x;
	this->y = point2f.y;
}

Point2f Point2f::operator+(float number) const
{
	return Point2f(x+ number,y+ number);
}

Point2f Point2f::operator-(float number) const
{
	return Point2f(x-number,y-number);
}

Point2f Point2f::operator*(float scale) const
{
	return Point2f(x*scale,y*scale);
}

Point2f Point2f::operator+(const Point2f & src_point) const
{
	return Point2f(x+src_point.x,y+src_point.y);
}

Point2f Point2f::operator=(const Point2f & src_point)
{
	this->x = src_point.x;
	this->y = src_point.y;
	return *this;
}

Point2f Point2f::operator-(const Point2f & src_point) const
{
	return Point2f(x-src_point.x,y-src_point.y);
}

Point2f Point2f::operator*(const Point2f & src_point) const
{
	return Point2f(x*src_point.x,y*src_point.y);
}

bool Point2f::operator==(const Point2f & src_point) const
{
	return x==src_point.x && y==src_point.y;
}

bool Point2f::operator>(const Point2f & src_point) const
{
	return x>src_point.x && y>src_point.y;
}

bool Point2f::operator<(const Point2f & src_point) const
{
	return x<src_point.x && y<src_point.y;
}
