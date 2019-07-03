#include "ComponentObject.h"

ComponentObject::ComponentObject()
{

}

ComponentObject::~ComponentObject()
{

}

void ComponentObject::LoadBimComponentObject(Json::Value data)
{
	// ��ʼ��λ��
	if (data.isMember("postation") && data["postation"].isString())
	{
		string pos_tmp = data["postation"].asString();
		this->center = Point3f(ComUtil::StringToPoint3f(pos_tmp));
	}

	// ��ʼ����λ������
	if (data.isMember("ComponentType") && data["ComponentType"].isInt())
	{
		int  type = data["ComponentType"].asInt();
		switch (type) 
		{
		/** ������ */
		case 15:
			this->componentJ_type = CJ_EleBoxL_Point;  
			break;
		/** ǿ���� */
		case 16:
			this->componentJ_type = CJ_EleBoxH_Point;   
			break;
		/** ��Ͱ��ˮ */
		case 31:
			this->componentJ_type = CJ_Closestool;
			break;
		/** ��©��ˮ */
		case 60:
			this->componentJ_type = CJ_Basin;
			break;
		/** ��ˮ���ܵ� */
		case 61:
			this->componentJ_type = CJ_Drain_Point;
			break;
		/** �뻧ˮ��λ */
		case 62:
			this->componentJ_type = CJ_Water_Supply;
			break;
		/** ��ˮ��λ */
		case 55:
			this->componentJ_type = CJ_Hot_Water;
			break;
		/** ��ˮ��λ */
		case 17:
			this->componentJ_type = CJ_Cold_Water;
			break;
		/** ��ˮ��λ */
		case 1202:
			this->componentJ_type = CJ_Middle_Water;
			break;
		/** ȼ���� */
		case 1201:
			this->componentJ_type = CJ_Gas_Meter;
			break;
		/** ȼ������ */
		case 1200:
			this->componentJ_type = CJ_Gas_Supervisor;
			break;
		/** �ּ�ˮ�� */
		case 1203:
			this->componentJ_type = CJ_Water_Collector;
			break;
		/** δ��� */
		default:
			this->componentJ_type = CJ_Default_NO;
		}
	}	
}

WallBox ComponentObject::GetNewWallBox(ComponentObject & in_component_object)
{
	WallBox temp_wall_box = WallBox();


	return temp_wall_box;
}
