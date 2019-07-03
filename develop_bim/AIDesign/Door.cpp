#include "Door.h"



Door::Door()
{
	door_type = D_NORMAL;
	door_layout_type = D_NORMAL_LAYOUT;
}


Door::~Door()
{
}

/*
"objectId":"40A3E5E141D66280528CB689D67A3474",
"objectName":"标准门",
"propertyFlag":65,
"pos":"X=-179.697 Y=-368.661 Z=0.000",
"direction":"X=-0.000 Y=-1.000 Z=-0.000",
"length":85,
"width":24,
"height":208,
"bRightOpen":false,
"RegisterClass":"FXRSingleDoor"
*/
void Door::LoadBimDoor(Json::Value data)
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

	string register_class;
	// 初始化注册类
	if (data.isMember("RegisterClass") && data["RegisterClass"].isString())
	{
		register_class = data["RegisterClass"].asString();
		this->SetRegisterClass(register_class);
	}

	// 设置门的类型-BIM
	if (register_class == "FXRNewPass"|| register_class == "FXRPass")
	{
		this->door_type = D_PASS;
	}
	else
	{
		this->door_type = D_NORMAL;
	}

	// 初始化object类型
	/** BIM无此数据 */
	if (data.isMember("ObjectType") && data["ObjectType"].isString())
	{
		string object_type = data["ObjectType"].asString();
		if (object_type == "OT_Pass")
		{
			this->door_type = D_PASS;
		}
		else
		{
			this->door_type = D_NORMAL;
		}
	}

	// 门的布局类型-APP
	//door_layout_type = D_NORMAL_LAYOUT;
	if (data.isMember("layout_type") && data["layout_type"].isInt())
	{
		int layout_type = data["layout_type"].asInt();
		if (layout_type == 1)
		{
			door_layout_type = D_SUN_LAYOUT;
		}
		else
		{
			door_layout_type = D_NORMAL_LAYOUT;
		}
	}

	// 是否为进光口-APP
	if (data.isMember("is_light") && data["is_light"].isInt())
	{
		int light_flag = data["is_light"].asInt();
		if (light_flag == 1)
		{
			is_light = true;
		}
	}

	// 初始化位置
	if (data.isMember("pos") && data["pos"].isString())
	{
		string pos_tmp = data["pos"].asString();
		this->pos = Point3f(ComUtil::StringToPoint3f(pos_tmp));
	}

	// 初始化朝向
	if (data.isMember("direction") && data["direction"].isString())
	{
		string direction_tmp = data["direction"].asString();
		this->direction = Point3f(ComUtil::StringToPoint3f(direction_tmp));
	}

	// 初始化长度
	this->length = static_cast<float>(ComUtil::GetJsonIntByKey(data, "length"));
	// 初始化宽度
	this->width = static_cast<float>(ComUtil::GetJsonIntByKey(data, "width"));
	// 初始化高度
	this->height = static_cast<float>(ComUtil::GetJsonIntByKey(data, "height"));


	// 初始化开门方向-BIM
	if (data.isMember("bRightOpen") && data["bRightOpen"].isBool())
	{
		bool bright_open = data["bRightOpen"].asBool();
		this->right_open = bright_open;
	}

	// 初始化开门方向-APP
	if (data.isMember("bRightOpen") && data["bRightOpen"].isInt())
	{
		int bright_open = data["bRightOpen"].asInt();
		if (bright_open == 0)
		{
			this->right_open = false;
		}
		else
		{
			this->right_open = true;
		}
	}


	//Json::FastWriter writer;
	//string  json_file = writer.write(data);

	// 初始化门的类型-APP
	if (data.isMember("type") && data["type"].isInt())
	{
		int  type = data["type"].asInt();
		switch (type) {
		case 0:
			this->door_type = D_NORMAL;
			break;
		case 1:
			this->door_type = D_SLIDING;
			break;
		case 2:
			this->door_type = D_PASS;
			break;
		case 3:
			this->door_type = D_SECURITY;
			break;
		default:
			this->door_type = D_NORMAL;
		}
	}
}