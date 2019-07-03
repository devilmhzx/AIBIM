#include "PackPipeJ.h"

void PackPipeJ::LoadBimPackPipe(Json::Value data)
{
	// 初始化编号
	if (data.isMember("objectId") && data["objectId"].isString())
	{
		string objectId = data["objectId"].asString();
		this->SetNo(objectId);
	}

	// 初始化object名称
	if (data.isMember("objectName") && data["objectName"].isString())
	{
		string obj_name = data["objectName"].asString();
		this->SetObjName(obj_name);
	}

	// 初始化标志
	if (data.isMember("propertyFlag") && data["propertyFlag"].isInt())
	{
		int property_flag = data["propertyFlag"].asInt();
		this->SetPropertyFlag(property_flag);
	}

	// 初始化注册类
	if (data.isMember("RegisterClass") && data["RegisterClass"].isString())
	{
		string register_class = data["RegisterClass"].asString();
		this->SetRegisterClass(register_class);
	}


	// 初始化房间点的集合
	if (data.isMember("outsideVertices") && data["outsideVertices"].isArray())
	{
		Json::Value outsideVertices = data["outsideVertices"];
		vector<Point3f> temp_outsideVertices;
		for (unsigned int i = 0; i < outsideVertices.size(); i++)
		{
			if (outsideVertices[i].isString())
			{
				string str_point = outsideVertices[i].asString();
				Point3f temp_point = ComUtil::StringToPoint3f(str_point);
				temp_outsideVertices.push_back(temp_point);
			}
		}

		CreateBox(temp_outsideVertices);
	}
}

FloorBox PackPipeJ::GetNewFloorBox(PackPipeJ& in_packpipe)
{
	FloorBox temp_floor_box = FloorBox();

	/** 地标类型 */
	temp_floor_box.floor_box_type = FB_PACKPIPE;

	/** 长宽高为构造后的矩形，高无效 */
	temp_floor_box.length = in_packpipe.length;
	temp_floor_box.width = in_packpipe.width;
	temp_floor_box.height = in_packpipe.height;
	temp_floor_box.is_circle = in_packpipe.is_circle;
	temp_floor_box.center = in_packpipe.center;
	temp_floor_box.radio = in_packpipe.radio;
	temp_floor_box.point_list = in_packpipe.outsideVertices;

	return temp_floor_box;
}

void PackPipeJ::CreateBox(vector<Point3f>& in_Vertices)
{
	float x_min = 99999.9f;
	float x_max = -99999.9f;
	float y_min = 99999.9f;
	float y_max = -99999.9f;

	for (auto& temp_point : in_Vertices)
	{
		x_min = (x_min < temp_point.x) ? x_min : temp_point.x;
		x_max = (x_max > temp_point.x) ? x_max : temp_point.x;
		y_min = (y_min < temp_point.y) ? y_min : temp_point.y;
		y_max = (y_max > temp_point.y) ? y_max : temp_point.y;
	}
	outsideVertices.push_back(Point3f(x_min, y_min, 0.0f));
	outsideVertices.push_back(Point3f(x_max, y_min, 0.0f));
	outsideVertices.push_back(Point3f(x_max, y_max, 0.0f));
	outsideVertices.push_back(Point3f(x_min, y_max, 0.0f));

	center = Point3f((x_max + x_min)/2, (y_max + y_min)/2, 0.0f);
	length = x_max - x_min;
	width = y_max - y_min;
}

