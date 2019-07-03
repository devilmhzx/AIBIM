#include "MetisRegionFeature.h"
#include "../ComMath.h"
#include <algorithm>
#include <fstream>
const Point3f phase_benchmark = Point3f(1000.f, 0.f, 0.f);
const float	  centroid_benchmark = 10;

MetisRegionFeature::MetisRegionFeature()
{
}


MetisRegionFeature::~MetisRegionFeature()
{
}

bool MetisRegionFeature::Serialization(const char*path) 
{
	std::ofstream fileout;
	fileout.open(path, std::ios::out | std::ios::binary|std::ios::trunc);
	if (!fileout.is_open())
		return false;
	//输出结构特征
	fileout.write((char*)&m_wall_size, sizeof(unsigned));
	fileout.write((char*)&m_door_size, sizeof(unsigned));
	fileout.write((char*)&m_win_size, sizeof(unsigned));
	//输出粒度
	fileout.write((char*)&m_complex_granularity, sizeof(unsigned));
	//输出粒度下的vector size
	unsigned size = (unsigned)m_complex_feature.size();
	fileout.write((char*)&size, sizeof(unsigned));
	//输出每个户型复数特征

	for (auto &feature : m_complex_feature) {
		float tmp_feature;
		tmp_feature = feature.door_properties;
		fileout.write((char*)&tmp_feature, sizeof(float));
		tmp_feature = feature.win_properties;
		fileout.write((char*)&tmp_feature, sizeof(float));
		size = (unsigned)feature.region_properties.size();
		fileout.write((char*)&size, sizeof(unsigned));
		//输出每个重叠特征点
		for (auto &complex : feature.region_properties) {
			tmp_feature = complex;
			fileout.write((char*)&tmp_feature, sizeof(float));
		}
	}

	fileout.close();
	return true;
}

bool MetisRegionFeature::Unserialization(const char*path)
{
	std::ifstream filein;
	filein.open(path, std::ios::in | std::ios::binary);
	if (!filein.is_open())
		return false;
	m_complex_feature.clear();
	m_complex_granularity = 0;

	//输入结构特征
	filein.read((char*)&m_wall_size, sizeof(unsigned));
	filein.read((char*)&m_door_size, sizeof(unsigned));
	filein.read((char*)&m_win_size, sizeof(unsigned));

	//1.输入粒度
	filein.read((char*)&m_complex_granularity, sizeof(unsigned));
	if (!m_complex_granularity)
		return false;
	unsigned size = 0;
	filein.read((char*)&size, sizeof(unsigned));
	if (!size)
		return false;
	m_complex_feature.reserve(size);

	for (unsigned i = 0; i < size; ++i) {
		ComplexFeature feature;
		float tmp_feature;
		filein.read((char*)&tmp_feature, sizeof(float));
		feature.door_properties= tmp_feature;
		filein.read((char*)&tmp_feature, sizeof(float));
		feature.win_properties = tmp_feature;
		unsigned internel_size = 0;
		filein.read((char*)&internel_size, sizeof(unsigned));
		if (!internel_size)
			return false;
		feature.region_properties.reserve(internel_size);
		for (unsigned j = 0; j < internel_size; ++j) {
			filein.read((char*)&tmp_feature, sizeof(float));
			feature.region_properties.push_back(tmp_feature);
		}
		m_complex_feature.push_back(std::move(feature));
	}

	auto c = filein.good();
	filein.close();
	return true;
}

void MetisRegionFeature::SetStartPhase(METIS_PHASE_ANGLE_FLAG Phase) {
	m_start_phase = Phase;
}
 
void MetisRegionFeature::SetStructCmpFlag(int Flag) {
	m_cmp_flag = Flag;
}

float MetisRegionFeature::CompareSimilarity(const MetisRegionFeature&other, float Region, float Door, float Win, bool *accpeted) 
{
	*accpeted = true;
	const float float_max = 999999.9f;
	if (!CompareOnStruct(other))
	{
		*accpeted = false;
		return float_max;
	}

	auto value_region = CompareSimilarityOnRegion(other, Region, accpeted);
	if (!*accpeted)
		return float_max;

	auto value_door = CompareSimilarityOnDoor(other, Door, accpeted);
	if (!*accpeted)
		return float_max;

	auto value_window = CompareSimilarityOnWindows(other, Win, accpeted);
	if (!*accpeted)
		return float_max;
	auto out = value_region + value_door + value_window;
	return out;
}

float MetisRegionFeature::CompareSimilarityOnRegion(const MetisRegionFeature&other, float tolerance,bool *accpeted) 
{
	const float float_max = 999999.9f;
	if (other.m_complex_granularity != this->m_complex_granularity)
	{
		*accpeted = false;
		return float_max;
	}
	float out = 0.f;
	size_t phase_index = 0;
	switch (m_start_phase)
	{
	case PHASE_0:
		phase_index = 0;
		break;
	case PHASE_90:
		phase_index = m_complex_feature.size() / 4;
		break;
	case PHASE_180:
		phase_index = m_complex_feature.size() / 2;
		break;
	case PHASE_270:
		phase_index = m_complex_feature.size() / 4 * 3;
		break;
	default:
		break;
	}

	for (size_t i = 0; i < m_complex_feature.size(); ++i)
	{
		size_t index = i + phase_index;
		index = index % m_complex_feature.size();
		auto &this_feature = m_complex_feature[i];
		auto &other_feature = other.m_complex_feature[index];
		//判断容差范围
		//对于每一个交点获取对应交点最近距离
		for (auto this_range : this_feature.region_properties) 
		{
			float dist = float_max;
			for (auto other_range : other_feature.region_properties)
			{
				auto dist_temp = abs(this_range-other_range);
				if (dist_temp < dist) 
				{
					dist = dist_temp;
				}
			}
			//没有在容差范围内的户型不具有相似性
			out += dist* tolerance;
		}
	}
	*accpeted = true;
	return out;
}

float MetisRegionFeature::CompareSimilarityOnDoor(const MetisRegionFeature&other, float tolerance,bool *accpeted) 
{
	const float float_max = 999999.9f;
	if (other.m_complex_granularity != this->m_complex_granularity) 
	{
		*accpeted = false;
		return float_max;
	}
	float out = 0.f;
	size_t phase_index = 0;
	switch (m_start_phase)
	{
	case PHASE_0:
		phase_index = 0;
		break;
	case PHASE_90:
		phase_index = m_complex_feature.size() / 4;
		break;
	case PHASE_180:
		phase_index = m_complex_feature.size() / 2;
		break;
	case PHASE_270:
		phase_index = m_complex_feature.size() / 4 * 3;
		break;
	default:
		break;
	}

	for (size_t i = 0; i < m_complex_feature.size(); ++i) {
		size_t index = i + phase_index;
		index = index % m_complex_feature.size();
		auto &this_feature = m_complex_feature[i];
		auto &other_feature = other.m_complex_feature[index];
		//判断容差范围
		//对于每一个交点获取对应交点最近距离
		auto dist = abs(this_feature.door_properties-other_feature.door_properties);
		out += dist * tolerance;
	}
	*accpeted = true;
	return out;
}

float MetisRegionFeature::CompareSimilarityOnWindows(const MetisRegionFeature&other, float tolerance, bool *accpeted)
{
	const float float_max = 999999.9f;
	if (other.m_complex_granularity != this->m_complex_granularity) 
	{
		*accpeted = false;
		return float_max;
	}
	float out = 0.f;
	size_t phase_index = 0;
	switch (m_start_phase)
	{
	case PHASE_0:
		phase_index = 0;
		break;
	case PHASE_90:
		phase_index = m_complex_feature.size() / 4;
		break;
	case PHASE_180:
		phase_index = m_complex_feature.size() / 2;
		break;
	case PHASE_270:
		phase_index = m_complex_feature.size() / 4 * 3;
		break;
	default:
		break;
	}

	for (size_t i = 0; i < m_complex_feature.size(); ++i) {
		size_t index = i + phase_index;
		index = index % m_complex_feature.size();
		auto &this_feature = m_complex_feature[i];
		auto &other_feature = other.m_complex_feature[index];
		//判断容差范围
		//对于每一个交点获取对应交点最近距离
		auto dist = abs(this_feature.win_properties-other_feature.win_properties);
		out += dist * tolerance;
	}
	*accpeted = true;
	return out;
}

bool MetisRegionFeature::CompareOnStruct(const MetisRegionFeature&other) {
	if (m_cmp_flag&METIS_STRUCTRAL_CMP_FLAG::STRUCT_WALL)
	{
		if (m_wall_size != other.m_wall_size)
			return false;
	}
	if (m_cmp_flag&METIS_STRUCTRAL_CMP_FLAG::STRUCT_DOOR) {
		if (m_door_size != other.m_door_size)
			return false;
	}
	if (m_cmp_flag&METIS_STRUCTRAL_CMP_FLAG::STRUCT_WIN) {
		if (m_win_size != other.m_win_size)
			return false;
	}
	return true;
}

bool  MetisRegionFeature::AnalyseCaseFeature(SingleCase*single_case,  unsigned granularity)
{
	if (!single_case)
		return false;
	m_complex_granularity = granularity;
	m_complex_feature.clear();
	float angle_2pi = 3.1415926f*2;
	//清理在重新适配内存
	m_complex_feature.clear();
	m_complex_feature.reserve(granularity);
	//获取结构特征
	ExtractFeatureOnStruct(single_case);

	//获取质心
	auto centroid = CalculateAABBCentre(single_case);
	//细分到复空间
	for (unsigned i = 0; i < granularity; ++i) 
	{
		auto phase = angle_2pi / granularity * i;
		ExtractFeatureOnPhase(single_case,phase, centroid);
	}
	return true;
}

Point3f MetisRegionFeature::CalculateRegionCentroid(SingleCase*single_case)
{
	//获取质心
	size_t sum = 0;
	Point3f centroid = Point3f(0.f, 0.f, 0.f);
	for (auto wall : single_case->wall_list) {
		auto &beg = wall.start_corner.point;
		auto &end = wall.end_corner.point;
		auto normalize = ComMath::Normalize(end - beg);
		auto length = ComMath::getTwoPointDistance(beg, end);
		// 每次增加0.5m
		for (auto start = 0.f; start < length; start+=centroid_benchmark) {
			auto end_point = beg + normalize * start;
			centroid = centroid+ end_point;
			++sum;
		}
	 }
	auto reciprocal_sum = 1.f / sum;
	return  centroid * reciprocal_sum;
}

Point3f MetisRegionFeature::CalculateAABBCentre(SingleCase*single_case) {
	Point3f Max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	Point3f Min(FLT_MAX, FLT_MAX, FLT_MAX);
	for (auto &wall : single_case->wall_list) {
		Max = ComMath::Point3fMax(wall.start_corner.point, Max);
		Max = ComMath::Point3fMax(wall.end_corner.point, Max);
		Min = ComMath::Point3fMin(wall.start_corner.point, Min);
		Min = ComMath::Point3fMin(wall.end_corner.point, Min);
	}
	Max = Max + Min;
	Max = Max * 0.5f;
	return Max;
}

void MetisRegionFeature::ExtractFeatureOnPhase(SingleCase*single_case, float phase, const Point3f&centroid)
{
	auto phase_real = cosf(phase)*phase_benchmark.x - sinf(phase)*phase_benchmark.y;
	auto phase_image = sinf(phase)*phase_benchmark.x + cosf(phase)*phase_benchmark.y;
	auto phase_vector3f = Point3f(phase_real, phase_image, 0.f);
	phase_vector3f = phase_vector3f + centroid;
	//判断复平面交点
	ComplexFeature feature;
 
	//对于区域的复平面
	for (auto wall : single_case->wall_list) {
		Point3f Intersection = centroid;	
		if (ComMath::bIntersectOnTwoLines(centroid, phase_vector3f, wall.start_corner.point, wall.end_corner.point, &Intersection))
		{
			feature.region_properties.push_back(ComMath::getTwoPointDistance(Intersection ,centroid));
		}
	}
	if (!feature.region_properties.size())
		feature.region_properties.push_back(0.f);
	//对于门的区域
	feature.door_properties = float(0.f);
	for (auto wall : single_case->wall_list) {
		for (auto door : wall.door_list)
		{
			Point3f Intersection = centroid;
			if (ComMath::bIntersectOnTwoLines(centroid, phase_vector3f, door.start_point, door.end_point, &Intersection))
				feature.door_properties = ComMath::getTwoPointDistance(Intersection,centroid);
		}
	 }
	//对于窗的区域
	feature.win_properties = float(0.f);
	for (auto wall : single_case->wall_list) {
		for (auto win : wall.window_list)
		{
			Point3f Intersection = centroid;
			if (ComMath::bIntersectOnTwoLines(centroid, phase_vector3f, win.start_point, win.end_point, &Intersection))
				feature.win_properties = ComMath::getTwoPointDistance(Intersection, centroid);
		}
	}
	m_complex_feature.push_back(std::move(feature));
}

void MetisRegionFeature::ExtractFeatureOnStruct(SingleCase*single_case)
{
	m_wall_size = (unsigned)single_case->wall_list.size();
	m_door_size = (unsigned)single_case->door_list.size();
	m_win_size = (unsigned)single_case->window_list.size();
}


