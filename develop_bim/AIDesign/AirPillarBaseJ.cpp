#include "AirPillarBaseJ.h"

AirPillarBaseJ::AirPillarBaseJ()
	:airpillar_type(NO_Type), top_left(Point3f(0.0f, 0.0f, 0.0f)), bottom_left(Point3f(0.0f, 0.0f, 0.0f)), top_right(Point3f(0.0f, 0.0f, 0.0f)), bottom_right(Point3f(0.0f, 0.0f, 0.0f))
{
}


AirPillarBaseJ::~AirPillarBaseJ()
{
}

void AirPillarBaseJ::LoadBimAirPillar(Json::Value data)
{
	// ��ʼ�����
	if (data.isMember("objectId") && data["objectId"].isString())
	{
		string objectId = data["objectId"].asString();
		this->m_no = objectId;
	}

	// ��ʼ��object����
	if (data.isMember("objectName") && data["objectName"].isString())
	{
		string obj_name = data["objectName"].asString();
		this->m_obj_name = obj_name;
	}

	// ��ʼ����־
	if (data.isMember("propertyFlag") && data["propertyFlag"].isInt())
	{
		int property_flag = data["propertyFlag"].asInt();
		this->m_property_flag = property_flag;
	}

	// ��ʼ��ע����
	string register_class;
	if (data.isMember("RegisterClass") && data["RegisterClass"].isString())
	{
		register_class = data["RegisterClass"].asString();
		this->m_register_class = register_class;
	}

	/** ���õر����� */
	if (register_class == "FXRAirFlue")
	{
		this->airpillar_type = Air_AirFlue;
	}
	else if (register_class == "FXRPillar")
	{
		this->airpillar_type = Pi_Pillar;
	}
	

	// ��ʼ��λ��
	if (data.isMember("pos") && data["pos"].isString())
	{
		string pos_tmp = data["pos"].asString();
		this->center = Point3f(ComUtil::StringToPoint3f(pos_tmp));
	}

	/** ���� */
	if (data.isMember("LeftToppos") && data["LeftToppos"].isString())
	{
		string left_top_tmp = data["LeftToppos"].asString();
		this->top_left = Point3f(ComUtil::StringToPoint3f(left_top_tmp));
	}
	/** ���� */
	if (data.isMember("LeftBottompos") && data["LeftBottompos"].isString())
	{
		string left_bottom_tmp = data["LeftBottompos"].asString();
		this->bottom_left = Point3f(ComUtil::StringToPoint3f(left_bottom_tmp));
	}
	/** ���� */
	if (data.isMember("RightToppos") && data["RightToppos"].isString())
	{
		string right_top_tmp = data["RightToppos"].asString();
		this->top_right = Point3f(ComUtil::StringToPoint3f(right_top_tmp));
	}
	/** ���� */
	if (data.isMember("RightBottompos") && data["RightBottompos"].isString())
	{
		string right_bottom_tmp = data["RightBottompos"].asString();
		this->bottom_right = Point3f(ComUtil::StringToPoint3f(right_bottom_tmp));
	}

	// ��ʼ������
	this->length = static_cast<float>(ComUtil::GetJsonIntByKey(data, "length"));
	// ��ʼ�����
	this->width = static_cast<float>(ComUtil::GetJsonIntByKey(data, "width"));
	// ��ʼ���߶�
	this->height = static_cast<float>(ComUtil::GetJsonIntByKey(data, "height"));
}

FloorBox AirPillarBaseJ::GetNewFloorBox(AirPillarBaseJ & in_airpillar)
{
	FloorBox temp_floor_box = FloorBox();

	/** �ر����� */
	if (in_airpillar.airpillar_type == Air_AirFlue)
	{
		temp_floor_box.floor_box_type = FB_PIPING;
	}
	else if (in_airpillar.airpillar_type == Pi_Pillar)
	{
		temp_floor_box.floor_box_type = FB_PILLAR;
	}

	temp_floor_box.length = in_airpillar.length;
	temp_floor_box.width = in_airpillar.width;
	temp_floor_box.height = in_airpillar.height;
	temp_floor_box.is_circle = in_airpillar.is_circle;
	temp_floor_box.center = in_airpillar.center;
	temp_floor_box.radio = in_airpillar.radio;
	temp_floor_box.point_list.push_back(in_airpillar.top_left);
	temp_floor_box.point_list.push_back(in_airpillar.top_right);
	temp_floor_box.point_list.push_back(in_airpillar.bottom_right);
	temp_floor_box.point_list.push_back(in_airpillar.bottom_left);

	return temp_floor_box;
}
