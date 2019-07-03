#include "Room.h"




Room::Room()
{
	name = "";
	is_outroom = false;
	space_id = 0;
	// points.clear();
}

Room::~Room()
{
}


// ���������ʼ������
Room::Room(RoomRegion region)
{
	this->SetNo(region.GetNo());
	this->SetPropertyFlag(region.GetPropertyFlag());
	this->SetRegisterClass(region.GetRegisterClass());
	this->SetObjName(region.GetObjName());
	this->is_outroom = false;
	this->name = region.getRegionName();
	this->space_id = region.getRegionId();
	this->point_list = region.point_list;

}
void Room::setName(string name)
{
	this->name = name;
}

string Room::getName()
{
	return this->name;
}


bool Room::getIsOutroom()
{
	return this->is_outroom;
}

int Room::getSpaceId()
{
	return this->space_id;
}

void Room::setSpaceId(int space_id)
{
	this->space_id = space_id;
}

vector<Point3f> Room::getPointList()
{
	return this->point_list;
}


// Room Room::operator=(const Room& src_room)
// {
// 	
// }
/*
"objectId":"ABB5A4F4423D7A380855CD9524E6A9E2",
"objectName":"�ռ�",
"propertyFlag":192,
"Name":"����",
"ObjType":"OT_InternalRoom",
"points":[
"X=263.900 Y=-356.661 Z=0.000",
"X=263.900 Y=92.804 Z=0.000",
"X=-260.671 Y=92.804 Z=0.000",
"X=-260.671 Y=-356.661 Z=0.000"
],
"RegisterClass":"FXRRoom"
*/
void Room::LoadBimData(Json::Value data)
{
	// ��ʼ�����
	if (data.isMember("objectId") && data["objectId"].isString())
	{
		string objectId = data["objectId"].asString();
		this->SetNo(objectId);
	}

	// ��ʼ����־
	if (data.isMember("propertyFlag") && data["propertyFlag"].isInt())
	{
		int property_flag = data["propertyFlag"].asInt();
		this->SetPropertyFlag(property_flag);
	}

	// ��ʼ��ע����
	if (data.isMember("RegisterClass") && data["RegisterClass"].isString())
	{
		string register_class = data["RegisterClass"].asString();
		this->SetRegisterClass(register_class);
	}

	// ��ʼ��object����
	if (data.isMember("objectName") && data["objectName"].isString())
	{
		string obj_name = data["objectName"].asString();
		obj_name = ComUtil::JsonStringToUTF_8(obj_name);
		this->SetObjName(obj_name);
	}

	// ��ʼ��object����
	if (data.isMember("ObjType") && data["ObjType"].isString())
	{
		string obj_name = data["ObjType"].asString();
		if (obj_name == "OT_OutRoom")
		{
			this->is_outroom = true;
		}
	}

	// ��ʼ������
	if (data.isMember("name") && data["name"].isString())
	{
		string obj_name = data["name"].asString();
		obj_name = ComUtil::JsonStringToUTF_8(obj_name);
		this->name = obj_name;
	}

	// ��ʼ��space_id
	if (data.isMember("SpaceId") && data["SpaceId"].isInt())
	{
		int space_id = data["SpaceId"].asInt();
		this->space_id= space_id;
	}
	else
	{
		this->space_id = 0;
	}


	// ��ʼ�������ļ���
	if (data.isMember("points") && data["points"].isArray())
	{
		Json::Value points = data["points"];
		for (unsigned int i = 0; i < points.size(); i++)
		{
			if (points[i].isString())
			{
				string str_point = points[i].asString();
				Point3f temp_point = ComUtil::StringToPoint3f(str_point);
				this->point_list.push_back(temp_point);
			}
		}
	}

}

// �Ƿ��ڷ�����
bool Room::PointInRoom(Point3f point)
{
	return ComMath::PointInPolygon(point, point_list);
}
