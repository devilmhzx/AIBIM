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
#include"MetisRegionFeature.h"
#include "../DatabaseHelper.h"
#include <vector>
#include "../AICase.h"
#include <fstream>

class MetisRegionMatcher
{
public:
	MetisRegionMatcher();
	~MetisRegionMatcher();
public:
	bool FindHighSimilarityOnServerTemplate(SingleCase*Case, int flag);
	SampleRoom GetHighSimilarityTemplate();
	//for test
	bool FindHighSimilarityTemplate(SingleCase*Case, int flag);
	bool FillTemplateFeaure(const std::vector<std::string>& Path);
	unsigned GetSimilarityTemplateIndex() {
		return m_similarity;
	}
	//test
	void SetGranularity(unsigned granularity);
	void SetWeight(float region, float win, float door);
private:
	bool OnMatch(SingleCase*Case,int flag);
private:
	unsigned											m_similarity;
	unsigned											m_granularity;
	float													m_region_weight;
	float													m_win_weight;
	float													m_door_weight;
	std::vector<MetisRegionFeature*>					m_template_feature;
	std::vector<SampleRoom>								m_room_template;
	SampleRoom													m_result;
};

