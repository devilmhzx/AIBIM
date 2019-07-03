/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:DatabaseHelper.h
*  ��Ҫ����:���ݿ���ز���
*
*  ��ǰ�汾:
*  ����:��Զ
*  ��������:2018-08-29
*  ˵��:
**************************************************/

#pragma once
#include "CommonSettings.h"
#include "ComUtil.h"
#include "Three\sqlite3\sqlite3.h"  
#include "Model.h"
#include <mutex>


struct SampleRoom
{
	string no;
	int		room_type;
	string design_no;
	string      data;
	string data_path;
};

struct RoomFunction
{
	int id;
	int		room_type;
	vector<int> model_inner_list;
	int      score;
};

// ����ṹ��
struct RegualrtionsStruct
{
	string  regualrtion; // ��������
	string  no; // ������
};

class DatabaseHelper
{
public:
	DatabaseHelper();
	~DatabaseHelper();

	//static std::mutex   db_mutex;
	//static DatabaseHelper* initance();

	// ��ʼ�����ݿ�
	bool initDB();

	// ͨ��ģ�ͱ�ż�������
	vector<string> searchRegualrtionsByModel(const int modelId);

	// ͨ���������ͼ�����������ֵ
	vector<SampleRoom> searchSampleRoomByType(const int roomType);

	// �������е�ģ������
	vector<SampleRoom> searchAllSampleRoom();

	// �������������
	bool insertSampleRoom(SampleRoom sample);


	// ͨ��ģ�ͱ�ź�������ż������򣬷��صĽ��Ϊ�����б�
	vector<string> searchRegualrtionsByModelandSample(const int modelId,const string sampleNo);

	// ͨ��ģ�ͱ�ź�������ż������򣬷��صĽ��Ϊ����ṹ���б�
	vector<RegualrtionsStruct> searchRegualrtionsStructByModelandSample(const int modelId, const string sampleNo);

	// ͨ��ģ�ͱ�ż���ģ����ϸ��Ϣ
	Model searchModelInfoById(const int modelId);

	// ��������ģ����Ϣ
	vector<Model>  searchAllModelInfo();

	// ͨ���������ͼ�����������ģ��
	vector<RoomFunction> searchRoomFunctionByType(const int roomType);

	// ͨ�������ż�����������
	string searchRegualrtionsDescriptionByNo(const string no);

	// ͨ��ģ�ͺͷ������ͻ�ù�������
	string searchRegualrtionsDescriptionByModel(const int space_id,const Model *p_model);

private:
	sqlite3 *db;
	string db_name;

	// �������ݿ�
	bool create();
    // �����ݿ�
	bool open();
	// �ر����ݿ�
	void close();
	// ����ģ�ͱ�
	bool createModelTable();
	// ����������
	bool createSampleRoomTable();
	// ���������
	bool createRegularTable();
	// ����������ȫ��
	bool createRoomFunctionTable();
	// ���Ƿ����
	bool isExitsTable(string tableName);

	static DatabaseHelper * p;
};

