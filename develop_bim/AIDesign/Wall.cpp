#include "Wall.h"



Wall::Wall()
{
	//start_corner = new Corner();
	//end_corner = new Corner();
	room_no = "";
	wall_type = WALL_NORMAL;
	height = 2400;
	is_bearing = true;
	is_virtual = false;
}


Wall::~Wall()
{
	//delete start_corner;
	//delete end_corner;
}

Wall::Wall(Corner* start_corner, Corner* end_corner)
{
	this->SetNo(ComUtil::getGuuidNo());
	this->start_corner = *start_corner;
	this->end_corner = *end_corner;
	is_virtual = false;
}