
/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:ComUtil.h
*  ��Ҫ����:��ͨ������
*
*  ��ǰ�汾:
*  ����:��Զ
*  ��������:2018-08-29
*  ˵��:
**************************************************/

#pragma once
#include "Point3f.h"
#include "CommonSettings.h"
#include <sstream> 
#include <vector>
#include "objbase.h"
#include <json.h>
#include <math.h>
#include"ComMath.h"

using namespace std;

class ComUtil
{
public:
	ComUtil();
	virtual ~ComUtil();

public:

	// ����ת�ַ���
	static string intToString(int str);
	// char * �Ƿ�Ϊ��
	static bool IsEmptyOfChar (char* str);

	// string����תPoint3f
	static Point3f StringToPoint3f(string str);

	// char*����ת��
	static string charToString(const char* src);

	// �ַ���ȥ���ո�
	static string trim(string src);

	// �ַ���ת������
	static float stringToFloat(string& str);

	// ���32λΨһ16�����ַ���
	static string getGuuidNo();

	// json�ַ�����������
	static string JsonStringToUTF_8(string src);
	// wcharתstring
	static string WcharToString(WCHAR *wchar);
	
	// ���ݱ�Ż�������ļ�·��
	static string GetSampleDataPath(string no);

	// ���ݱ�Ż�������ļ�·��
	static int GetJsonIntByKey(Json::Value data,string key);

	// ���ݱ�Ż�������ļ�·��
	static float GetJsonFloatByKey(Json::Value data, string key);

	// ���ݱ�Ż�������ļ�·��
	static string GetStringFloatByKey(Json::Value data, string key);

	// �ַ���װ����
	static vector<int> StringToVector(string str,string key);
	// �������Ʊ任
	static int ChangeRoomId(int room_id);
	//UTF8����תGBK
	static std::string Utf8Togbk(std::string utfString);
	//GBK����תUTF8
	static std::string gbkToUtf8(std::string gbkstirng);
	// bimģ��idת��Ϊ����id
	static int BimToModel(int id);
	// ����idת��Ϊbimģ��id
	static int ModelToBim(int id);

	// ���ݿ��ļ�·��
	static std::string db_path;
	// ���ݿ���������·��
	static std::string sample_data_path;
	// ��־����·��
	static std::string log_path;
	// dump·��
	static std::string dump_path;
};

