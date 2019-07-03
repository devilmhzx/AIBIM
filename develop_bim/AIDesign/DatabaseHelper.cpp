/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  Author:changyuan
*  Create Date:2018-09-19
*  Description:数据库相关类
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

// 初始化数据库
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
// 通过模型编号检索规则
vector<string> DatabaseHelper::searchRegualrtionsByModel(const int modelId)
{
	vector<string> regulation_list;
	string sql = "select regualtion from modelregulation where model_id='" + to_string(modelId) + "';";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;
	// 打开数据库
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

// 通过房间类型检索房间特征值
vector<SampleRoom> DatabaseHelper::searchSampleRoomByType(const int roomType)
{
	vector<SampleRoom> sample_list;
	string sql = "select no,room_type,design_no,data from sampleroom where room_type='" + to_string(roomType) + "';";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;

	// 打开数据库
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


// 检索所有的模板数据
vector<SampleRoom> DatabaseHelper::searchAllSampleRoom()
{
	vector<SampleRoom> sample_list;
	string sql = "select no,room_type,design_no,data from sampleroom ;";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;

	// 打开数据库
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

// 插入样板间数据
bool  DatabaseHelper::insertSampleRoom(SampleRoom sample)
{
	string sql = "replace into sampleroom(no,room_type,design_no,data) values('" + sample.no + "','" + to_string(sample.room_type) + "','" + sample.design_no + "','" + sample.data + "');";
	char* c_errmsg;

	// 打开数据库
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

// 通过模型编号和样板间编号检索规则
vector<string> DatabaseHelper::searchRegualrtionsByModelandSample(const int modelId, const string sampleNo)
{
	vector<string> regulation_list;
	string sql = "select regualtion from modelregulation where model_id='" + to_string(modelId) + "' and sample_no='" + sampleNo +"';";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;
	// 打开数据库
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
		// 没有找到模板对应的规则去整体规则的前2条
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

// 通过模型编号和样板间编号检索规则，返回的结果为规则结构体列表
vector<RegualrtionsStruct> DatabaseHelper::searchRegualrtionsStructByModelandSample(const int modelId, const string sampleNo)
{
	vector<RegualrtionsStruct> regulation_list;
	string sql = "select no,regualtion from modelregulation where model_id='" + to_string(modelId) + "' and sample_no='" + sampleNo + "';";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;
	// 打开数据库
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
// 通过模型编号检索模型详细信息
Model DatabaseHelper::searchModelInfoById(const int modelId)
{
	Model model;
	string sql = "select id,no,name,layout_type,direction,weight,length,width,height,heiht_to_floor from modelinfo where id='" + to_string(modelId)+"';";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;

	// 打开数据库
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

// 检索所有模型信息
vector<Model>  DatabaseHelper::searchAllModelInfo() 
{
	vector<Model> model_list;
	string sql = "select id,no,name,layout_type,direction,weight,length,width,height,heiht_to_floor from modelinfo;";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;

	// 打开数据库
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

// 通过房间类型检索房间所需模型
vector<RoomFunction> DatabaseHelper::searchRoomFunctionByType(const int roomType)
{
	vector<RoomFunction> function_list;
	string sql = "select id,model_inner_list,score from roomfunction where room_type= " + to_string(roomType)+"; ";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;

	// 打开数据库
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

// 通过规则编号检索规则描述
string DatabaseHelper::searchRegualrtionsDescriptionByNo(const string no)
{
	string str_result = "";
	string sql = "select description from modelregulation where no='" + no + "';";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;
	// 打开数据库
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


// 通过模型和房间编号获得规则描述
string DatabaseHelper::searchRegualrtionsDescriptionByModel(const int space_id,const Model *p_model)
{
	string rule_description = "";
	if (space_id == CHUFANG || space_id == WEISHENGJIAN || space_id == ZHUWEISHENGJIAN || space_id == KEWEISHENGJIAN || space_id == KETING)
	{
		switch (p_model->id)
		{
			// 地柜
		    case 132:
				rule_description = "厨房地柜的设置，一般讲究一金二银，即在水槽的一侧留足300mm的沥水区（银一），水槽和炉灶之间留足600mm的调理区（金一），以及炉灶的一侧留足300mm的备餐银区（银二），以保证满足使用需求\n";
				break;
			// 水槽
			case 135:
				rule_description = "厨房设计一般将水槽和窗户这两个不能放置吊柜的东西放在一起，以便提高空间利用率；或放置在与冰箱、炉灶呈三角形的位置，以便提高厨房活动的效率";
				break;
			// 炉灶
			case 136:
				rule_description = "作为厨房的三大工作中心之一，炉灶的位置一般会在冰箱、水槽之后放置，并与其呈三角形状，最好成为等边三角形，以保证动线最优\n";
				break;
			// 冰箱
			case 126:
				rule_description = "当冰箱摆放在厨房时，一般会放在提柜的首端或尾端。当放在离门近端，从餐桌取水果、饮料会更方便，厨房做饭的动线也更自然；当放在距门远端，则一般炉灶会离门更近，做好饭后端菜会更快一些\n";
				break;
			// 马桶
			case 127:
				rule_description = "一般卫生间马桶会直接坐落于马桶坑位上，这是由于马桶移位器挪位超过200mm就很容易冲水堵塞，所以一般不移动马桶位置";
				break;

			// 淋浴
			case 129:
				// 花洒
				rule_description = "对于兼用型卫生间，淋浴和淋浴房作为使用频率较低、使用时溅起水汽的设备，放在空间靠里侧的边角处是较好的选择。\n";
				break;
			// 浴室柜
			case 128:
				rule_description = "浴室柜作为使用最频繁的设备，最好放在动线上最便于使用的位置上，如门的周边。\n";
				break;
			// 浴缸
			case 131:
				rule_description = "对于空间较大的卫生间，也可以为其特别配置浴缸，以满足泡澡的需求；由于浴缸使用时会起水汽、所以放在空间靠侧的边角处是较好的选择。\n";
				break;
			// 洗衣机
			case 125:
				rule_description = "卫生间空间有富余的，一般会将洗衣机也置于卫生间之中。洗衣机的位置没有讲究，只要可以摆放、不影响其他物品的使用净空和动线即可";
				break;

			/** 客厅 */
			//主位沙发
			case 111:
				rule_description = "为了保证采光良好，同时主人活动具有一定隐私性，沙发组合放在客厅柜主进光口两侧中离门较近的一侧较为合适\n";
				break;
			//辅位沙发-左
			case 112:
				rule_description = "为了保证采光良好，同时主人活动具有一定隐私性，沙发组合放在客厅柜主进光口两侧中离门较近的一侧较为合适\n";
				break;
			//辅位沙发-右
			case 113:
				rule_description = "为了保证采光良好，同时主人活动具有一定隐私性，沙发组合放在客厅柜主进光口两侧中离门较近的一侧较为合适\n";
				break;
			//茶几
			case 115:
				rule_description = "为了保证采光良好，同时主人活动具有一定隐私性，沙发组合放在客厅柜主进光口两侧中离门较近的一侧较为合适\n";

		default:
			break;
		}
		return rule_description;
	}

	rule_description = searchRegualrtionsDescriptionByNo(p_model->layout_rule);
	return rule_description;
}

// 打开数据库
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

// 关闭数据库
void DatabaseHelper::close()
{
	sqlite3_close(db);
}

// 创建数据库
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
// 创建模型表
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
	// 打开数据库
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

// 创建样板间表
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
	// 打开数据库
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

// 创建规则表
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
	// 打开数据库
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

// 创建功能齐全表
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
	// 打开数据库
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
// 表是否存在
bool DatabaseHelper::isExitsTable(string tableName)
{
	string sql = "select count(*) from sqlite_master where type='table' and name='" + tableName+"';";
	char** p_result;
	char* c_errmsg;
	int n_row;
	int n_col;
	// 打开数据库
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