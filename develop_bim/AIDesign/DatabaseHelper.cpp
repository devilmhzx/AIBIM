/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  Author:changyuan
*  Create Date:2018-09-19
*  Description:���ݿ������
**************************************************/

#include "Log/easylogging++.h"
#include "DatabaseHelper.h"
#include "CommonSettings.h"

DatabaseHelper::DatabaseHelper()
{
	db_name = ComUtil::db_path;
}


DatabaseHelper::~DatabaseHelper()
{
}


//std::mutex DatabaseHelper::db_mutex;
//DatabaseHelper* DatabaseHelper::p = NULL;
//DatabaseHelper* DatabaseHelper::initance()
//{
//	if (p == NULL)
//	{
//		db_mutex.lock();
//		if (p == NULL)
//			p = new DatabaseHelper();
//		db_mutex.unlock();
//	}
//	return p;
//}

// ��ʼ�����ݿ�
bool DatabaseHelper::initDB()
{
	create();
	if (!isExitsTable("modelinfo"))
	{
		if (!createModelTable())
		{
			return false;
		}
	}

	if (!isExitsTable("sampleroom"))
	{
		if (!createSampleRoomTable())
		{
			return false;
		}
	}

	if (!isExitsTable("modelregulation"))
	{
		if (!createRegularTable())
		{
			return false;
		}
	}

	if (!isExitsTable("roomfunction"))
	{
		if (!createRoomFunctionTable())
		{
			return false;
		}
	}


	return true;
}
// ͨ��ģ�ͱ�ż�������
vector<string> DatabaseHelper::searchRegualrtionsByModel(const int modelId)
{
	vector<string> regulation_list;
	string sql = "select regualtion from modelregulation where model_id='" + to_string(modelId) + "';";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;
	// �����ݿ�
	if (!open())
	{
		return regulation_list;
	}
	int n_res = sqlite3_get_table(db, sql.c_str(), &p_result, &n_row, &n_col, &c_errmsg);
	if (n_res != SQLITE_OK)
	{
		sqlite3_free_table(p_result);
		close();
		return regulation_list;
	}

	for (int i = 0; i < n_row; i++)
	{
		int index = (i+1)* n_col;
		string str = ComUtil::charToString(p_result[index]);
		//str = "id=" + to_string(modelId) + str;
		regulation_list.push_back(str);
	}

	sqlite3_free_table(p_result);
	close();
	return regulation_list;

}

// ͨ���������ͼ�����������ֵ
vector<SampleRoom> DatabaseHelper::searchSampleRoomByType(const int roomType)
{
	vector<SampleRoom> sample_list;
	string sql = "select no,room_type,design_no,data from sampleroom where room_type='" + to_string(roomType) + "';";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;

	// �����ݿ�
	if (!open())
	{
		return sample_list;
	}

	int n_res = sqlite3_get_table(db, sql.c_str(), &p_result, &n_row, &n_col, &c_errmsg);
	if (n_res != SQLITE_OK)
	{
		sqlite3_free_table(p_result);
		return sample_list;
	}

	for (int i = 0; i < n_row; i++)
	{
		int index = (i + 1) * n_col;
		SampleRoom *sample = new SampleRoom;
		sample->no = ComUtil::charToString(p_result[index]);
		sample->room_type = atoi(p_result[index + 1]);
		sample->design_no = ComUtil::charToString(p_result[index+2]);
		sample->data = ComUtil::charToString(p_result[index + 3]);
		sample->data_path = ComUtil::GetSampleDataPath(sample->no);
		sample_list.push_back(*sample);
	}

	sqlite3_free_table(p_result);
	close();
	return sample_list;
}


// �������е�ģ������
vector<SampleRoom> DatabaseHelper::searchAllSampleRoom()
{
	vector<SampleRoom> sample_list;
	string sql = "select no,room_type,design_no,data from sampleroom ;";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;

	// �����ݿ�
	if (!open())
	{
		return sample_list;
	}

	int n_res = sqlite3_get_table(db, sql.c_str(), &p_result, &n_row, &n_col, &c_errmsg);
	if (n_res != SQLITE_OK)
	{
		sqlite3_free_table(p_result);
		return sample_list;
	}

	for (int i = 0; i < n_row; i++)
	{
		int index = (i + 1) * n_col;
		SampleRoom *sample = new SampleRoom;
		sample->no = ComUtil::charToString(p_result[index]);
		sample->room_type = atoi(p_result[index + 1]);
		sample->design_no = ComUtil::charToString(p_result[index + 2]);
		sample->data = ComUtil::charToString(p_result[index + 3]);
		sample->data_path = ComUtil::GetSampleDataPath(sample->no);
		sample_list.push_back(*sample);
	}

	sqlite3_free_table(p_result);
	close();
	return sample_list;
}

// �������������
bool  DatabaseHelper::insertSampleRoom(SampleRoom sample)
{
	string sql = "replace into sampleroom(no,room_type,design_no,data) values('" + sample.no + "','" + to_string(sample.room_type) + "','" + sample.design_no + "','" + sample.data + "');";
	char* c_errmsg;

	// �����ݿ�
	if (!open())
	{
		return false;
	}
	int n_res = sqlite3_exec(db, sql.c_str(), NULL,NULL, &c_errmsg);
	if (n_res != SQLITE_OK)
	{
		close();
		return false;
	}

	close();
	return true;
}

// ͨ��ģ�ͱ�ź�������ż�������
vector<string> DatabaseHelper::searchRegualrtionsByModelandSample(const int modelId, const string sampleNo)
{
	vector<string> regulation_list;
	string sql = "select regualtion from modelregulation where model_id='" + to_string(modelId) + "' and sample_no='" + sampleNo +"';";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;
	// �����ݿ�
	if (!open())
	{
		return regulation_list;
	}
	int n_res = sqlite3_get_table(db, sql.c_str(), &p_result, &n_row, &n_col, &c_errmsg);
	if (n_res != SQLITE_OK)
	{
		sqlite3_free_table(p_result);
		close();
		return regulation_list;
	}

	for (int i = 0; i < n_row; i++)
	{
		int index = (i + 1)* n_col;
		string str = ComUtil::charToString(p_result[index]);
		//str = "id=" + to_string(modelId) + str;
		regulation_list.push_back(str);
	}

	sqlite3_free_table(p_result);
	close();

	return  regulation_list;
	/*if (regulation_list.size() > 0)
	{
		return regulation_list;
	}
	else
	{
		// û���ҵ�ģ���Ӧ�Ĺ���ȥ��������ǰ2��
		LOG(INFO) << "The Template is not regualrtion.";
		vector<string> tmp_list = searchRegualrtionsByModel(modelId);
		if (tmp_list.size() < 3)
		{
			return  tmp_list;
		}
		regulation_list.push_back(tmp_list[0]);
		regulation_list.push_back(tmp_list[1]);
		return  regulation_list;
	}*/
	
}

// ͨ��ģ�ͱ�ź�������ż������򣬷��صĽ��Ϊ����ṹ���б�
vector<RegualrtionsStruct> DatabaseHelper::searchRegualrtionsStructByModelandSample(const int modelId, const string sampleNo)
{
	vector<RegualrtionsStruct> regulation_list;
	string sql = "select no,regualtion from modelregulation where model_id='" + to_string(modelId) + "' and sample_no='" + sampleNo + "';";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;
	// �����ݿ�
	if (!open())
	{
		return regulation_list;
	}
	int n_res = sqlite3_get_table(db, sql.c_str(), &p_result, &n_row, &n_col, &c_errmsg);
	if (n_res != SQLITE_OK)
	{
		sqlite3_free_table(p_result);
		close();
		return regulation_list;
	}

	for (int i = 0; i < n_row; i++)
	{
		RegualrtionsStruct regulation_struct;
		int index = (i + 1)* n_col;
		regulation_struct.no = ComUtil::charToString(p_result[index]);
		regulation_struct.regualrtion = ComUtil::charToString(p_result[index+1]);
		regulation_list.push_back(regulation_struct);
	}

	sqlite3_free_table(p_result);
	close();

	return  regulation_list;
}
// ͨ��ģ�ͱ�ż���ģ����ϸ��Ϣ
Model DatabaseHelper::searchModelInfoById(const int modelId)
{
	Model model;
	string sql = "select id,no,name,layout_type,direction,weight,length,width,height,heiht_to_floor from modelinfo where id='" + to_string(modelId)+"';";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;

	// �����ݿ�
	if (!open())
	{
		return model;
	}

	int n_res = sqlite3_get_table(db, sql.c_str(), &p_result, &n_row, &n_col, &c_errmsg);
	if (n_res != SQLITE_OK)
	{
		sqlite3_free_table(p_result);
		return model;
	}

	for (int i = 0; i < n_row; i++)
	{
		int index = (i+1) * n_col;
		model.id = atoi(p_result[index]);
		model.SetNo(ComUtil::charToString(p_result[index + 1]));
		model.name = ComUtil::charToString(p_result[index+2]);
		model.layout_type = atoi(p_result[index+3]);
		model.direction = atoi(p_result[index + 4]);
		model.weight = atoi(p_result[index+5]);
		model.length = float(atof(p_result[index+6]));
		model.width = float(atof(p_result[index + 7]));
		model.height = static_cast<float>(atof(p_result[index + 8]));
		model.heiht_to_floor = static_cast<float>(atof(p_result[index + 9]));

		break;
	}

	sqlite3_free_table(p_result);
	close();
	return model;
}

// ��������ģ����Ϣ
vector<Model>  DatabaseHelper::searchAllModelInfo() 
{
	vector<Model> model_list;
	string sql = "select id,no,name,layout_type,direction,weight,length,width,height,heiht_to_floor from modelinfo;";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;

	// �����ݿ�
	if (!open())
	{
		return model_list;
	}

	int n_res = sqlite3_get_table(db, sql.c_str(), &p_result, &n_row, &n_col, &c_errmsg);
	if (n_res != SQLITE_OK)
	{
		sqlite3_free_table(p_result);
		close();
		return model_list;
	}


	for (int i = 0; i < n_row; i++)
	{
		Model model;
		int index = (i + 1) * n_col;
		model.id = atoi(p_result[index]);
		model.SetNo(ComUtil::charToString(p_result[index + 1]));
		model.name = ComUtil::charToString(p_result[index + 2]);
		model.layout_type = atoi(p_result[index + 3]);
		model.direction = atoi(p_result[index + 4]);
		model.weight = atoi(p_result[index + 5]);
		model.length = static_cast<float>(atof(p_result[index + 6]));
		model.width = static_cast<float>(atof(p_result[index + 7]));
		model.height = static_cast<float>(atof(p_result[index + 8]));
		model.heiht_to_floor = static_cast<float>(atof(p_result[index + 9]));
		model_list.push_back(model);
	}

	sqlite3_free_table(p_result);
	close();
	return model_list;
}

// ͨ���������ͼ�����������ģ��
vector<RoomFunction> DatabaseHelper::searchRoomFunctionByType(const int roomType)
{
	vector<RoomFunction> function_list;
	string sql = "select id,model_inner_list,score from roomfunction where room_type= " + to_string(roomType)+"; ";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;

	// �����ݿ�
	if (!open())
	{
		return function_list;
	}

	int n_res = sqlite3_get_table(db, sql.c_str(), &p_result, &n_row, &n_col, &c_errmsg);
	if (n_res != SQLITE_OK)
	{
		sqlite3_free_table(p_result);
		close();
		return function_list;
	}


	for (int i = 0; i < n_row; i++)
	{
		RoomFunction funciton = RoomFunction();
		int index = (i + 1) * n_col;
		funciton.id = atoi(p_result[index]);
		string str_inner = ComUtil::charToString(p_result[index+1]);
		funciton.model_inner_list = ComUtil::StringToVector(str_inner,"#");
		funciton.score = atoi(p_result[index + 2]);

		function_list.push_back(funciton);
	}

	sqlite3_free_table(p_result);
	close();
	return function_list;
}

// ͨ�������ż�����������
string DatabaseHelper::searchRegualrtionsDescriptionByNo(const string no)
{
	string str_result = "";
	string sql = "select description from modelregulation where no='" + no + "';";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;
	// �����ݿ�
	if (!open())
	{
		return str_result;
	}
	int n_res = sqlite3_get_table(db, sql.c_str(), &p_result, &n_row, &n_col, &c_errmsg);
	if (n_res != SQLITE_OK)
	{
		sqlite3_free_table(p_result);
		close();
		return str_result;
	}

	for (int i = 0; i < n_row; i++)
	{
		int index = (i + 1)* n_col;
		str_result = ComUtil::Utf8Togbk(p_result[index]);
		break;
	}

	sqlite3_free_table(p_result);
	close();

	return  str_result;
}


// ͨ��ģ�ͺͷ����Ż�ù�������
string DatabaseHelper::searchRegualrtionsDescriptionByModel(const int space_id,const Model *p_model)
{
	string rule_description = "";
	if (space_id == CHUFANG || space_id == WEISHENGJIAN || space_id == ZHUWEISHENGJIAN || space_id == KEWEISHENGJIAN || space_id == KETING)
	{
		switch (p_model->id)
		{
			// �ع�
		    case 132:
				rule_description = "�����ع�����ã�һ�㽲��һ�����������ˮ�۵�һ������300mm����ˮ������һ����ˮ�ۺ�¯��֮������600mm�ĵ���������һ�����Լ�¯���һ������300mm�ı������������������Ա�֤����ʹ������\n";
				break;
			// ˮ��
			case 135:
				rule_description = "�������һ�㽫ˮ�ۺʹ������������ܷ��õ���Ķ�������һ���Ա���߿ռ������ʣ������������䡢¯��������ε�λ�ã��Ա���߳������Ч��";
				break;
			// ¯��
			case 136:
				rule_description = "��Ϊ����������������֮һ��¯���λ��һ����ڱ��䡢ˮ��֮����ã��������������״����ó�Ϊ�ȱ������Σ��Ա�֤��������\n";
				break;
			// ����
			case 126:
				rule_description = "������ڷ��ڳ���ʱ��һ�����������׶˻�β�ˡ����������Ž��ˣ��Ӳ���ȡˮ�������ϻ�����㣬���������Ķ���Ҳ����Ȼ�������ھ���Զ�ˣ���һ��¯������Ÿ��������÷���˲˻����һЩ\n";
				break;
			// ��Ͱ
			case 127:
				rule_description = "һ����������Ͱ��ֱ����������Ͱ��λ�ϣ�����������Ͱ��λ��Ųλ����200mm�ͺ����׳�ˮ����������һ�㲻�ƶ���Ͱλ��";
				break;

			// ��ԡ
			case 129:
				// ����
				rule_description = "���ڼ����������䣬��ԡ����ԡ����Ϊʹ��Ƶ�ʽϵ͡�ʹ��ʱ����ˮ�����豸�����ڿռ俿���ı߽Ǵ��ǽϺõ�ѡ��\n";
				break;
			// ԡ�ҹ�
			case 128:
				rule_description = "ԡ�ҹ���Ϊʹ����Ƶ�����豸����÷��ڶ����������ʹ�õ�λ���ϣ����ŵ��ܱߡ�\n";
				break;
			// ԡ��
			case 131:
				rule_description = "���ڿռ�ϴ�������䣬Ҳ����Ϊ���ر�����ԡ�ף��������������������ԡ��ʹ��ʱ����ˮ�������Է��ڿռ俿��ı߽Ǵ��ǽϺõ�ѡ��\n";
				break;
			// ϴ�»�
			case 125:
				rule_description = "������ռ��и���ģ�һ��Ὣϴ�»�Ҳ����������֮�С�ϴ�»���λ��û�н�����ֻҪ���԰ڷš���Ӱ��������Ʒ��ʹ�þ��պͶ��߼���";
				break;

			/** ���� */
			//��λɳ��
			case 111:
				rule_description = "Ϊ�˱�֤�ɹ����ã�ͬʱ���˻����һ����˽�ԣ�ɳ����Ϸ��ڿ���������������������ŽϽ���һ���Ϊ����\n";
				break;
			//��λɳ��-��
			case 112:
				rule_description = "Ϊ�˱�֤�ɹ����ã�ͬʱ���˻����һ����˽�ԣ�ɳ����Ϸ��ڿ���������������������ŽϽ���һ���Ϊ����\n";
				break;
			//��λɳ��-��
			case 113:
				rule_description = "Ϊ�˱�֤�ɹ����ã�ͬʱ���˻����һ����˽�ԣ�ɳ����Ϸ��ڿ���������������������ŽϽ���һ���Ϊ����\n";
				break;
			//�輸
			case 115:
				rule_description = "Ϊ�˱�֤�ɹ����ã�ͬʱ���˻����һ����˽�ԣ�ɳ����Ϸ��ڿ���������������������ŽϽ���һ���Ϊ����\n";

		default:
			break;
		}
		return rule_description;
	}

	rule_description = searchRegualrtionsDescriptionByNo(p_model->layout_rule);
	return rule_description;
}

// �����ݿ�
bool DatabaseHelper::open()
{
	int n_res = sqlite3_open(db_name.c_str(), &db);
	if (n_res != SQLITE_OK)
	{
		close();
		return false;
	}
	return true;
}

// �ر����ݿ�
void DatabaseHelper::close()
{
	sqlite3_close(db);
}

// �������ݿ�
bool DatabaseHelper::create()
{
	int n_res = sqlite3_open(db_name.c_str(), &db);
	if (n_res != SQLITE_OK)
	{
		close();
		return false;
	}

	close();
	return true;
}
// ����ģ�ͱ�
bool DatabaseHelper::createModelTable()
{
	string sql = "create table modelinfo("  \
		"id int primary key     not null," \
		"no           text    not null," \
		"name         text    ," \
		"layout_type         INT     not null," \
		"direction         INT     not null," \
		"weight       INT     not null," \
		"length       real    not null," \
		"width       real     not null," \
		"height       real    not null," \
		"heiht_to_floor       real not null,"\
		"create_date timestamp not null default(datetime('now','localtime')) ,"\
        "update_date timestamp not null default(datetime('now','localtime')))";

	/* Execute SQL statement */
	char* c_errmsg;
	// �����ݿ�
	if (!open())
	{
		return false;
	}
	int n_res = sqlite3_exec(db, sql.c_str(), 0, 0, &c_errmsg);
	if (n_res != SQLITE_OK)
	{
		close();
		return false;
	}
	else
	{
		close();
		return true;
	}
}

// ����������
bool DatabaseHelper::createSampleRoomTable()
{
	string sql = "create table sampleroom("  \
		"no     text   primary key not null ," \
		"room_type     int    not null," \
		"design_no      text not null ," \
		"data      text not null ,"\
		"create_date timestamp not null default(datetime('now','localtime')) ,"\
		"update_date timestamp not null default(datetime('now','localtime')))";

	/* Execute SQL statement */
	char* c_errmsg;
	// �����ݿ�
	if (!open())
	{
		return false;
	}
	int n_res = sqlite3_exec(db, sql.c_str(), 0, 0, &c_errmsg);
	if (n_res != SQLITE_OK)
	{
		close();
		return false;
	}
	else
	{
		close();
		return true;
	}
}

// ���������
bool DatabaseHelper::createRegularTable()
{
	string sql = "create table modelregulation("  \
		"no      text  primary key   not null ," \
		"model_id     int    not null," \
		"sample_no      text ," \
		"regualtion      text not null ," \
		"description      text ," \
		"create_date timestamp not null default(datetime('now','localtime')) ,"\
		"update_date timestamp not null default(datetime('now','localtime')),"\
		"foreign key(model_id) references modelinfo(id))";

	/* Execute SQL statement */
	char* c_errmsg;
	// �����ݿ�
	if (!open())
	{
		return false;
	}
	int n_res = sqlite3_exec(db, sql.c_str(), 0, 0, &c_errmsg);
	if (n_res != SQLITE_OK)
	{
		close();
		return false;
	}
	else
	{
		close();
		return true;
	}
}

// ����������ȫ��
bool DatabaseHelper::createRoomFunctionTable()
{
	string sql = "create table roomfunction("  \
		"id      int  primary key   not null ," \
		"room_type     int    not null," \
		"model_inner_list     text not null," \
		"score       int    not null ," \
		"create_date timestamp not null default(datetime('now','localtime')) ,"\
		"update_date timestamp not null default(datetime('now','localtime')))";

	/* Execute SQL statement */
	char* c_errmsg;
	// �����ݿ�
	if (!open())
	{
		return false;
	}
	int n_res = sqlite3_exec(db, sql.c_str(), 0, 0, &c_errmsg);
	if (n_res != SQLITE_OK)
	{
		close();
		return false;
	}
	else
	{
		close();
		return true;
	}
}
// ���Ƿ����
bool DatabaseHelper::isExitsTable(string tableName)
{
	string sql = "select count(*) from sqlite_master where type='table' and name='" + tableName+"';";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;
	// �����ݿ�
	if (!open())
	{
		return false;
	}
	int n_res = sqlite3_get_table(db, sql.c_str(), &p_result, &n_row, &n_col, &c_errmsg);
	if (n_res != SQLITE_OK)
	{
		sqlite3_free_table(p_result);
		close();
		return false;
	}
	int count = atoi(p_result[1]);
	if (count == 1)
	{
		close();
		return true;
	}
	else
	{
		close();
		return false;
	}
}