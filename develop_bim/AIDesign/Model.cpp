#include "Model.h"
#include "DatabaseHelper.h"


Model::Model()
{
	length = 0.0;
	width = 0.0;
	height = 0.0;
	layout_type = 0;
	weight = 0;
	heiht_to_floor = 0.0;
	direction = 0;
	inner_type = 0;

	name = "";
	rule_description = "";
	toggleMode = -1;
	scale = Point3f(1.0, 1.0, 1.0);
	is_delete = true;
	is_maker = false;
	
	id = DEFAULT_ID;
	product_id = DEFAULT_ID;
}

Model::Model(Model* model)
{
	this->id = model->id;
	this->product_id = model->product_id;
	this->length = model->length;
	this->width = model->width;
	this->height = model->height;
	this->layout_type = model->layout_type;
	this->heiht_to_floor = model->heiht_to_floor;
	this->direction = model->direction;
	this->inner_type = model->inner_type;
	this->name = model->name;
	this->is_delete = model->is_delete;
	this->is_maker = model->is_maker;
	this->location = model->location;
	this->SetNo(model->GetNo());
	this->SetObjName(model->GetObjName());
	this->SetPropertyFlag(model->GetPropertyFlag());
	this->SetRegisterClass(model->GetRegisterClass());
}

Model::~Model()
{
}

/*
"id":218,
"location":"X=50.111 Y=-226.457 Z=1.000",
"rotation":"P=0.000000 Y=0.000000 R=0.000000",
"scale":"X=1.000 Y=1.000 Z=1.000",
"toggleMode":-1,
"materialList":[

]
*/
/*****************************************************************
*  函数名:初始化模型数据
*  功能描述:
*
*  @param json数据
*
*****************************************************************/

void Model::LoadBimData(Json::Value data)
{
	// 初始化id
	if (data.isMember("aiCode") && data["aiCode"].isInt())
	{
		int i_id = data["aiCode"].asInt();

		this->id = i_id;
	}

	// 初始化商品id
	if (data.isMember("productId") && data["productId"].isInt())
	{
		int i_product_id = data["productId"].asInt();

		this->product_id = i_product_id;
	}

	// 初始化模型编号
	if (data.isMember("no") && data["no"].isString())
	{
		this->SetNo(data["no"].asString());
	}
	else
	{
		this->SetNo(ComUtil::getGuuidNo());
	}

	// 初始化位置
	if (data.isMember("location") && data["location"].isString())
	{
		string str_tmp = data["location"].asString();
		this->location = Point3f(ComUtil::StringToPoint3f(str_tmp));
	}

	// 初始化旋转
	if (data.isMember("rotation") && data["rotation"].isString())
	{
		string str_tmp = data["rotation"].asString();
		this->rotation = Point3f(ComUtil::StringToPoint3f(str_tmp));
	}

	// 初始化缩放
	if (data.isMember("scale") && data["scale"].isString())
	{
		string str_tmp = data["scale"].asString();
		this->scale = Point3f(ComUtil::StringToPoint3f(str_tmp));
	}


	// 初始化临时房间数据
	if (data.isMember("room_no") && data["room_no"].isString())
	{
		string str_tmp = data["room_no"].asString();
		this->room_no = str_tmp;
	}

	// 初始化toggleMode
	if (data.isMember("toggleMode") && data["toggleMode"].isInt())
	{
		int i_tmp = data["toggleMode"].asInt();
		this->toggleMode = i_tmp;
	}


    // 初始化宽度
 	if (data.isMember("width") && data["width"].isInt())
 	{
 		float width_tmp = static_cast<float>(data["width"].asInt());
 		//this->width = ComMath::Round23(width_tmp);
		this->width = width_tmp;
 	}
 	
 	
 	// 初始化高度
 	if (data.isMember("height") && data["height"].isInt())
 	{
 		float height_tmp = static_cast<float>(data["height"].asInt());
 		this->height = height_tmp;
 	}
 	// 初始化深度
 	if (data.isMember("length") && data["length"].isInt())
 	{
 		float length_tmp = static_cast<float>(data["length"].asInt());
		//this->length = ComMath::Round23(length_tmp);
		this->length = length_tmp;
 	}

	// 初始化是否可定制
	if (data.isMember("is_maker") && data["is_maker"].isInt())
	{
		int tpm = data["is_maker"].asInt();
		if(tpm == 1)
		{
			this->is_maker = true;
		}
		else
		{
			this->is_maker = false;
		}
	}
}

/*****************************************************************
*  函数名:模型数据转化为json
*  功能描述:
*
*  @param json数据
*
*****************************************************************/
/*
"id":218,
"location":"X=50.111 Y=-226.457 Z=1.000",
"rotation":"P=0.000000 Y=0.000000 R=0.000000",
"scale":"X=1.000 Y=1.000 Z=1.000",
"toggleMode":-1,
"materialList":[

]
*/
Json::Value Model::DumpBimData()
{
	Json::Value model_value; //定义根节点
	/** @纪仁泽 修改了ID */
	model_value["aiCode"] = id;
	model_value["productId"] = product_id;
	// 临时使用TODO
	//model_value["id"] = ComUtil::ModelToBim(id);
	//model_value["id"] = ModelConfig::GetBimIdByAiId(id);
	model_value["location"] = location.ToString();
	model_value["rotation"] = rotation.ToString();
	model_value["scale"] = scale.ToString();
	model_value["toggleMode"] = toggleMode;
	model_value["width"] = width;
	model_value["length"] = length;
	model_value["height"] = height;
	model_value["no"] = this->GetNo();
	if (is_maker)
	{
		model_value["is_maker"] = 1;
	}
	else
	{
		model_value["is_maker"] = 0;
	}

	//model_value["name"] = name;
	// 规则编号
	model_value["rule_no"] = layout_rule;
	// 规则描述
	shared_ptr<DatabaseHelper> p_db = std::make_shared<DatabaseHelper>();
	if (rule_description == "")
	{
		rule_description = p_db->searchRegualrtionsDescriptionByNo(layout_rule);
	}
	model_value["rule_description"] = ComUtil::gbkToUtf8(rule_description);
	model_value["materialList"].resize(0);

	return model_value;
}
// 装换位置
void Model::ConvertByModel(Model *src_model)
{
	rotation = src_model->rotation;
	location = src_model->location;
}