#include "Window.h"



Window::Window()
{
	height_to_floor = 0;
	window_type = WIN_NORMAL;
	is_light = false;
}


Window::~Window()
{
}

Window::Window(Door *p_door)
{
	this->SetNo(p_door->GetNo());
	this->SetPropertyFlag(p_door->GetPropertyFlag());
	this->SetRegisterClass(p_door->GetRegisterClass());
	this->pos = p_door->pos;
	this->direction = p_door->direction;
	// 初始化长度
	this->length = p_door->length;
	// 初始化宽度
	this->width = p_door->width;
	// 初始化高度
	this->height = p_door->height;
	this->height_to_floor = 0;
	this->right_open = p_door->right_open;
	this->window_type = WIN_NORMAL;
	this->is_light = p_door->is_light;
}
/*
"objectId":"583B41464125B74E6FE8D3AB9E6CD971",
"objectName":"平开窗",
"propertyFlag":65,
"pos":"X=42.101 Y=104.804 Z=0.000",
"direction":"X=0.000 Y=1.000 Z=0.000",
"length":237,
"width":24,
"height":201,
"heightToFloor":60,
"bRightOpen":true,
"RegisterClass":"FXRWindow"
*/
void Window::LoadBimWindow(Json::Value data)
{
	// 初始化编号
	if (data.isMember("objectId") && data["objectId"].isString())
	{
		string objectId = data["objectId"].asString();
		this->SetNo(objectId);
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

	// 初始化object名称
	if (data.isMember("objectName") && data["objectName"].isString())
	{
		string obj_name = data["objectName"].asString();
		this->SetObjName(obj_name);
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

	// 初始化离地高度
	this->height_to_floor = ComUtil::GetJsonIntByKey(data, "heightToFloor");

	// 初始化开门方向
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

	// 是否为进光口-APP
	if (data.isMember("is_light") && data["is_light"].isInt())
	{
		int light_flag = data["is_light"].asInt();
		if (light_flag == 1)
		{
			is_light = true;
		}
	}

	// 初始化窗户的类型
	/** 此数据BIM不稳定 */
	if (data.isMember("type") && data["type"].isInt())
	{
		int  type = data["type"].asInt();
		switch (type) {
		case 0:
			this->window_type = WIN_NORMAL;
			break;
		case 1:
			this->window_type = WIN_FLOOR;
			break;
		case 2:
			this->window_type = WIN_BAY_FLOOR;
			break;
		case 3:
			this->window_type = WIN_TRAPE_BAY;
			break;
		default:
			this->window_type = WIN_NORMAL;
		}
	}
}