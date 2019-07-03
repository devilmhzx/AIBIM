/*******************************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:�����ڸ��任�µ�����
*  ��Ҫ����:
*
*  ��������:2018/10/8
*  ��   ��:��С��
*  ˵   ��:
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
	/*���л��������ļ�*/
	bool Serialization(const char*path);
	/*���ļ�������*/
	bool Unserialization(const char*path);
	/*����������������*/
	bool  AnalyseCaseFeature(SingleCase*single_case,unsigned granularity);
	/*��ȡ���ƶ�*/
	float CompareSimilarity(const MetisRegionFeature&other,float Region,float Door,float Win,bool *accpeted);
	/*������ʼ��λ*/
	void SetStartPhase(METIS_PHASE_ANGLE_FLAG Phase);
	/*���ýṹ�����Ƚ�����*/
	void SetStructCmpFlag(int Flag);
private:
	//���ƶȱȽ�(region)
	float CompareSimilarityOnRegion(const MetisRegionFeature&other,float tolerance, bool *accpeted);
	//���ƶȱȽ�(door)
	float CompareSimilarityOnDoor(const MetisRegionFeature&other, float tolerance, bool *accpeted);
	//���ƶȱȽ�(windows)
	float CompareSimilarityOnWindows(const MetisRegionFeature&other, float tolerance, bool *accpeted);
	//�ṹ�Ƚ�
	bool CompareOnStruct(const MetisRegionFeature&other);
	//��ȡ��������
	Point3f CalculateRegionCentroid(SingleCase*single_case);
	//��ȡAABB���ĵ�
	Point3f CalculateAABBCentre(SingleCase*single_case);
	//��ȡ��λ����
	void ExtractFeatureOnPhase(SingleCase*single_case,float phase,const Point3f&centroid);
	//��ȡ�ṹ����
	void ExtractFeatureOnStruct(SingleCase*single_case);
private:
	//��λ��
	METIS_PHASE_ANGLE_FLAG	m_start_phase;
	//�ṹ�Ա�λ
	int						m_cmp_flag;
	//���θ�ƽ��ϸ������
	unsigned				m_complex_granularity;
	//��������
	std::vector<ComplexFeature>	 m_complex_feature;
	//�ṹ����(ǽ�ĸ���)
	unsigned				m_wall_size;
	//�ṹ����(�ŵĸ���)
	unsigned				m_door_size;
	//�ṹ����(�����ĸ���)
	unsigned				m_win_size;
};

