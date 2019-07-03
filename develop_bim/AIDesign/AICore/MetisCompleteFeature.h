/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:������ȫ
*  ��Ҫ����:
*
*  ��ǰ�汾:V1.0.0
*  ����:jirenze
*  ��������:2018-11-06
*  ˵��:���������Ƿ���ȫ��������ɹ����ļ���
**************************************************/
#pragma once
#include "../DatabaseHelper.h"
#include "../AICase.h"

struct LocalFunction
{
	string room_no;
	int inner_type;
	int room_type;
	int score;
	int id;
};

class MetisCompleteFeature
{
public:
	MetisCompleteFeature():global_room_type(99),full_score(40) {};
	
	MetisCompleteFeature(const shared_ptr<AICase> &aicase);

	~MetisCompleteFeature() {};

	/** ��ȡ���ݿ������ */
	void PickBaseData();

	/** ��ȡ������ȫ������ */
	void PickGlobalData();

	/** ��ȡ�����пռ����� */
	void PickLocalData();
	 
	/** �ܷ� */
	int CalculateTotalScore();

	/** �������� */
	Json::Value TransferData();

private:
	/** ���ݿ�ȫ�� */
	vector<RoomFunction> database_globals;

	/** �����е�ȫ�ֹ������ */
	vector<int> program_global_modeltypes;

	/** ȫ�ֿ۷��� */
	vector<RoomFunction> global_marks;

	/** �ռ�۷��� */
	vector<LocalFunction> local_marks;

	/** ai_case */
	const shared_ptr<AICase> ai_case;

	/** ȫ�����ݿ�roomtype */
	const int global_room_type;

	/**����Ϊ40��*/
	const int full_score;
};
