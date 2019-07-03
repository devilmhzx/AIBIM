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
	// ��ʼ������
	this->length = p_door->length;
	// ��ʼ�����
	this->width = p_door->width;
	// ��ʼ���߶�
	this->height = p_door->height;
	this->height_to_floor = 0;
	this->right_open = p_door->right_open;
	this->window_type = WIN_NORMAL;
	this->is_light = p_door->is_light;
}
/*
"objectId":"583B41464125B74E6FE8D3AB9E6CD971",
"objectName":"ƽ����",
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
		this->SetObjName(obj_name);
	}

	// ��ʼ��λ��
	if (data.isMember("pos") && data["pos"].isString())
	{
		string pos_tmp = data["pos"].asString();
		this->pos = Point3f(ComUtil::StringToPoint3f(pos_tmp));
	}

	// ��ʼ������
	if (data.isMember("direction") && data["direction"].isString())
	{
		string direction_tmp = data["direction"].asString();
		this->direction = Point3f(ComUtil::StringToPoint3f(direction_tmp));
	}


	// ��ʼ������
	this->length = static_cast<float>(ComUtil::GetJsonIntByKey(data, "length"));
	// ��ʼ�����
	this->width = static_cast<float>(ComUtil::GetJsonIntByKey(data, "width"));
	// ��ʼ���߶�
	this->height = static_cast<float>(ComUtil::GetJsonIntByKey(data, "height"));

	// ��ʼ����ظ߶�
	this->height_to_floor = ComUtil::GetJsonIntByKey(data, "heightToFloor");

	// ��ʼ�����ŷ���
	if (data.isMember("bRightOpen") && data["bRightOpen"].isBool())
	{
		bool bright_open = data["bRightOpen"].asBool();
		this->right_open = bright_open;
	}

	// ��ʼ�����ŷ���-APP
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

	// �Ƿ�Ϊ�����-APP
	if (data.isMember("is_light") && data["is_light"].isInt())
	{
		int light_flag = data["is_light"].asInt();
		if (light_flag == 1)
		{
			is_light = true;
		}
	}

	// ��ʼ������������
	/** ������BIM���ȶ� */
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