#include "CombinedGeneration.h"
#include "SingleCase.h"
#include <sstream>



//客厅包的长度,深度
#define package_1_width	  340.f
#define package_1_length  185.f
#define package_2_width	  260
#define package_2_length  185
#define package_3_width	  package_2_width
#define package_3_length  package_2_length	
#define package_4_width	  180
#define package_4_length  180

 //客厅包的内部家具相对位置
#define package_1_left_position	  Point3f(47.5f, -135.f, 0.f)
#define package_1_major_position  Point3f(-57.5f, 0.f, 0.f)
#define	package_1_tea_position    Point3f(47.5f, 0.f, 0.f)
#define	package_1_right_position  Point3f(47.5f, 135.f, 0.f)

#define	package_2_major_position  Point3f(-57.5f, -40.f, 0.f)
#define	package_2_tea_position    Point3f(47.5f, -40.f, 0.f)
#define	package_2_right_position  Point3f(47.5f, 90.f, 0.f)
 
#define	package_3_major_position  Point3f(-57.5f, 40.f, 0.f)
#define	package_3_tea_position    Point3f(47.5f, 40.f, 0.f)
#define	package_3_left_position	  Point3f(47.5f, -90.f, 0.f)
 
#define	package_4_major Point3f(-50.f, 0.f, 0.f)
#define	package_4_tea   Point3f(60.f, 0.f, 0.f)
 
 
//获取模型组,从虚拟包中
bool	CombinedGeneration::GetCacheByModelGroup(const std::vector<int> &group, VirtualModel &p_model)
{
	std::vector<int> id;
	for (auto &model : group)
		id.push_back(model);
	//生成id标识符
	std::string id_string=GetUniqueID(id);
	//根据id标识符号,在缓存包中查找
	auto iter = cache_package.find(id_string);
	if (iter == cache_package.end())
		return false;
	//得到虚拟包
	p_model = iter->second.virtual_model;
	return true;
}
 //获取虚拟包
bool	CombinedGeneration::GetModelGroupByVirtualModel(const VirtualModel&p_model, std::vector<Model> &group)
{
	//查询虚拟包的组别
	auto iter = cache_package.find(p_model.virtual_no);
	if (iter == cache_package.end())
		return false;
	for (auto &model : group)
	{
		//对比查询包中Inner Type 如果不对,返回错误
		auto model_point_iter=iter->second.local_location.find(model.id);
		if (model_point_iter == iter->second.local_location.end())
			return false;
		//对世界坐标处理
		Point3f src = model_point_iter->second;
		auto angle = p_model.model.rotation.y / 180.f * M_PI;
		auto x = src.x*cosf(angle) - src.y*sinf(angle);
		auto y = src.x*sinf(angle) + src.y*cosf(angle);
		Point3f point = Point3f(x, y, 0.f);
		model.location = point + p_model.model.location;
	}
	//处理第1个包的方向
	if (p_model.virtual_no == packag_no[0])
		ProcessRotateByPackage_1(p_model, group);
	//处理第2个包的方向
	else if (p_model.virtual_no == packag_no[1])
		ProcessRotateByPackage_2(p_model, group);
	//处理第3个包的方向
	else if (p_model.virtual_no == packag_no[2])
		ProcessRotateByPackage_3(p_model, group);
	//处理第4个包的方向
	else
		ProcessRotateByPackage_4(p_model, group);

	return true;
}
//解包的方向 package @1
void CombinedGeneration::ProcessRotateByPackage_1(const VirtualModel&p_model, std::vector<Model> &group)
{
	for (auto &model : group)
	{
		//此包中-主沙发,茶桌方向与包方向一致
		if (model.id == GROUP_MAIN_SOFA)
			model.rotation = p_model.model.rotation;
		else if (model.id == GROUP_TEA_TABLE)
			model.rotation = p_model.model.rotation;
		else if (model.id == GROUP_ASSIST_SOFA_R)
		{
			if (p_model.model.rotation.y == 0.f)
				model.rotation.y = 270;
			else if (p_model.model.rotation.y == 90.f)
				model.rotation.y = 0;
			else if (p_model.model.rotation.y == 180.f)
				model.rotation.y = 90;
			else
				model.rotation.y = 180;
		}
		else if (model.id == GROUP_ASSIST_SOFA_L)
		{
			if (p_model.model.rotation.y == 0.f)
				model.rotation.y = 90;
			else if (p_model.model.rotation.y == 90.f)
				model.rotation.y = 180;
			else if (p_model.model.rotation.y == 180.f)
				model.rotation.y = 270;
			else if (p_model.model.rotation.y == 270.f)
				model.rotation.y = 0;
		}
		model.layout_rule = p_model.model.layout_rule;
	}
}
//解包的方向 package @2
void CombinedGeneration::ProcessRotateByPackage_2(const VirtualModel&p_model, std::vector<Model> &group)
{
	for (auto &model : group)
	{
		if (model.id == GROUP_MAIN_SOFA)
			model.rotation = p_model.model.rotation;
		else if (model.id == GROUP_TEA_TABLE)
			model.rotation = p_model.model.rotation;
		else if (model.id == GROUP_ASSIST_SOFA_R)
		{
			if (p_model.model.rotation.y == 0.f)
				model.rotation.y = 270;
			else if (p_model.model.rotation.y == 90.f)
				model.rotation.y = 0;
			else if (p_model.model.rotation.y == 180.f)
				model.rotation.y = 90;
			else
				model.rotation.y = 180;
		}
		model.layout_rule = p_model.model.layout_rule;
	}
}
//解包的方向 package @3
void CombinedGeneration::ProcessRotateByPackage_3(const VirtualModel&p_model, std::vector<Model> &group)
{
	for (auto &model : group)
	{
		if (model.id == GROUP_MAIN_SOFA)
			model.rotation = p_model.model.rotation;
		else if (model.id == GROUP_TEA_TABLE)
			model.rotation = p_model.model.rotation;
		else if (model.id == GROUP_ASSIST_SOFA_L)
		{
			if (p_model.model.rotation.y == 0.f)
				model.rotation.y = 90;
			else if (p_model.model.rotation.y == 90.f)
				model.rotation.y = 180;
			else if (p_model.model.rotation.y == 180.f)
				model.rotation.y = 270;
			else if (p_model.model.rotation.y == 270.f)
				model.rotation.y = 0;
		}
		model.layout_rule = p_model.model.layout_rule;
	}
}
//解包的方向 package @4
void CombinedGeneration::ProcessRotateByPackage_4(const VirtualModel&p_model, std::vector<Model> &group)
{
	for (auto &model : group)
	{
		model.rotation = p_model.model.rotation;
		model.layout_rule = p_model.model.layout_rule;
	}
}
//唯一标识
std::string CombinedGeneration::GetUniqueID(std::vector<int>&model)
{
	std::sort(model.begin(), model.end(), std::less<int>());
	std::string id;
	for (auto i : model)
	{
		std::stringstream sstream;
		sstream << i;
		std::string  number;
		sstream >> number;
		id += number;
	}
	return id;
}
//初始化
void CombinedGeneration::InitializeModelPackage()
{
	//package 1:
	{
		std::vector<int>  real_id;
		//辅位沙发-左
		real_id.push_back(GROUP_ASSIST_SOFA_L);
		//三人沙发
		real_id.push_back(GROUP_MAIN_SOFA);
		//茶几
		real_id.push_back(GROUP_TEA_TABLE);
		//辅位沙发-右
		real_id.push_back(GROUP_ASSIST_SOFA_R);
		packag_no[0] = GetUniqueID(real_id);
		ModelPackage package;
		//虚拟模型初始化(virtual_no)
		package.virtual_model.virtual_no = packag_no[0];
		//虚拟模型初始化(width)
		package.virtual_model.model.width = package_1_width;
		//虚拟模型初始化(length)
		package.virtual_model.model.length = package_1_length;
		//虚拟模型初始化(id)
		package.virtual_model.model.id = VIRTUAL_VISITOR_ROOM;
		//插入包的相对位置
		package.local_location.insert(std::make_pair(GROUP_ASSIST_SOFA_L, package_1_left_position));
		package.local_location.insert(std::make_pair(GROUP_MAIN_SOFA, package_1_major_position));
		package.local_location.insert(std::make_pair(GROUP_TEA_TABLE, package_1_tea_position));
		package.local_location.insert(std::make_pair(GROUP_ASSIST_SOFA_R, package_1_right_position));

		cache_package.insert(std::make_pair(packag_no[0], package));

	}
	//package 2: 
	{
		std::vector<int>  real_id;
		//三人沙发
		real_id.push_back(GROUP_MAIN_SOFA);
		//茶几
		real_id.push_back(GROUP_TEA_TABLE);
		//辅助	
		real_id.push_back(GROUP_ASSIST_SOFA_R);

		packag_no[1] = GetUniqueID(real_id);
		ModelPackage package;
		//虚拟模型初始化(virtual_no)
		package.virtual_model.virtual_no = packag_no[1];
		//虚拟模型初始化(width)
		package.virtual_model.model.width = package_2_width;
		//虚拟模型初始化(length)
		package.virtual_model.model.length = package_2_length;
		//虚拟模型初始化(id)
		package.virtual_model.model.id =VIRTUAL_VISITOR_ROOM;
		//插入包的相对位置
		package.local_location.insert(std::make_pair(GROUP_MAIN_SOFA, package_2_major_position));
		package.local_location.insert(std::make_pair(GROUP_TEA_TABLE, package_2_tea_position));
		package.local_location.insert(std::make_pair(GROUP_ASSIST_SOFA_R, package_2_right_position));
		cache_package.insert(std::make_pair(packag_no[1], package));
	}
	//package 3: 
	{
		std::vector<int>  real_id;
		//三人沙发
		real_id.push_back(GROUP_MAIN_SOFA);
		//茶几
		real_id.push_back(GROUP_TEA_TABLE);
		//辅助
		real_id.push_back(GROUP_ASSIST_SOFA_L);

		packag_no[2] = GetUniqueID(real_id);
		ModelPackage package;
		//虚拟模型初始化(virtual_no)
		package.virtual_model.virtual_no = packag_no[2];
		//虚拟模型初始化(width)
		package.virtual_model.model.width = package_3_width;
		//虚拟模型初始化(length)
		package.virtual_model.model.length = package_3_length;
		//虚拟模型初始化(id)
		package.virtual_model.model.id =VIRTUAL_VISITOR_ROOM;
		//插入包的相对位置
		package.local_location.insert(std::make_pair(GROUP_MAIN_SOFA, package_3_major_position));
		package.local_location.insert(std::make_pair(GROUP_TEA_TABLE, package_3_tea_position));
		package.local_location.insert(std::make_pair(GROUP_ASSIST_SOFA_L, package_3_left_position));
		cache_package.insert(std::make_pair(packag_no[2], package));
	}
	//package 4: 
	{
		std::vector<int>  real_id;
		//三人沙发
		real_id.push_back(GROUP_MAIN_SOFA);
		//茶几
		real_id.push_back(GROUP_TEA_TABLE);


		packag_no[3] = GetUniqueID(real_id);
		ModelPackage package;
		//虚拟模型初始化(virtual_no)
		package.virtual_model.virtual_no = packag_no[3];
		//虚拟模型初始化(width)
		package.virtual_model.model.width = package_4_width;
		//虚拟模型初始化(length)
		package.virtual_model.model.length = package_4_width;
		//虚拟模型初始化(id)
		package.virtual_model.model.id =VIRTUAL_VISITOR_ROOM;
		//插入包的相对位置
		package.local_location.insert(std::make_pair(GROUP_MAIN_SOFA, package_4_major));
		package.local_location.insert(std::make_pair(GROUP_TEA_TABLE, package_4_tea));
		cache_package.insert(std::make_pair(packag_no[3], package));
	}

	visitor_package.insert(GROUP_ASSIST_SOFA_L);
	visitor_package.insert(GROUP_MAIN_SOFA);
	visitor_package.insert(GROUP_TEA_TABLE);
	visitor_package.insert(GROUP_ASSIST_SOFA_R);
}
// 通过SingleCase获取可能生成的包
std::vector<PackageStruct>  CombinedGeneration::GetCacheModelListBySingleCase(SingleCase*single_case)
{
	/** 2019-02-20 将构件ID更新 @纪仁泽 */

	std::vector<PackageStruct> packagelist;
	std::vector<int> model;
	for (int i = 0; i < single_case->model_list.size(); ++i)
	{
		if (visitor_package.find(single_case->model_list[i].id) == visitor_package.end())
			continue;
		model.push_back(single_case->model_list[i].id);
	}
	//客厅包
	VirtualModel v_model;
	if (GetCacheByModelGroup(model, v_model))
	{
		PackageStruct package;
		package.virtual_model= v_model;
		for (auto id : model)
		{
			for (auto &real_model : single_case->model_list)
			{
				if (id == real_model.id)
					package.mode_list.push_back(real_model);
			}
		}
		packagelist.push_back(package);
	}

	return packagelist;
}

std::unordered_map<std::string, ModelPackage>  CombinedGeneration::cache_package;
std::string  CombinedGeneration::packag_no[4];
std::set<int>	CombinedGeneration::visitor_package;