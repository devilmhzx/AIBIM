#include "TestData.h"



TestData::TestData()
{
	int index = 1;

	string base_path = "D://test/";
	// 方案1
	string src_hosue = base_path + "BIMDATA/1/DrawHome/1644_754d0229701f16f70e01f3f8aaaf1105.json";
	string src_design = base_path + "BIMDATA/1/LevelPlan/1517_9c09c84f3ab729fad9365f8b60f03116.json";
	string src_out = base_path + "BIMDATA/1/LevelPlan_New/1517_9c09c84f3ab729fad9365f8b60f03116";
	vector<string> tmp_vector;
	tmp_vector.push_back(src_hosue);
	tmp_vector.push_back(src_design);
	tmp_vector.push_back(src_out);
	src_maps[index++] = tmp_vector;

	// 方案2
	src_hosue = base_path + "BIMDATA/2/DrawHome/1644_a018c40a1910054f20598369e9628f1e.json";
	src_design = base_path + "BIMDATA/2/LevelPlan/1517_ef51114959e805f5b230f4439f9d66f0.json";
	src_out = base_path + "BIMDATA/2/LevelPlan_New/1517_ef51114959e805f5b230f4439f9d66f0";
	tmp_vector.clear();
	tmp_vector.push_back(src_hosue);
	tmp_vector.push_back(src_design);
	tmp_vector.push_back(src_out);
	src_maps[index++] = tmp_vector;

	// 方案3
	src_hosue = base_path + "BIMDATA/3/DrawHome/1644_99f972377b0657df9f7e3e352af6cf99.json";
	src_design = base_path + "BIMDATA/3/LevelPlan/1517_4403647ad2c1b4291d654791ec067829.json";
	src_out = base_path + "BIMDATA/3/LevelPlan_New/1517_4403647ad2c1b4291d654791ec067829";
	tmp_vector.clear();
	tmp_vector.push_back(src_hosue);
	tmp_vector.push_back(src_design);
	tmp_vector.push_back(src_out);
	src_maps[index++] = tmp_vector;

	// 方案4
	src_hosue = base_path + "BIMDATA/4/DrawHome/1644_d9910964c8a851b9abd28d166e7579af.json";
	src_design = base_path + "BIMDATA/4/LevelPlan/1517_941e8a1279f71ac6419e2c1ffdf8c65a.json";
	src_out = base_path + "BIMDATA/4/LevelPlan_New/1517_941e8a1279f71ac6419e2c1ffdf8c65a";
	tmp_vector.clear();
	tmp_vector.push_back(src_hosue);
	tmp_vector.push_back(src_design);
	tmp_vector.push_back(src_out);
	src_maps[index++] = tmp_vector;

	// 方案5
	src_hosue = base_path + "BIMDATA/5/DrawHome/1644_99340a2f111a15f4c68dc7268893674d.json";
	src_design = base_path + "BIMDATA/5/LevelPlan/1517_d530004539695f17efde3d0709e09863.json";
	src_out = base_path + "BIMDATA/5/LevelPlan_New/1517_d530004539695f17efde3d0709e09863";
	tmp_vector.clear();
	tmp_vector.push_back(src_hosue);
	tmp_vector.push_back(src_design);
	tmp_vector.push_back(src_out);
	src_maps[index++] = tmp_vector;

	// 方案6
	src_hosue = base_path + "BIMDATA/6/DrawHome/1944_6162df1249c641896edecd6a1bee3997.json";
	src_design = base_path + "BIMDATA/6/LevelPlan/1811_9d8e39d43d09ac940ad561b59cd03876.json";
	src_out = base_path + "BIMDATA/6/LevelPlan_New/1811_9d8e39d43d09ac940ad561b59cd03876";
	tmp_vector.clear();
	tmp_vector.push_back(src_hosue);
	tmp_vector.push_back(src_design);
	tmp_vector.push_back(src_out);
	src_maps[index++] = tmp_vector;

	// 方案6
	index = 301;
	src_hosue = base_path + "BIMDATA/301/DrawHome/2361_e90154a7f2d047855a586b82765ba9e3.json";
	src_design = base_path + "BIMDATA/301/LevelPlan/1811_9d8e39d43d09ac940ad561b59cd03876.json";
	src_out = base_path + "BIMDATA/301/LevelPlan_New/1811_9d8e39d43d09ac940ad561b59cd03876";
	tmp_vector.clear();
	tmp_vector.push_back(src_hosue);
	tmp_vector.push_back(src_design);
	tmp_vector.push_back(src_out);
	src_maps[index++] = tmp_vector;

	// 方案999
	src_hosue = base_path + "BIMDATA/999/DrawHome/2784_d9ff880440c65922bdad2ce76ca15c35.json";
	src_design = base_path + "BIMDATA/999/LevelPlan/1811_9d8e39d43d09ac940ad561b59cd03876.json";
	src_out = base_path + "BIMDATA/999/LevelPlan_New/1811_9d8e39d43d09ac940ad561b59cd03876";
	tmp_vector.clear();
	tmp_vector.push_back(src_hosue);
	tmp_vector.push_back(src_design);
	tmp_vector.push_back(src_out);
	src_maps[999] = tmp_vector;



}


TestData::~TestData()
{
}

TestData* TestData::p = new TestData;
TestData* TestData::initance()
{
	return p;
}


AICase *TestData::getAICaseById(int index, vector<int>model_list)
{
	if (map_case.count(index) < 1)
	{
		return NULL;
	}

	AICase * tmp_case = map_case[index];
	SingleCase *single_case = &(tmp_case->single_case_list[0]);
	shared_ptr<DatabaseHelper> db = std::make_shared<DatabaseHelper>();

	for (int i = 0; i < model_list.size();i++)
	{
		Model new_model = Model();
		Model tmp = db->searchModelInfoById(model_list[i]);
		if (tmp.id < 0)
		{
			continue;
		}

		new_model.id = tmp.id;
		new_model.SetNo(tmp.GetNo());
		new_model.layout_type = tmp.layout_type;
		new_model.weight = tmp.weight;
		new_model.width = tmp.width;
		new_model.height = tmp.height;
		new_model.length = tmp.length;
		new_model.heiht_to_floor = tmp.heiht_to_floor;
		new_model.name = tmp.name;
		new_model.room_no = single_case->single_room.GetNo();
		single_case->model_list.push_back(new_model);
		tmp_case->design.model_list.push_back(new_model);
	}
	return tmp_case;

}
	

string TestData::getOutPath(int index)
{
	if (src_maps.count(index) > 0)
	{
		return src_maps[index][2];
	}
	else
	{
		return "d://test/1000/data.json";
	}
}


bool TestData::initMapCase()
{
	map<int, vector<string>>::iterator iter;
	for (iter = src_maps.begin(); iter != src_maps.end(); iter++)
	{
		int key = iter->first;
		string src_houselayout = iter->second[0];
		string src_design = iter->second[1];
		AICase* src_case = new AICase();

		Json::Reader reader;
		Json::Value root;
		fstream f;

		// 初始化模板户型
		f.open(src_houselayout, ios::in);
		if (!f.is_open()) 
		{
			cout << "错误的户型为:" << key << endl;
			cout << "Open src_houselayout file error!="<<src_houselayout<< endl;
			return false;
		}
		else
		{
			if (reader.parse(f, root))
			{
				src_case->house.LoadBimData(root);
				f.close();
			}
			else
			{
				cout << "错误的户型为:" << key << endl;
				cout << "Open src_houselayout file error!=" << src_houselayout << endl;
				return false;
			}
		}

		// 初始化模板设计
		f.open(src_design, ios::in);
		if (!f.is_open()) 
		{
			cout << "错误的户型为:" << key << endl;
			cout << "Open src_design file error!=" << src_design << endl;
			return false;
		}
		else
		{
			if (reader.parse(f, root))
			{
				src_case->design.LoadBimData(root);
				f.close();
			}
			else
			{
				cout << "错误的户型为:" << key << endl;
				cout << "Open src_design file error!=" << src_design << endl;
				return false;
			}
		}

		src_case->Init();
		// 初始化单房间数据
		src_case->InitSingleCase();
		map_case.insert(make_pair(key, src_case));

	}

	return true;

}
