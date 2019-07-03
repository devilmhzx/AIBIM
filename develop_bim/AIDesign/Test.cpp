
#include "ComMath.h"
#include "ComUtil.h"
#include "Point3f.h"
#include "AICase.h"
#include "Room.h"
#include "Houselayout.h"
#include "AICore/MetisNode.h"
#include "AICore/MetisRegionResolver.h"
#include "AICore/MetisDynamicLayout.h"
#include "AICore/MetisFormulaContainer.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include "Three\sqlite3\sqlite3.h"  
#include "DatabaseHelper.h"
#include "TestData.h"
#include "CaseMatch/MetisFeatureMaker.h"
#include "CaseMatch/MetisRegionMatcher.h"

#include "Three/json/json.h"
#include "Log/easylogging++.h"
#include "Log/dump.h"
#include <io.h>
using namespace std;

INITIALIZE_EASYLOGGINGPP;
//ELPP_THREAD_SAFE;
//ELPP_USE_STD_THREADING;

//vector<std::string> GetFormulaList()
//{
//	std::vector<std::string> S;
//	std::string formular = "id=1843#rule=+CWallWithNothing*100*5+IWallWithWin*20*20+DDoor*10*10$";
//	S.push_back(std::move(formular));
//	std::string formular2 = "id=1844#rule=+WallParallelWithWin*50*50+CCorner*100*50+DDoor*10*10+DWin*10*10$";
//	S.push_back(std::move(formular2));
//	std::string formular3 = "id=1841#rule=+CWall*50*5+CCorner*100*50+DWin*10*10$";
//	S.push_back(std::move(formular3));
//	return S;
//}

void PointPrint(Point3f point)
{
	cout.setf(ios::fixed);
	cout << "x = "<< setprecision(3)<< point.x <<"  y = "<< point.y <<"  z = "<< point.z << endl;
}


// 初始化日志
void LogInit()
{
	el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
	el::Configurations conf(ComUtil::log_path);
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::MaxLogFileSize,
		"100000000");
	/// 也可以设置全部logger的配置  
	el::Loggers::reconfigureAllLoggers(conf);
}

// 获得单房间数据
SingleCase  getSingleCase(string hosue_name,string design_name)
{
	AICase* src_case = new AICase();
	SingleCase single_case = SingleCase();

	Json::Reader reader;
	Json::Value root;
	fstream f;

	// 初始化模板户型
	f.open(hosue_name, ios::in);
	if (!f.is_open())
	{
		cout << "Open hosue_name file error!=" << hosue_name << endl;
		return single_case;
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
			cout << "Open hosue_name file error!=" << hosue_name << endl;
			return single_case;
		}
	}

	src_case->json_file_name = hosue_name;
	src_case->Init();
	// 初始化单房间数据
	src_case->InitSingleCase();
	// 处理阳光门
	src_case->SetSunDoor();
    // 处理进光口
	src_case->SetSunnyOpening();

	SingleCase first_single_case = src_case->single_case_list[0];

	if (first_single_case.region_list.size() == 0)
	{
		first_single_case.entrance_door = *src_case->house.GetEntranceDoor();
		return first_single_case;
	}
	// 获得客厅的region
	RoomRegion tmpRegion = first_single_case.GetRegionById(CANTING);
	if (tmpRegion.getRegionId() == 0)
	{
		return single_case;
	}
	single_case = first_single_case.GetNewSingleCaseByRegion(tmpRegion);
	return single_case;
}

bool CreateSample()
{
	string house_path = "D:/test/sample/test/3810_ad2744d7e00bd6fff29ba6986afd40b9.json";
	string design_path = "D:/test/sample/1736_3faee801d50148b768239ef6a96f9847.json";
	SingleCase single_case = getSingleCase(house_path, design_path);
	if (single_case.corner_list.size() == 0)
	{
		return false;
	}

	return MetisFeatureMaker::ExtractRegionFeatureToFileAndRecordToServer(&single_case);
}

SampleRoom SearchSampleBySingle(SingleCase* p_single_case)
{
	MetisRegionMatcher * p_matcher = new MetisRegionMatcher();
	bool test = p_matcher->FindHighSimilarityOnServerTemplate(p_single_case, 100);
	if (test)
	{
		return p_matcher->GetHighSimilarityTemplate();
	}
	else
	{
		SampleRoom tmp = SampleRoom();
		return tmp;
	}
}

//int main()
//{
//   
//	// 初始化日志
//	LogInit();
//	shared_ptr<DatabaseHelper> p_db = std::make_shared<DatabaseHelper>();
//	if (!p_db->initDB())
//	{
//		LOG(ERROR)<<"database error!";
//		return 0;
//	}
//
//	TestData *p_data = TestData::initance();
//	if (!p_data->initMapCase())
//	{
//		cout << "户型数据异常！" << endl;
//		return 0;
//	}
//
//	int home_index = 1;
//	vector<int> model_list;
//
//	//阳台测试 10-26
//	model_list.push_back(10001);//休闲椅-右
//	//model_list.push_back(10058);//休闲椅-左
//	//model_list.push_back(10047);//1.0m阳台柜
//	//model_list.push_back(10021);//边几
//
//	AICase *src_case = p_data->getAICaseById(home_index, model_list);
//	SingleCase* p_single_case = &(src_case->single_case_list[0]);
//
//	//SampleRoom test_room = SearchSampleBySingle(p_single_case);
//	std::shared_ptr<SingleCase> ptr_single_case(p_single_case);
//	shared_ptr<MetisRegionResolver> region_resolver = make_shared<MetisRegionResolver>(ptr_single_case);
//
//	/** 厨房布局 */
//	/*KitchenAutomaticalDesign Design;
//	Design.AutoDesign(p_single_case);*/
//
//	/** 建立公式解析器 */
//	MetisFormulaContainer formula_container;
//	/** 独立性家具布局 */
//	MetisRegionResolver* p_region_resolver = region_resolver.get();
//	bool a = formula_container.ConstructWeightMapWithIndependent(p_region_resolver, p_single_case);
//	MetisDynamicLayout::CalculateIndependentExcellentRegion(region_resolver);
//
//	/** 依赖性家具布局 */
//	bool b = formula_container.ConstructWeightMapWithAllType(p_region_resolver, p_single_case);
//	MetisDynamicLayout::CalculateAttachExcellentRegion(region_resolver);
//	MetisDynamicLayout::ReviseFurnitureInformaiton(region_resolver, ptr_single_case);
//
//	// 同步设计数据
//	//src_case->SyncAICaseBySingle();
//	// 同步模型数据
//	src_case->SyncSingleToDesign();
//
//	// 生成json数据
//	string file_base = p_data->getOutPath(home_index);
//	for (int i = 0; i < p_single_case->layout_list.size(); i++)
//	{
//		string des_design = file_base + "_" + to_string(i) + ".json";
//		Json::Value root_value;
//		root_value = src_case->design.DumpBimData(i);
//		Json::StyledWriter swriter;
//		//输出到文件
//		ofstream os;
//		os.open(des_design);
//		os << swriter.write(root_value);
//		os.close();
//	}
//}

 //#include"UnitTest.h"
#include "HttpServer/MetisHttpServer.h"
#include <memory>
 int main()
 {
	/* string hosue_name = "D://test/all/3958_28455873c8d9619fecc529d9c1dfbed6.json";
	 string design_name = "D://test/all/2222.json";
	 SingleCase single_case = getSingleCase(hosue_name, design_name);*/

 	//UnitMetisFormulaContainer::Test();
 	//UnitMetisWeight::Test();
 	 
 	//UnitSingleCase::Test();
	// UnitHybirdWight::Test();
	 //LogInit();
	//#define  BASE_PATH  "D://test//sample_data/"
	//#define  DB_PATH  "D://test//aidesign.db"
	//#define  SAMPLE_DATA_PATH  "D://test//sample_data/"
	//#define  LOG_INIT_PATH  "D://test//my_log.ini"
	//#define  DUMP_PATH  "D://test//dump"

	 // 初始化数据
	 ComUtil::db_path = "D://test/aidesign.db";
	 ComUtil::sample_data_path = "D://test/sample_data/";
	 ComUtil::log_path = "D://test//AutoDesignLog.ini";
	 ComUtil::dump_path = "D://test/dump";

	 LogInit();

	 if (_access(ComUtil::db_path.c_str(), 0) == -1)
	 {
		 LOG(INFO) << "db_path is not exist";
	 }
	 else
	 {
		 LOG(INFO) << "db_path is exist";
	 }

	 if (_access(ComUtil::sample_data_path.c_str(), 0) == -1)
	 {
		 LOG(INFO) << "db_path is not exist";
	 }
	 else
	 {
		 LOG(INFO) << "db_path is exist";
	 }
	


	 shared_ptr<DatabaseHelper> p_db = std::make_shared<DatabaseHelper>();
	if (!p_db->initDB())
	{
		LOG(ERROR)<<"database error!";
		return 0;
	}

	LOG(INFO) << "start service......";
	//TryDump(
        MetisHttpServer *http_server = new MetisHttpServer();
		http_server->InitializeMetisHttpServer(5);
   //);
  }

