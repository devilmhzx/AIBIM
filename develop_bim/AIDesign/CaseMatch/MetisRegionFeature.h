/*******************************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:户型在复变换下的特征
*  简要描述:
*
*  创建日期:2018/10/8
*  作   者:冯小翼
*  说   明:
*
******************************************************************/
#pragma once
#include "../Point3f.h"
#include "../SingleCase.h"
#include <vector>
struct ComplexFeature
{
	std::vector<float>	region_properties;
	float					door_properties;
	float					win_properties;
};
enum  METIS_PHASE_ANGLE_FLAG
{
	PHASE_0,
	PHASE_90,
	PHASE_180,
	PHASE_270
};
enum  METIS_STRUCTRAL_CMP_FLAG
{
	STRUCT_WALL = 0x1,
	STRUCT_WIN =0x2,
	STRUCT_DOOR=0x4
};
class MetisRegionFeature
{
public:
	MetisRegionFeature();
	~MetisRegionFeature();
public:
	/*序列化特征到文件*/
	bool Serialization(const char*path);
	/*从文件反序列*/
	bool Unserialization(const char*path);
	/*分析输入区域特征*/
	bool  AnalyseCaseFeature(SingleCase*single_case,unsigned granularity);
	/*获取相似度*/
	float CompareSimilarity(const MetisRegionFeature&other,float Region,float Door,float Win,bool *accpeted);
	/*设置起始相位*/
	void SetStartPhase(METIS_PHASE_ANGLE_FLAG Phase);
	/*设置结构特征比较类型*/
	void SetStructCmpFlag(int Flag);
private:
	//相似度比较(region)
	float CompareSimilarityOnRegion(const MetisRegionFeature&other,float tolerance, bool *accpeted);
	//相似度比较(door)
	float CompareSimilarityOnDoor(const MetisRegionFeature&other, float tolerance, bool *accpeted);
	//相似度比较(windows)
	float CompareSimilarityOnWindows(const MetisRegionFeature&other, float tolerance, bool *accpeted);
	//结构比较
	bool CompareOnStruct(const MetisRegionFeature&other);
	//提取质心坐标
	Point3f CalculateRegionCentroid(SingleCase*single_case);
	//提取AABB中心点
	Point3f CalculateAABBCentre(SingleCase*single_case);
	//提取相位特征
	void ExtractFeatureOnPhase(SingleCase*single_case,float phase,const Point3f&centroid);
	//提取结构特征
	void ExtractFeatureOnStruct(SingleCase*single_case);
private:
	//相位角
	METIS_PHASE_ANGLE_FLAG	m_start_phase;
	//结构对比位
	int						m_cmp_flag;
	//几何复平面细分粒度
	unsigned				m_complex_granularity;
	//几何特征
	std::vector<ComplexFeature>	 m_complex_feature;
	//结构特征(墙的个数)
	unsigned				m_wall_size;
	//结构特征(门的个数)
	unsigned				m_door_size;
	//结构特征(窗户的个数)
	unsigned				m_win_size;
};

