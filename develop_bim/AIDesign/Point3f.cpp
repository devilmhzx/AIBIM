#include "Point3f.h"
#include "ComMath.h"


Point3f::Point3f()
{
	this->x = 0.0;
	this->y = 0.0;
	this->z = 0.0;
}


Point3f::~Point3f()
{
}

Point3f::Point3f(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Point3f::Point3f(const Point3f &point3f)
{
	this->x = point3f.x;
	this->y = point3f.y;
	this->z = point3f.z;
}

// 重载加法
Point3f Point3f::operator+(const Point3f& src_point)
{
	Point3f tmp_point;
	tmp_point.x = this->x + src_point.x;
	tmp_point.y = this->y + src_point.y;
	tmp_point.z = this->z + src_point.z;

	return tmp_point;

}
// 重载等号
Point3f Point3f::operator=(const Point3f& src_point)
{
	this->x = src_point.x;
	this->y = src_point.y;
	this->z = src_point.z;

	return *this;
}


void   Point3f::FromArray(float Array[3]) {
	this->x = Array[0];
	this->y = Array[1];
	this->z = Array[2];
}

void   Point3f::ToArray(float Array[3]) {
	Array[0] = x;
	Array[1] = y;
	Array[2] = z;
}
// 重载减法
Point3f Point3f::operator-(const Point3f& src_point)
{
	Point3f tmp_point;
	tmp_point.x = this->x - src_point.x;
	tmp_point.y = this->y - src_point.y;
	tmp_point.z = this->z - src_point.z;

	return tmp_point;

}


Point3f Point3f::operator*(float src)
{
	Point3f tmp_point;
	tmp_point.x = this->x *src;
	tmp_point.y = this->y*src;
	tmp_point.z = this->z *src;
	return tmp_point;
}
Point3f  Point3f::operator-(void)
{
	return Point3f( -x,-y,-z);
}

bool Point3f::operator ==(const Point3f& src_point)
{
	if (ComMath::comTwoPoint3f(*this, src_point))
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool Point3f::operator!=(const Point3f& src_point)
{
	if (*this == src_point)
	{
		return false;
	}
	else
	{
		return true;
	}
}
string Point3f::ToString()
{
	char *temp = new char[64];
	sprintf(temp, "X=%3.3f Y=%3.3f Z=%3.3f\0", x, y, z);
	string result = string(temp);
	delete[]temp;
	return result;
}

