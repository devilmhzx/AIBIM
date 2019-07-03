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
"objectName":"��׼��",
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
	// ��ʼ�����
	if (data.isMember("objectId") && data["objectId"].isString())
	{
		string objectId = data["objectId"].asString();
		this->SetNo(objectId);
	}

	// ��ʼ��object����
	if (data.isMember("objectName") && data["objectName"].isString())
	{
		string obj_name = data["objectName"].asString();
		this->SetObjName(obj_name);
	}

	// ��ʼ����־
	if (data.isMember("propertyFlag") && data["propertyFlag"].isInt())
	{
		int property_flag = data["propertyFlag"].asInt();
		this->SetPropertyFlag(property_flag);
	}

	string register_class;
	// ��ʼ��ע����
	if (data.isMember("RegisterClass") && data["RegisterClass"].isString())
	{
		register_class = data["RegisterClass"].asString();
		this->SetRegisterClass(register_class);
	}

	// �����ŵ�����-BIM
	if (register_class == "FXRNewPass"|| register_class == "FXRPass")
	{
		this->door_type = D_PASS;
	}
	else
	{
		this->door_type = D_NORMAL;
	}

	// ��ʼ��object����
	/** BIM�޴����� */
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

	// �ŵĲ�������-APP
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

	// �Ƿ�Ϊ�����-APP
	if (data.isMember("is_light") && data["is_light"].isInt())
	{
		int light_flag = data["is_light"].asInt();
		if (light_flag == 1)
		{
			is_light = true;
		}
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


	// ��ʼ�����ŷ���-BIM
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


	//Json::FastWriter writer;
	//string  json_file = writer.write(data);

	// ��ʼ���ŵ�����-APP
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