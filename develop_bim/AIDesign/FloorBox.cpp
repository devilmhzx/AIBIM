#include "FloorBox.h"



FloorBox::FloorBox()
{
}


FloorBox::~FloorBox()
{
}

// 解析BIM数据
/*"objectId":"8DDD9F8B47C4FEA4AA55CB91FC675EA2",
"objectName" : "柱子",
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
	string register_class;
	if (data.isMember("RegisterClass") && data["RegisterClass"].isString())
	{
		register_class = data["RegisterClass"].asString();
		this->SetRegisterClass(register_class);
	}

	// 设置地标类型
	if (register_class == "FXRPillar")
	{

	}
	// 初始化位置
	if (data.isMember("pos") && data["pos"].isString())
	{
		string pos_tmp = data["pos"].asString();
		this->center = Point3f(ComUtil::StringToPoint3f(pos_tmp));
	}



	// 初始化编号
	if (data.isMember("Type") && data["Type"].isString())
	{
		string type = data["Type"].asString();

		// 柱子代替马桶
		if (type == "OT_Pillar")
		{
			this->floor_box_type = FB_STOOL;
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

			// 取坐标点
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
	// 初始化编号
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

	// 初始化位置
	if (data.isMember("pos") && data["pos"].isString())
	{
		string pos_tmp = data["pos"].asString();
		this->center = Point3f(ComUtil::StringToPoint3f(pos_tmp));
	}

	// 初始化长度
	this->length = static_cast<float>(ComUtil::GetJsonIntByKey(data, "length"));
	// 初始化宽度
	this->width = static_cast<float>(ComUtil::GetJsonIntByKey(data, "width"));
	// 初始化高度
	this->height = static_cast<float>(ComUtil::GetJsonIntByKey(data, "height"));

}
