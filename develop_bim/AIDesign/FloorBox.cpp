#include "FloorBox.h"



FloorBox::FloorBox()
{
}


FloorBox::~FloorBox()
{
}

// ����BIM����
/*"objectId":"8DDD9F8B47C4FEA4AA55CB91FC675EA2",
"objectName" : "����",
"propertyFlag" : 65,
"ParentList" : [

],
"Children" : [

],
"Type" : "OT_Pillar",
"ComponentType" : -1,
"BelongClass" : 0,
"Refrom" : false,
"Altitude" : 0,
"LocalTransform" : "-130.902878,35.266033,0.000000|0.000000,0.000000,0.000000|1.000000,1.000000,1.000000",
"Altitudes" : Array[0],
"PolyLines" : Array[2],
"Points" : Array[10],
"RelatedFurnitureMap" : [

],
"SwitchCoupletNum" : -842150451,
"RegisterClass" : "FXRFurniture"*/

void FloorBox::LoadBimData(Json::Value data)
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

	// ��ʼ��ע����
	string register_class;
	if (data.isMember("RegisterClass") && data["RegisterClass"].isString())
	{
		register_class = data["RegisterClass"].asString();
		this->SetRegisterClass(register_class);
	}

	// ���õر�����
	if (register_class == "FXRPillar")
	{

	}
	// ��ʼ��λ��
	if (data.isMember("pos") && data["pos"].isString())
	{
		string pos_tmp = data["pos"].asString();
		this->center = Point3f(ComUtil::StringToPoint3f(pos_tmp));
	}



	// ��ʼ�����
	if (data.isMember("Type") && data["Type"].isString())
	{
		string type = data["Type"].asString();

		// ���Ӵ�����Ͱ
		if (type == "OT_Pillar")
		{
			this->floor_box_type = FB_STOOL;
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

			// ȡ�����
			if (data.isMember("Points") && data["Points"].isArray())
			{
				Json::Value points = data["Points"];
				if (points.size() < 3)
				{
					this->SetNo(DEFAULT_NO);
					return;
				}

				Json::Value tmp_point = points[2];
				if (tmp_point.isMember("Pos") && tmp_point["Pos"].isString())
				{
					string str_point = tmp_point["Pos"].asString();
					this->center = ComUtil::StringToPoint3f(str_point);
			    }
				else
				{
					this->SetNo(DEFAULT_NO);
					return;
				}
				
			}
		}
		else
		{
			this->SetNo(DEFAULT_NO);
			return;
		}
	}
}

void FloorBox::LoadBimDataTest(Json::Value data)
{
	// ��ʼ�����
	if (data.isMember("objectId") && data["objectId"].isString())
	{
		string objectId = data["objectId"].asString();
		this->SetNo(objectId);
	}

	if (data.isMember("pos") && data["pos"].isString())
	{
		string str_point = data["pos"].asString();
		this->center = ComUtil::StringToPoint3f(str_point);
	}

	// ��ʼ��λ��
	if (data.isMember("pos") && data["pos"].isString())
	{
		string pos_tmp = data["pos"].asString();
		this->center = Point3f(ComUtil::StringToPoint3f(pos_tmp));
	}

	// ��ʼ������
	this->length = static_cast<float>(ComUtil::GetJsonIntByKey(data, "length"));
	// ��ʼ�����
	this->width = static_cast<float>(ComUtil::GetJsonIntByKey(data, "width"));
	// ��ʼ���߶�
	this->height = static_cast<float>(ComUtil::GetJsonIntByKey(data, "height"));

}
