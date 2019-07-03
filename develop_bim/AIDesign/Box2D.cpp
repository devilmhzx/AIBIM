#include "Box2D.h"

bool Box2D::operator==(const Box2D & other) const
{
	return (min_point==other.min_point)&&(max_point==other.max_point);
}

Box2D Box2D::operator=(const Box2D & other)
{
	this->max_point =  other.max_point;
	this->min_point = other.min_point;
	return *this;
}

Point2f Box2D::GetExtent() const
{
	return (max_point-min_point)*0.5f;
}

Point2f Box2D::GetCenter() const
{
	return Point2f((min_point+max_point)*0.5f);
}

Point2f Box2D::GetSize() const
{
	return(max_point - min_point);
}
