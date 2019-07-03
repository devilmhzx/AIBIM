#include "Corner.h"
#include "Point3f.h"


Corner::Corner()
{
	point.x = 0.0;
	point.y = 0.0;
	point.z = 0.0;
	room_no = "";
}


Corner::~Corner()
{
}


void Corner::CopyCorner(Corner *corner)
{
	this->point = corner->point;
	this->GetNo() = corner->GetNo();
}
