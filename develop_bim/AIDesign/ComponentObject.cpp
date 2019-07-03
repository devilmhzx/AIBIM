#include "ComponentObject.h"

ComponentObject::ComponentObject()
{

}

ComponentObject::~ComponentObject()
{

}

void ComponentObject::LoadBimComponentObject(Json::Value data)
{
	// 初始化位置
	if (data.isMember("postation") && data["postation"].isString())
	{
		string pos_tmp = data["postation"].asString();
		this->center = Point3f(ComUtil::StringToPoint3f(pos_tmp));
	}

	// 初始化点位的类型
	if (data.isMember("ComponentType") && data["ComponentType"].isInt())
	{
		int  type = data["ComponentType"].asInt();
		switch (type) 
		{
		/** 弱电箱 */
		case 15:
			this->componentJ_type = CJ_EleBoxL_Point;  
			break;
		/** 强电箱 */
		case 16:
			this->componentJ_type = CJ_EleBoxH_Point;   
			break;
		/** 马桶下水 */
		case 31:
			this->componentJ_type = CJ_Closestool;
			break;
		/** 地漏下水 */
		case 60:
			this->componentJ_type = CJ_Basin;
			break;
		/** 下水主管道 */
		case 61:
			this->componentJ_type = CJ_Drain_Point;
			break;
		/** 入户水点位 */
		case 62:
			this->componentJ_type = CJ_Water_Supply;
			break;
		/** 热水点位 */
		case 55:
			this->componentJ_type = CJ_Hot_Water;
			break;
		/** 冷水点位 */
		case 17:
			this->componentJ_type = CJ_Cold_Water;
			break;
		/** 中水点位 */
		case 1202:
			this->componentJ_type = CJ_Middle_Water;
			break;
		/** 燃气表 */
		case 1201:
			this->componentJ_type = CJ_Gas_Meter;
			break;
		/** 燃气主管 */
		case 1200:
			this->componentJ_type = CJ_Gas_Supervisor;
			break;
		/** 分集水器 */
		case 1203:
			this->componentJ_type = CJ_Water_Collector;
			break;
		/** 未添加 */
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
