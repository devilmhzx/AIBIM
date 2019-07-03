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

	/** ��λɳ�� �輸 */
	package_function.insert(make_pair(package1_id, 1));

	/** ��λɳ�� �輸 ��λɳ��-�� */
	package_function.insert(make_pair(package2_id, 2));

	/** ��λɳ�� �輸 ��λɳ��-�� */
	package_function.insert(make_pair(package3_id, 3));

	/** ��λɳ�� �輸 ��λɳ��-�� ��λɳ��-�� */
	package_function.insert(make_pair(package4_id, 4));

	end_model_package = make_shared<ModelPackageJ>();

	/** ���ɰ� */
	GeneratePackage(in_model);

	return true;
}

void CombinedGenerationJ::GeneratePackage(vector<Model>& in_model)
{
	/** ����ģ��ID */
	vector<int> model_id_vec;
	/** ����ģ�� */
	vector<Model> temp_generate_model;
	/** ����������Ժ�Ĳ���ģ���б� */
	vector<Model> temp_model_list;

	/** ɸѡ��������ģ�� */
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
	/** ���� ��С���� */
	sort(model_id_vec.begin(), model_id_vec.end(), less<int>());

	/** ��model.id ���ɰ��ı�ʶ */
	for (auto&model_id : model_id_vec)
	{
		stringstream sstream;
		sstream << model_id;
		string id_str;
		sstream >> id_str;
		read_id_str += id_str;
	}

	/** ִ�д�� */
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
	/** �������в��� */
	for (auto& layout : in_Layout)
	{
		/** ����ģ���б� */
		vector<Model> temp_generate_model_list;
		/** �����ɺ��ģ���б� */
		vector<Model> temp_model_list;

		for (auto& model : layout.model_list)
		{
			if (model.id == VIRTUAL_VISITOR_ROOM)
			{
				if (model.location == Point3f(0.0, 0.0, 0.0))
				{
					/** �޷��� */
					out_result = false;
				}
				else
				{
					end_model_package->virtual_model = model;

					/** ִ�н�� */
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
			/** �޷��� */
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

	/** ��λɳ�� �輸 */
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
	/** �ĸ����� */
	vector<Point3f> vertex_point;

	/** ���ĵ� */
	Point3f main_sofa_center_point;
	Point3f tea_table_center_point;
	Point3f virtual_center_point;

	/********************************************
	* vector �洢��λ˳��
	*   [0]top_left        [1]top_right
	*
	*
	*
	*
	*	[3]bottom_right    [2]bottom_right
	********************************************/
	/** ��λɳ�� */
	vertex_point.push_back(Point3f(0, 0, 0));
	vertex_point.push_back(Point3f(0, main_sofa.width, 0));
	vertex_point.push_back(Point3f(-main_sofa.length, main_sofa.width, 0));
	vertex_point.push_back(Point3f(-main_sofa.length, 0, 0));
	main_sofa_center_point = Point3f(-main_sofa.length*0.5f, main_sofa.width*0.5f, 0.0f);

	/** �輸 */
	tea_table_center_point = main_sofa_center_point + Point3f(-main_sofa.length*0.5f - 40.f - tea_table.length*0.5f, 0, 0);
	vertex_point.push_back(tea_table_center_point + Point3f(tea_table.length*0.5f, -tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(tea_table.length*0.5f, tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(-tea_table.length*0.5f, tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(-tea_table.length*0.5f, -tea_table.width*0.5f, 0));

	/** ����������ĳ��� */
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

	/** ���ĳ��� */
	end_model_package->virtual_model.width = virtual_point_max.y - virtual_point_min.y;
	end_model_package->virtual_model.length = virtual_point_max.x - virtual_point_min.x;
	virtual_center_point = (virtual_point_max + virtual_point_min)*0.5f;

	/** ��λɳ�� �輸 ���λ�� */
	end_model_package->relative_position.insert(make_pair(GROUP_MAIN_SOFA, virtual_center_point - main_sofa_center_point));
	end_model_package->relative_position.insert(make_pair(GROUP_TEA_TABLE, virtual_center_point - tea_table_center_point));
}

void CombinedGenerationJ::SetPackage2(vector<Model>& in_model)
{
	end_model_package->package_no = package1_id;
	end_model_package->virtual_model.id = VIRTUAL_VISITOR_ROOM;
	end_model_package->package_model_list = in_model;

	/** ��λɳ�� �輸 ��λɳ��-�� */
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

	/** �ĸ����� */
	vector<Point3f> vertex_point;

	/** ���ĵ� */
	Point3f main_sofa_center_point;
	Point3f tea_table_center_point;
	Point3f assist_sofa_l_center_point;
	Point3f virtual_center_point;

	/********************************************
	* vector �洢��λ˳��
	*   [0]top_left        [1]top_right
	*
	*
	*
	*
	*	[3]bottom_right    [2]bottom_right
	********************************************/
	/** ��λɳ�� */
	vertex_point.push_back(Point3f(0, 0, 0));
	vertex_point.push_back(Point3f(0, main_sofa.width, 0));
	vertex_point.push_back(Point3f(-main_sofa.length, main_sofa.width, 0));
	vertex_point.push_back(Point3f(-main_sofa.length, 0, 0));
	main_sofa_center_point = Point3f(-main_sofa.length*0.5f, main_sofa.width*0.5f, 0.0f);

	/** �輸 */
	tea_table_center_point = main_sofa_center_point + Point3f(-main_sofa.length*0.5f - 40.f - tea_table.length*0.5f, 0, 0);
	vertex_point.push_back(tea_table_center_point + Point3f(tea_table.length*0.5f, -tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(tea_table.length*0.5f, tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(-tea_table.length*0.5f, tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(-tea_table.length*0.5f, -tea_table.width*0.5f, 0));

	/** ��λɳ��-�� */
	assist_sofa_l_center_point = tea_table_center_point + Point3f(0, -tea_table.width*0.5f - 40.f - assist_sofa_l.length*0.5f, 0);
	vertex_point.push_back(assist_sofa_l_center_point + Point3f(assist_sofa_l.width*0.5f, -assist_sofa_l.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_l_center_point + Point3f(assist_sofa_l.width*0.5f, assist_sofa_l.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_l_center_point + Point3f(-assist_sofa_l.width*0.5f, assist_sofa_l.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_l_center_point + Point3f(-assist_sofa_l.width*0.5f, -assist_sofa_l.length*0.5f, 0));

	/** ����������ĳ��� */
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

	/** ���ĳ��� */
	end_model_package->virtual_model.width = virtual_point_max.y - virtual_point_min.y;
	end_model_package->virtual_model.length = virtual_point_max.x - virtual_point_min.x;
	virtual_center_point = (virtual_point_max + virtual_point_min)*0.5f;

	/** ��λɳ�� �輸 ��λɳ��-�� ���λ�� */
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
	/** �ĸ����� */
	vector<Point3f> vertex_point;

	/** ���ĵ� */
	Point3f main_sofa_center_point;
	Point3f tea_table_center_point;
	Point3f assist_sofa_r_center_point;
	Point3f virtual_center_point;

	/********************************************
	* vector �洢��λ˳��
	*   [0]top_left        [1]top_right
	*
	*
	*
	*
	*	[3]bottom_right    [2]bottom_right
	********************************************/
	/** ��λɳ�� */
	vertex_point.push_back(Point3f(0, 0, 0));
	vertex_point.push_back(Point3f(0, main_sofa.width, 0));
	vertex_point.push_back(Point3f(-main_sofa.length, main_sofa.width, 0));
	vertex_point.push_back(Point3f(-main_sofa.length, 0, 0));
	main_sofa_center_point = Point3f(-main_sofa.length*0.5f, main_sofa.width*0.5f, 0.0f);

	/** �輸 */
	tea_table_center_point = main_sofa_center_point + Point3f(-main_sofa.length*0.5f - 40.f - tea_table.length*0.5f, 0, 0);
	vertex_point.push_back(tea_table_center_point + Point3f(tea_table.length*0.5f, -tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(tea_table.length*0.5f, tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(-tea_table.length*0.5f, tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(-tea_table.length*0.5f, -tea_table.width*0.5f, 0));

	/** ��λɳ��-�� */
	assist_sofa_r_center_point = tea_table_center_point + Point3f(0, tea_table.width*0.5f + 40.f + assist_sofa_r.length*0.5f, 0);
	vertex_point.push_back(assist_sofa_r_center_point + Point3f(assist_sofa_r.width*0.5f, -assist_sofa_r.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_r_center_point + Point3f(assist_sofa_r.width*0.5f, assist_sofa_r.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_r_center_point + Point3f(-assist_sofa_r.width*0.5f, assist_sofa_r.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_r_center_point + Point3f(-assist_sofa_r.width*0.5f, -assist_sofa_r.length*0.5f, 0));

	/** ����������ĳ��� */
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

	/** ���ĳ��� */
	end_model_package->virtual_model.width = virtual_point_max.y - virtual_point_min.y;
	end_model_package->virtual_model.length = virtual_point_max.x - virtual_point_min.x;
	virtual_center_point = (virtual_point_max + virtual_point_min)*0.5f;

	/** ��λɳ�� �輸 ��λɳ��-�� ���λ�� */
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

	/** �ĸ����� */
	vector<Point3f> vertex_point;

	/** ���ĵ� */
	Point3f main_sofa_center_point;
	Point3f tea_table_center_point;
	Point3f assist_sofa_l_center_point;
	Point3f assist_sofa_r_center_point;
	Point3f virtual_center_point;

	/********************************************
	* vector �洢��λ˳��
	*   [0]top_left        [1]top_right
	*
	*
	*
	*
	*	[3]bottom_right    [2]bottom_right
	********************************************/
	/** ��λɳ�� */
	vertex_point.push_back(Point3f(0, 0, 0));
	vertex_point.push_back(Point3f(0, main_sofa.width, 0));
	vertex_point.push_back(Point3f(-main_sofa.length, main_sofa.width, 0));
	vertex_point.push_back(Point3f(-main_sofa.length, 0, 0));
	main_sofa_center_point = Point3f(-main_sofa.length*0.5f, main_sofa.width*0.5f, 0.0f);

	/** �輸 */
	tea_table_center_point = main_sofa_center_point + Point3f(-main_sofa.length*0.5f - 40.f - tea_table.length*0.5f, 0, 0);
	vertex_point.push_back(tea_table_center_point + Point3f(tea_table.length*0.5f, -tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(tea_table.length*0.5f, tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(-tea_table.length*0.5f, tea_table.width*0.5f, 0));
	vertex_point.push_back(tea_table_center_point + Point3f(-tea_table.length*0.5f, -tea_table.width*0.5f, 0));

	/** ��λɳ��-�� */
	assist_sofa_l_center_point = tea_table_center_point + Point3f(0, -tea_table.width*0.5f - 40.f - assist_sofa_l.length*0.5f, 0);
	vertex_point.push_back(assist_sofa_l_center_point + Point3f(assist_sofa_l.width*0.5f, -assist_sofa_l.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_l_center_point + Point3f(assist_sofa_l.width*0.5f, assist_sofa_l.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_l_center_point + Point3f(-assist_sofa_l.width*0.5f, assist_sofa_l.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_l_center_point + Point3f(-assist_sofa_l.width*0.5f, -assist_sofa_l.length*0.5f, 0));

	/** ��λɳ��-�� */
	assist_sofa_r_center_point = tea_table_center_point + Point3f(0, tea_table.width*0.5f + 40.f + assist_sofa_r.length*0.5f, 0);
	vertex_point.push_back(assist_sofa_r_center_point + Point3f(assist_sofa_r.width*0.5f, -assist_sofa_r.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_r_center_point + Point3f(assist_sofa_r.width*0.5f, assist_sofa_r.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_r_center_point + Point3f(-assist_sofa_r.width*0.5f, assist_sofa_r.length*0.5f, 0));
	vertex_point.push_back(assist_sofa_r_center_point + Point3f(-assist_sofa_r.width*0.5f, -assist_sofa_r.length*0.5f, 0));

	/** ����������ĳ��� */
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

	/** ���ĳ��� */
	end_model_package->virtual_model.width = virtual_point_max.y - virtual_point_min.y;
	end_model_package->virtual_model.length = virtual_point_max.x - virtual_point_min.x;
	virtual_center_point = (virtual_point_max + virtual_point_min)*0.5f;

	/** ��λɳ�� �輸 ��λɳ��-�� ���λ�� */
	end_model_package->relative_position.insert(make_pair(GROUP_MAIN_SOFA, virtual_center_point - main_sofa_center_point));
	end_model_package->relative_position.insert(make_pair(GROUP_TEA_TABLE, virtual_center_point - tea_table_center_point));
	end_model_package->relative_position.insert(make_pair(GROUP_ASSIST_SOFA_L, virtual_center_point - assist_sofa_l_center_point));
	end_model_package->relative_position.insert(make_pair(GROUP_ASSIST_SOFA_R, virtual_center_point - assist_sofa_r_center_point));
}

void CombinedGenerationJ::GetPackage(shared_ptr<ModelPackageJ>& in_model_package)
{
	/** ִ�н�� */
	for (auto &model : in_model_package->package_model_list)
	{
		/** ��ѯ���ڴ��ڵ���Թ�ϵ */
		map<int, Point3f>::iterator relative_position_iter;
		relative_position_iter = in_model_package->relative_position.find(model.id);
		if (relative_position_iter != in_model_package->relative_position.end())
		{
			/** ���������괦�� */
			Point3f src = relative_position_iter->second;
			float angle = static_cast<float>(in_model_package->virtual_model.rotation.y / 180.f * pi);
			float x = src.x*cosf(angle) - src.y*sinf(angle);
			float y = src.x*sinf(angle) + src.y*cosf(angle);
			Point3f point = Point3f(x, y, 0.f);
			model.location = point + in_model_package->virtual_model.location;
			model.layout_rule = in_model_package->virtual_model.layout_rule;
		}
	}

	/** ����ת�� */
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


