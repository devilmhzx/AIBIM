#include "Design.h"
#include <set>


Design::Design()
{
	version = 10000;
	space_id = 0;
}


Design::~Design()
{
}

/*****************************************************************
*  ������:����BIM�����������
*  ��������:
*
*  @param data json����
*
*****************************************************************/
void Design::LoadBimData(Json::Value data)
{
	if (data.isMember("hardMode") && data["hardMode"].isObject())
	{
		Json::Value hardMode = data["hardMode"];

		// ��ʼ���汾��
		if (hardMode.isMember("version") && hardMode["version"].isInt())
		{
			this->version = hardMode["version"].asInt();
		}

		// ��ʼ��ģ���б�
		if (hardMode.isMember("moveableMeshList") && hardMode["moveableMeshList"].isArray())
		{
			Json::Value models = hardMode["moveableMeshList"];
			for (unsigned int i = 0; i < models.size(); i++)
			{
				if (models[i].isObject())
				{
					Json::Value tmp_model_value = models[i];
					Model tem_model = Model();
					tem_model.LoadBimData(tmp_model_value);
					this->model_list.push_back(tem_model);
				}
			}
		}

		// ��ʼ����������-hardMode_RoomSpaces
		if (hardMode.isMember("hardMode_RoomSpaces") && hardMode["hardMode_RoomSpaces"].isArray())
		{
			hard_room_space = hardMode["hardMode_RoomSpaces"];
		}
	}

	// ��ʼ����������-hydropowerMode
	if (data.isMember("hydropowerMode") && data["hydropowerMode"].isObject())
	{
		hydropower_model = data["hydropowerMode"];
	}
	
	// ��ʼ����������-ConstructionModel
	if (data.isMember("ConstructionModel") && data["ConstructionModel"].isObject())
	{
		construction_model = data["ConstructionModel"];
	}
	

	// ��ʼ����������-fileList
	if (data.isMember("fileList") && data["fileList"].isArray())
	{
		file_list = data["fileList"];
	}

	// ��ʼ��ģ������
	InitModel();
}

/*****************************************************************
*  ������: ����App���������������
*  ��������:
*
*  @param data json����
*
*****************************************************************/
void Design::LoadAppSingleData(Json::Value data)
{
	if (data.isMember("hardMode") && data["hardMode"].isObject())
	{
		Json::Value hardMode = data["hardMode"];

		// ��ʼ���汾��
		if (hardMode.isMember("version") && hardMode["version"].isInt())
		{
			this->version = hardMode["version"].asInt();
		}

		// ��ʼ��ģ���б�
		if (hardMode.isMember("moveableMeshList") && hardMode["moveableMeshList"].isArray())
		{
			Json::Value models = hardMode["moveableMeshList"];
			for (unsigned int i = 0; i < models.size(); i++)
			{
				if (models[i].isObject())
				{
					Json::Value tmp_model_value = models[i];
					Model tem_model = Model();
					tem_model.LoadBimData(tmp_model_value);
					this->model_list.push_back(tem_model);
				}
			}
		}
	}
	// ��ʼ��ģ������
	InitModel();
}

/*****************************************************************
*  ������: ����Bim���������������
*  ��������:
*
*  @param data json����
*
*****************************************************************/
void Design::LoadBimSingleData(Json::Value data)
{
	if (data.isMember("hardMode") && data["hardMode"].isObject())
	{
		Json::Value hardMode = data["hardMode"];

		// ��ʼ���汾��
		if (hardMode.isMember("version") && hardMode["version"].isInt())
		{
			this->version = hardMode["version"].asInt();
		}

		// ��ʼ��ģ���б�
		if (hardMode.isMember("moveableMeshList") && hardMode["moveableMeshList"].isArray())
		{
			Json::Value models = hardMode["moveableMeshList"];
			for (unsigned int i = 0; i < models.size(); i++)
			{
				if (models[i].isObject())
				{
					Json::Value tmp_model_value = models[i];
					Model tem_model = Model();
					tem_model.LoadBimData(tmp_model_value);
					this->model_list.push_back(tem_model);
				}
			}
		}
	}
	// ��ʼ��ģ������
	InitModel();
}
/*****************************************************************
*  ������: ����App�����������
*  ��������:
*
*  @param data json����
*
*****************************************************************/
void Design::LoadAppData(Json::Value data)
{
	if (data.isMember("hardMode") && data["hardMode"].isObject())
	{
		Json::Value hardMode = data["hardMode"];

		// ��ʼ���汾��
		if (hardMode.isMember("version") && hardMode["version"].isInt())
		{
			this->version = hardMode["version"].asInt();
		}

		// ��ʼ��ģ���б�
		if (hardMode.isMember("roomList") && hardMode["roomList"].isArray())
		{
			Json::Value room_list = hardMode["roomList"];
			for (unsigned int i = 0; i < room_list.size(); i++)
			{
				Json::Value room = room_list[i];
				if (room.isObject())
				{
					string room_no = "";
					if (room.isMember("room_no") && room["room_no"].isString())
					{
						room_no = room["room_no"].asString();
					}
					if (room.isMember("moveableMeshList") && room["moveableMeshList"].isArray())
					{
						Json::Value models = room["moveableMeshList"];
						for (unsigned int j = 0; j < models.size(); j++)
						{
							if (models[j].isObject())
							{
								Json::Value tmp_model_value = models[j];
								Model tem_model = Model();
								tem_model.LoadBimData(tmp_model_value);
								tem_model.room_no = room_no;
								this->model_list.push_back(tem_model);
							}
						}
					}
				}
			}
		}
	}
}
/*****************************************************************
*  ������:���л������������ΪBim����
*  ��������:
*
*  @param data json����
*
*****************************************************************/
Json::Value Design::DumpBimData(int index)
{
	Json::Value design_value; //������ڵ�
	Json::Value hard_mode;
	hard_mode["version"] = version;
	if (model_list.size() < 1)
	{
		hard_mode["moveableMeshList"].resize(0);
	}
	else
	{
		Json::Value modelsObj;
		vector<Model> tmp_list = layout_list[index].model_list;
		for (vector<Model>::iterator model = tmp_list.begin(); model != tmp_list.end(); model++)
		{
			Json::Value model_value = model->DumpBimData();
			modelsObj.append(model_value);
		}
		hard_mode["moveableMeshList"] = modelsObj;
	}
	
	if (!hard_room_space.isNull())
	{
		hard_mode["hardMode_RoomSpaces"] = hard_room_space;
	}

	design_value["hardMode"] = hard_mode;


	if (!hydropower_model.isNull())
	{
		design_value["hydropowerMode"] = hydropower_model;
	}

	if (!construction_model.isNull())
	{
		design_value["ConstructionModel"] = construction_model;
	}

	if (!file_list.isNull())
	{
		design_value["fileList"] = file_list;
	}

	return design_value;
}

// ͨ��������ģ���б�
vector<Model> Design::GetModelsByRoomNo(string room_no)
{
	vector<Model> temp_list;
	for (vector<Model>::iterator model = model_list.begin(); model != model_list.end(); model++)
	{
		//if (room_no == model->room_no)
		//{
			temp_list.push_back(*model);
		//}
	}
	return temp_list;
}

// ͨ��ģ�ͱ�Ų���ģ��
Model* Design::GetModelByNo(string model_no)
{
	for (vector<Model>::iterator model = model_list.begin(); model != model_list.end(); model++)
	{
		if (model_no == model->GetNo())
		{
			return &(*model);
		}
	}

	return NULL;
}

// ͨ��ģ��id����ģ��
Model* Design::GetModelById(int model_id)
{
	for (vector<Model>::iterator model = model_list.begin(); model != model_list.end(); model++)
	{
		if (model_id == model->id)
		{
			return &(*model);
		}
	}

	return NULL;
}

// ��ȥ�ظ��Ĳ���
void Design::RomoveSameLayout()
{
	 //��ȡ�����������
 
	std::set<int> remove_list;
	for (int i = 0; i < layout_list.size(); ++i)
	{
		if (remove_list.find(i) != remove_list.end())
			continue;
		for (int j = 0; j < layout_list.size(); ++j)
		{
			if (i == j)
				continue;
			if (remove_list.find(j) != remove_list.end())
				continue;
			//ģ������һ��
			if (layout_list[i].model_list.size() == layout_list[j].model_list.size())
			{
				bool is_same = true;
				//���ģ���ǲ���һ��
				std::set<int>  model_id_this;
				for (auto model : layout_list[i].model_list)
					model_id_this.insert(model.id);

				std::set<int>	  model_id_other;
				for(auto model: layout_list[j].model_list)
					model_id_other.insert(model.id);

				for (auto id : model_id_this) {
					if (model_id_other.find(id) == model_id_other.end()) {
						is_same = false;
						break;
					}
				}
				//��ʱģ��idһ��
				if (is_same)
				{


					for (auto & this_model: layout_list[i].model_list)
					{
						for (auto & compare:layout_list[j].model_list)
						{
							//ģ��idһ��,�Ƚ�
							if( this_model.id != 30016&& this_model.id== compare.id)
								if (!ComMath::IsSameVectorOnApproximatly(this_model.location, compare.location, 0.1f))
									is_same = false;
								
						}
					}
				}
				if (is_same)
					remove_list.insert(j);
			}
		}
	}

	std::vector<ModelLayout> Layout;
	for (int i = 0; i < layout_list.size(); ++i)
	{
		if (remove_list.find(i) == remove_list.end())
			Layout.push_back(std::move(layout_list[i]));
	}
	layout_list = Layout;

}

///////////////////////////////˽�з���////////////////////////////
// �������ݿ���Ϣ��ʼ��ģ�������Ϣ
void Design::InitModel()
{
	shared_ptr<DatabaseHelper> p_db = std::make_shared<DatabaseHelper>();
	if (!p_db)
		return;
	for (vector<Model>::iterator model = model_list.begin(); model != model_list.end(); model++)
	{
		Model tmp_model = p_db->searchModelInfoById(model->id);
		if (tmp_model.id < 0)
		{
			return;
		}

		// ���ַ�ʽ
		model->layout_type = tmp_model.layout_type;
		model->SetNo(tmp_model.GetNo());
		//model->width = tmp_model.width;
		//model->height = tmp_model.height;
		//model->length = tmp_model.length;
		//model->heiht_to_floor = tmp_model.heiht_to_floor;
	}
}