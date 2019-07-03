#include "MetisRegionMatcher.h"
#include <memory>


#define XIANG_WEI 36
MetisRegionMatcher::MetisRegionMatcher()
{
}


MetisRegionMatcher::~MetisRegionMatcher()
{
	for (auto feature : m_template_feature) {
		delete feature;
	}
	m_template_feature.clear();
}

bool MetisRegionMatcher::FindHighSimilarityOnServerTemplate(SingleCase*Case, int flag) {
	if(!Case)
		return false;
	shared_ptr<DatabaseHelper> p_db = std::make_shared<DatabaseHelper>();
	if (!p_db)
		return false;
	m_room_template.clear();
	m_room_template = p_db->searchSampleRoomByType(Case->single_room.getSpaceId());
	if (!m_room_template.size())
		return false;
	if (!OnMatch(Case, flag))
		return false;
	return true;
}

SampleRoom MetisRegionMatcher::GetHighSimilarityTemplate() {
	return m_result;
}

std::shared_ptr<AICase> GetTempSingleCase(string path)
{

	auto src_case = std::make_shared<AICase>();

	Json::Reader reader;
	Json::Value root;
	fstream f;

	// 初始化模板户型
	f.open(path, ios::in);
	if (!f.is_open())
	{
		cout << "Open hosue_name file error!=" << path << endl;
		return NULL;
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
			cout << "Open hosue_name file error!=" << path << endl;
			return NULL;
		}
	}

	src_case->json_file_name = path;
	src_case->Init();
	// 初始化单房间数据
	src_case->InitSingleCase();
	
	return src_case;

}

bool MetisRegionMatcher::OnMatch(SingleCase*Case, int flag) {
	std::shared_ptr<MetisRegionFeature> src_feature = std::make_shared<MetisRegionFeature>();
  

	/** 相位修改为72 */
	if (!src_feature->AnalyseCaseFeature(Case, XIANG_WEI))
		return false;
	src_feature->SetStructCmpFlag(flag);

	bool b_result = false;
	const float float_max = 999999.9f;
	float loss = 999999.9f;
	std::shared_ptr<MetisRegionFeature> dest_feature= std::make_shared<MetisRegionFeature>();
   	for (auto &sample : m_room_template) {
		//此处用json 实时生成特征，以后改成直接从据特征文件读取
		if (!dest_feature->Unserialization(sample.data_path.c_str()))
			return false;

        //if(dest_feature->AnalyseCaseFeature(,))
		//auto Aicase=GetTempSingleCase(sample.data);
		//auto single_case =&Aicase->single_case_list[0];

		//dest_feature->AnalyseCaseFeature(single_case, XIANG_WEI);
		// 0 相位
		bool accepted = true;
		src_feature->SetStartPhase(PHASE_0);
		auto out = src_feature->CompareSimilarity(*dest_feature, 0.8f, 0.0f, 0.1f, &accepted);
		if (accepted&&out < loss) {
			loss = out;
			m_result = sample;
			b_result = true;
		}
		// 90 相位
		src_feature->SetStartPhase(PHASE_90);
		 out = src_feature->CompareSimilarity(*dest_feature, 0.8f, 0.0f, 0.1f, &accepted);
		if (accepted&&out < loss) {
			loss = out;
			m_result = sample;
			b_result = true;
		}
		// 180 相位
		src_feature->SetStartPhase(PHASE_180);
		 out = src_feature->CompareSimilarity(*dest_feature, 0.8f, 0.0f, 0.1f, &accepted);
		if (accepted&&out < loss) {
			loss = out;
			m_result = sample;
			b_result = true;
		}
		// 270 相位
		src_feature->SetStartPhase(PHASE_270);
		 out = src_feature->CompareSimilarity(*dest_feature, 0.8f, 0.0f, 0.1f, &accepted);
		if (accepted&&out < loss) {
			loss = out;
			m_result = sample;
			b_result = true;
		}
	}
	if (!b_result)
		return false;
	return true;
}

void MetisRegionMatcher::SetGranularity(unsigned granularity)
{
	m_granularity = granularity;
}

void MetisRegionMatcher::SetWeight(float region, float win, float door)
{
	m_region_weight = region;
	m_region_weight = door;
	m_region_weight = win;
}