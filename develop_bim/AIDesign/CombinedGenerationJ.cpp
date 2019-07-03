#include "CombinedGenerationJ.h"

CombinedGenerationJ::CombinedGenerationJ()
	:result(true)
{

}

CombinedGenerationJ::~CombinedGenerationJ()
{

}

bool CombinedGenerationJ::InitModelPackage(vector<Model>& in_model)
{
	if (in_model.empty())
	{
		return false;
	}

	/** 主位沙发 茶几 */
	package_function.insert(make_pair(package1_id, 1));

	/** 主位沙发 茶几 辅位沙发-左 */
	package_function.insert(make_pair(package2_id, 2));

	/** 主位沙发 茶几 辅位沙发-右 */
	package_function.insert(make_pair(package3_id, 3));

	/** 主位沙发 茶几 辅位沙发-左 辅位沙发-右 */
	package_function.insert(make_pair(package4_id, 4));

	end_model_package = make_shared<ModelPackageJ>();

	/** 生成包 */
	GeneratePackage(in_model);

	return true;
}

void CombinedGenerationJ::GeneratePackage(vector<Model>& in_model)
{
	/** 客厅模型ID */
	vector<int> model_id_vec;
	/** 包内模型 */
	vector<Model> temp_generate_model;
	/** 加入虚拟包以后的布局模型列表 */
	vector<Model> temp_model_list;

	/** 筛选客厅包内模型 */
	for (auto& model : in_model)
	{
		if (model.id == GROUP_MAIN_SOFA || model.id == GROUP_ASSIST_SOFA_L || model.id == GROUP_ASSIST_SOFA_R || model.id == GROUP_TEA_TABLE)
		{
			model_id_vec.push_back(model.id);
			temp_generate_model.push_back(model);
		}
		else
		{
			temp_model_list.push_back(model);
		}
	}

	in_model = temp_model_list;
	/** 排序 按小到大 */
	sort(model_id_vec.begin(), model_id_vec.end(), less<int>());

	/** 将model.id 做成包的标识 */
	for (auto&model_id : model_id_vec)
	{
		stringstream sstream;
		sstream << model_id;
		string id_str;
		sstream >> id_str;
		read_id_str += id_str;
	}

	/** 执行打包 */
	map<string, int>::iterator package_iter;
	package_iter = package_function.find(read_id_str);
	if (package_iter != package_function.end())
	{
		switch (package_iter->second)
		{
		case 1:
			SetPackage1(temp_generate_model);
			break;
		case 2:
			SetPackage2(temp_generate_model);
			break;
		case 3:
			SetPackage3(temp_generate_model);
			break;
		case 4:
			SetPackage4(temp_generate_model);
			break;
		default:
			break;
		}
	}
}

bool CombinedGenerationJ::AnalyzePackage(vector<ModelLayout>& in_Layout)
{
	bool out_result = true;
	/** 检索所有布局 */
	for (auto& layout : in_Layout)
	{
		/** 包内模型列表 */
		vector<Model> temp_generate_model_list;
		/** 解包完成后的模型列表 */
		vector<Model> temp_model_list;

		for (auto& model : layout.model_list)
		{
			if (model.id == VIRTUAL_VISITOR_ROOM)
			{
				if (model.location == Point3f(0.0, 0.0, 0.0))
				{
					/** 无方案 */
					out_result = false;
				}
				else
				{
					end_model_package->virtual_model = model;

					/** 执行解包 */
					map<string, int>::iterator package_iter;
					package_iter = package_function.find(end_model_package->package_no);
					if (package_iter != package_function.end())
					{
						GetPackage(end_model_package);
					}
					temp_generate_model_list = end_model_package->package_model_list;
				}
			}
			else
			{
				if (model.location != Point3f(0.0, 0.0, 0.0))
				{
					temp_model_list.push_back(model);
				}
			}
		}
		for (auto& model : temp_generate_model_list)
		{
			temp_model_list.push_back(model);
		}
		if (temp_model_list.size() == 0)
		{
			/** 无方案 */
			out_result = false;
		}
		layout.model_list = temp_model_list;
	}
	return out_result;
}

Model CombinedGenerationJ::GetVirtualModel()
{
	return end_model_package->virtual_model;
}

void CombinedGenerationJ::SetPackage1(vector<Model>& in_model)
{
	end_model_package->package_no = package1_id;
	end_model_package->virtual_model.id = VIRTUAL_VISITOR_ROOM;
	end_model_package->package_model_list = in_model;

	/** 主位沙发 茶几 */
	Model main_sofa;
	Model tea_table;
	for (auto& model : in_model)
	{
		if (model.id == GROUP_MAIN_SOFA)
		{
			main_sofa = model;
		}

		else if (model.id == GROUP_TEA_TABLE)
		{
			tea_table = model;
		}
	}
	/** 四个顶点 */
	vector<Point3f> vertex_point;

	/** 中心点 */
	Point3f main_sofa_center_point;
	Point3f tea_table_center_point;
	Point3f virtual_center_point;

	/********************************************
	* vector 存储点位顺序
	*   [0]top_left        [1]top_right
	*
	*
	*
	*
	*	[3]bottom_right    [2]bottom_right
	********************************************/
	/** 主位沙发 */
	vertex_point.push_back(Point3f(0, 0, 0));
	vertex_point.push_back(Point3f(0, main_sofa.width, 0));
	vertex_point.push_back(Point3f(-main_sofa.length, main_sofa.width, 0));
	vertex_point.push_back(Point3f(-main_sofa.length, 0, 0));
	main_sofa_center_point = Point3f(-main_sofa.length*0.5f, main_sofa.width*0.5f, 0.0f);

	/** 茶几 */
	tea_table_center_point = main_sofa_center_point + Point3f(-main_sofa.length*0.5f - 40.f - tea_table.length*0.5f, 0, 0);
	vertex_point.push_back(tea_table_center_point + Point3f(tea_table.length*0.5f, -tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(tea_table.length*0.5f, tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(-tea_table.length*0.5f, tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(-tea_table.length*0.5f, -tea_table.width*0.5f, 0));

	/** 计算虚拟包的长宽 */
	Point3f virtual_point_min(999999, 999999, 0);
	Point3f virtual_point_max(-999999, -999999, 0);
	for (auto&point : vertex_point)
	{
		if (virtual_point_min.x > point.x)
		{
			virtual_point_min.x = point.x;
		}
		if (virtual_point_min.y > point.y)
		{
			virtual_point_min.y = point.y;
		}
		if (virtual_point_max.x < point.x)
		{
			virtual_point_max.x = point.x;
		}
		if (virtual_point_max.y < point.y)
		{
			virtual_point_max.y = point.y;
		}
	}

	/** 包的长宽 */
	end_model_package->virtual_model.width = virtual_point_max.y - virtual_point_min.y;
	end_model_package->virtual_model.length = virtual_point_max.x - virtual_point_min.x;
	virtual_center_point = (virtual_point_max + virtual_point_min)*0.5f;

	/** 主位沙发 茶几 相对位置 */
	end_model_package->relative_position.insert(make_pair(GROUP_MAIN_SOFA, virtual_center_point - main_sofa_center_point));
	end_model_package->relative_position.insert(make_pair(GROUP_TEA_TABLE, virtual_center_point - tea_table_center_point));
}

void CombinedGenerationJ::SetPackage2(vector<Model>& in_model)
{
	end_model_package->package_no = package1_id;
	end_model_package->virtual_model.id = VIRTUAL_VISITOR_ROOM;
	end_model_package->package_model_list = in_model;

	/** 主位沙发 茶几 辅位沙发-左 */
	Model main_sofa;
	Model tea_table;
	Model assist_sofa_l;

	for (auto& model : in_model)
	{
		if (model.id == GROUP_MAIN_SOFA)
		{
			main_sofa = model;
		}

		else if (model.id == GROUP_TEA_TABLE)
		{
			tea_table = model;
		}

		else if (model.id == GROUP_ASSIST_SOFA_L)
		{
			assist_sofa_l = model;
		}
	}

	/** 四个顶点 */
	vector<Point3f> vertex_point;

	/** 中心点 */
	Point3f main_sofa_center_point;
	Point3f tea_table_center_point;
	Point3f assist_sofa_l_center_point;
	Point3f virtual_center_point;

	/********************************************
	* vector 存储点位顺序
	*   [0]top_left        [1]top_right
	*
	*
	*
	*
	*	[3]bottom_right    [2]bottom_right
	********************************************/
	/** 主位沙发 */
	vertex_point.push_back(Point3f(0, 0, 0));
	vertex_point.push_back(Point3f(0, main_sofa.width, 0));
	vertex_point.push_back(Point3f(-main_sofa.length, main_sofa.width, 0));
	vertex_point.push_back(Point3f(-main_sofa.length, 0, 0));
	main_sofa_center_point = Point3f(-main_sofa.length*0.5f, main_sofa.width*0.5f, 0.0f);

	/** 茶几 */
	tea_table_center_point = main_sofa_center_point + Point3f(-main_sofa.length*0.5f - 40.f - tea_table.length*0.5f, 0, 0);
	vertex_point.push_back(tea_table_center_point + Point3f(tea_table.length*0.5f, -tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(tea_table.length*0.5f, tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(-tea_table.length*0.5f, tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(-tea_table.length*0.5f, -tea_table.width*0.5f, 0));

	/** 辅位沙发-左 */
	assist_sofa_l_center_point = tea_table_center_point + Point3f(0, -tea_table.width*0.5f - 40.f - assist_sofa_l.length*0.5f, 0);
	vertex_point.push_back(assist_sofa_l_center_point + Point3f(assist_sofa_l.width*0.5f, -assist_sofa_l.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_l_center_point + Point3f(assist_sofa_l.width*0.5f, assist_sofa_l.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_l_center_point + Point3f(-assist_sofa_l.width*0.5f, assist_sofa_l.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_l_center_point + Point3f(-assist_sofa_l.width*0.5f, -assist_sofa_l.length*0.5f, 0));

	/** 计算虚拟包的长宽 */
	Point3f virtual_point_min(999999, 999999, 0);
	Point3f virtual_point_max(-999999, -999999, 0);

	for (auto&point : vertex_point)
	{
		if (virtual_point_min.x > point.x)
		{
			virtual_point_min.x = point.x;
		}
		if (virtual_point_min.y > point.y)
		{
			virtual_point_min.y = point.y;
		}
		if (virtual_point_max.x < point.x)
		{
			virtual_point_max.x = point.x;
		}
		if (virtual_point_max.y < point.y)
		{
			virtual_point_max.y = point.y;
		}
	}

	/** 包的长宽 */
	end_model_package->virtual_model.width = virtual_point_max.y - virtual_point_min.y;
	end_model_package->virtual_model.length = virtual_point_max.x - virtual_point_min.x;
	virtual_center_point = (virtual_point_max + virtual_point_min)*0.5f;

	/** 主位沙发 茶几 辅位沙发-左 相对位置 */
	end_model_package->relative_position.insert(make_pair(GROUP_MAIN_SOFA, virtual_center_point - main_sofa_center_point));
	end_model_package->relative_position.insert(make_pair(GROUP_TEA_TABLE, virtual_center_point - tea_table_center_point));
	end_model_package->relative_position.insert(make_pair(GROUP_ASSIST_SOFA_L, virtual_center_point - assist_sofa_l_center_point));
}

void CombinedGenerationJ::SetPackage3(vector<Model>& in_model)
{
	end_model_package->package_no = package1_id;
	end_model_package->virtual_model.id = VIRTUAL_VISITOR_ROOM;
	end_model_package->package_model_list = in_model;
	Model main_sofa;
	Model tea_table;
	Model assist_sofa_r;

	for (auto& model : in_model)
	{
		if (model.id == GROUP_MAIN_SOFA)
		{
			main_sofa = model;
		}

		else if (model.id == GROUP_TEA_TABLE)
		{
			tea_table = model;
		}
		else if (model.id == GROUP_ASSIST_SOFA_R)
		{
			assist_sofa_r = model;
		}
	}
	/** 四个顶点 */
	vector<Point3f> vertex_point;

	/** 中心点 */
	Point3f main_sofa_center_point;
	Point3f tea_table_center_point;
	Point3f assist_sofa_r_center_point;
	Point3f virtual_center_point;

	/********************************************
	* vector 存储点位顺序
	*   [0]top_left        [1]top_right
	*
	*
	*
	*
	*	[3]bottom_right    [2]bottom_right
	********************************************/
	/** 主位沙发 */
	vertex_point.push_back(Point3f(0, 0, 0));
	vertex_point.push_back(Point3f(0, main_sofa.width, 0));
	vertex_point.push_back(Point3f(-main_sofa.length, main_sofa.width, 0));
	vertex_point.push_back(Point3f(-main_sofa.length, 0, 0));
	main_sofa_center_point = Point3f(-main_sofa.length*0.5f, main_sofa.width*0.5f, 0.0f);

	/** 茶几 */
	tea_table_center_point = main_sofa_center_point + Point3f(-main_sofa.length*0.5f - 40.f - tea_table.length*0.5f, 0, 0);
	vertex_point.push_back(tea_table_center_point + Point3f(tea_table.length*0.5f, -tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(tea_table.length*0.5f, tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(-tea_table.length*0.5f, tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(-tea_table.length*0.5f, -tea_table.width*0.5f, 0));

	/** 辅位沙发-右 */
	assist_sofa_r_center_point = tea_table_center_point + Point3f(0, tea_table.width*0.5f + 40.f + assist_sofa_r.length*0.5f, 0);
	vertex_point.push_back(assist_sofa_r_center_point + Point3f(assist_sofa_r.width*0.5f, -assist_sofa_r.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_r_center_point + Point3f(assist_sofa_r.width*0.5f, assist_sofa_r.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_r_center_point + Point3f(-assist_sofa_r.width*0.5f, assist_sofa_r.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_r_center_point + Point3f(-assist_sofa_r.width*0.5f, -assist_sofa_r.length*0.5f, 0));

	/** 计算虚拟包的长宽 */
	Point3f virtual_point_min(999999, 999999, 0);
	Point3f virtual_point_max(-999999, -999999, 0);

	for (auto&point : vertex_point)
	{
		if (virtual_point_min.x > point.x)
		{
			virtual_point_min.x = point.x;
		}
		if (virtual_point_min.y > point.y)
		{
			virtual_point_min.y = point.y;
		}
		if (virtual_point_max.x < point.x)
		{
			virtual_point_max.x = point.x;
		}
		if (virtual_point_max.y < point.y)
		{
			virtual_point_max.y = point.y;
		}
	}

	/** 包的长宽 */
	end_model_package->virtual_model.width = virtual_point_max.y - virtual_point_min.y;
	end_model_package->virtual_model.length = virtual_point_max.x - virtual_point_min.x;
	virtual_center_point = (virtual_point_max + virtual_point_min)*0.5f;

	/** 主位沙发 茶几 辅位沙发-右 相对位置 */
	end_model_package->relative_position.insert(make_pair(GROUP_MAIN_SOFA, virtual_center_point - main_sofa_center_point));
	end_model_package->relative_position.insert(make_pair(GROUP_TEA_TABLE, virtual_center_point - tea_table_center_point));
	end_model_package->relative_position.insert(make_pair(GROUP_ASSIST_SOFA_R, virtual_center_point - assist_sofa_r_center_point));

}

void CombinedGenerationJ::SetPackage4(vector<Model>& in_model)
{
	end_model_package->package_no = package1_id;
	end_model_package->virtual_model.id = VIRTUAL_VISITOR_ROOM;
	end_model_package->package_model_list = in_model;
	Model main_sofa;
	Model tea_table;
	Model assist_sofa_l;
	Model assist_sofa_r;
	for (auto& model : in_model)
	{
		if (model.id == GROUP_MAIN_SOFA)
		{
			main_sofa = model;
		}

		else if (model.id == GROUP_TEA_TABLE)
		{
			tea_table = model;
		}
		else if (model.id == GROUP_ASSIST_SOFA_L)
		{
			assist_sofa_l = model;
		}
		else if (model.id == GROUP_ASSIST_SOFA_R)
		{
			assist_sofa_r = model;
		}
	}

	/** 四个顶点 */
	vector<Point3f> vertex_point;

	/** 中心点 */
	Point3f main_sofa_center_point;
	Point3f tea_table_center_point;
	Point3f assist_sofa_l_center_point;
	Point3f assist_sofa_r_center_point;
	Point3f virtual_center_point;

	/********************************************
	* vector 存储点位顺序
	*   [0]top_left        [1]top_right
	*
	*
	*
	*
	*	[3]bottom_right    [2]bottom_right
	********************************************/
	/** 主位沙发 */
	vertex_point.push_back(Point3f(0, 0, 0));
	vertex_point.push_back(Point3f(0, main_sofa.width, 0));
	vertex_point.push_back(Point3f(-main_sofa.length, main_sofa.width, 0));
	vertex_point.push_back(Point3f(-main_sofa.length, 0, 0));
	main_sofa_center_point = Point3f(-main_sofa.length*0.5f, main_sofa.width*0.5f, 0.0f);

	/** 茶几 */
	tea_table_center_point = main_sofa_center_point + Point3f(-main_sofa.length*0.5f - 40.f - tea_table.length*0.5f, 0, 0);
	vertex_point.push_back(tea_table_center_point + Point3f(tea_table.length*0.5f, -tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(tea_table.length*0.5f, tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(-tea_table.length*0.5f, tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(-tea_table.length*0.5f, -tea_table.width*0.5f, 0));

	/** 辅位沙发-左 */
	assist_sofa_l_center_point = tea_table_center_point + Point3f(0, -tea_table.width*0.5f - 40.f - assist_sofa_l.length*0.5f, 0);
	vertex_point.push_back(assist_sofa_l_center_point + Point3f(assist_sofa_l.width*0.5f, -assist_sofa_l.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_l_center_point + Point3f(assist_sofa_l.width*0.5f, assist_sofa_l.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_l_center_point + Point3f(-assist_sofa_l.width*0.5f, assist_sofa_l.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_l_center_point + Point3f(-assist_sofa_l.width*0.5f, -assist_sofa_l.length*0.5f, 0));

	/** 辅位沙发-右 */
	assist_sofa_r_center_point = tea_table_center_point + Point3f(0, tea_table.width*0.5f + 40.f + assist_sofa_r.length*0.5f, 0);
	vertex_point.push_back(assist_sofa_r_center_point + Point3f(assist_sofa_r.width*0.5f, -assist_sofa_r.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_r_center_point + Point3f(assist_sofa_r.width*0.5f, assist_sofa_r.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_r_center_point + Point3f(-assist_sofa_r.width*0.5f, assist_sofa_r.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_r_center_point + Point3f(-assist_sofa_r.width*0.5f, -assist_sofa_r.length*0.5f, 0));

	/** 计算虚拟包的长宽 */
	Point3f virtual_point_min(999999, 999999, 0);
	Point3f virtual_point_max(-999999, -999999, 0);

	for (auto&point : vertex_point)
	{
		if (virtual_point_min.x > point.x)
		{
			virtual_point_min.x = point.x;
		}
		if (virtual_point_min.y > point.y)
		{
			virtual_point_min.y = point.y;
		}
		if (virtual_point_max.x < point.x)
		{
			virtual_point_max.x = point.x;
		}
		if (virtual_point_max.y < point.y)
		{
			virtual_point_max.y = point.y;
		}
	}

	/** 包的长宽 */
	end_model_package->virtual_model.width = virtual_point_max.y - virtual_point_min.y;
	end_model_package->virtual_model.length = virtual_point_max.x - virtual_point_min.x;
	virtual_center_point = (virtual_point_max + virtual_point_min)*0.5f;

	/** 主位沙发 茶几 辅位沙发-左 相对位置 */
	end_model_package->relative_position.insert(make_pair(GROUP_MAIN_SOFA, virtual_center_point - main_sofa_center_point));
	end_model_package->relative_position.insert(make_pair(GROUP_TEA_TABLE, virtual_center_point - tea_table_center_point));
	end_model_package->relative_position.insert(make_pair(GROUP_ASSIST_SOFA_L, virtual_center_point - assist_sofa_l_center_point));
	end_model_package->relative_position.insert(make_pair(GROUP_ASSIST_SOFA_R, virtual_center_point - assist_sofa_r_center_point));
}

void CombinedGenerationJ::GetPackage(shared_ptr<ModelPackageJ>& in_model_package)
{
	/** 执行解包 */
	for (auto &model : in_model_package->package_model_list)
	{
		/** 查询包内存在的相对关系 */
		map<int, Point3f>::iterator relative_position_iter;
		relative_position_iter = in_model_package->relative_position.find(model.id);
		if (relative_position_iter != in_model_package->relative_position.end())
		{
			/** 对世界坐标处理 */
			Point3f src = relative_position_iter->second;
			float angle = static_cast<float>(in_model_package->virtual_model.rotation.y / 180.f * pi);
			float x = src.x*cosf(angle) - src.y*sinf(angle);
			float y = src.x*sinf(angle) + src.y*cosf(angle);
			Point3f point = Point3f(x, y, 0.f);
			model.location = point + in_model_package->virtual_model.location;
			model.layout_rule = in_model_package->virtual_model.layout_rule;
		}
	}

	/** 坐标转换 */
	for (auto &model : in_model_package->package_model_list)
	{
		if (model.id == GROUP_MAIN_SOFA)
		{
			model.rotation = in_model_package->virtual_model.rotation;
		}
		else if (model.id == GROUP_TEA_TABLE)
		{
			model.rotation = in_model_package->virtual_model.rotation;
		}
		else if (model.id == GROUP_ASSIST_SOFA_L)
		{
			if (in_model_package->virtual_model.rotation.y == 0.f)
				model.rotation.y = 270;
			else if (in_model_package->virtual_model.rotation.y == 90.f)
				model.rotation.y = 0;
			else if (in_model_package->virtual_model.rotation.y == 180.f)
				model.rotation.y = 90;
			else
				model.rotation.y = 180;
		}
		else if (model.id == GROUP_ASSIST_SOFA_R)
		{
			if (in_model_package->virtual_model.rotation.y == 0.f)
				model.rotation.y = 90;
			else if (in_model_package->virtual_model.rotation.y == 90.f)
				model.rotation.y = 180;
			else if (in_model_package->virtual_model.rotation.y == 180.f)
				model.rotation.y = 270;
			else if (in_model_package->virtual_model.rotation.y == 270.f)
				model.rotation.y = 0;
		}
	}
}


