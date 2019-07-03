
/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:ComUtil.h
*  简要描述:共通方法库
*
*  当前版本:
*  作者:常远
*  创建日期:2018-08-29
*  说明:
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

	// 整型转字符串
	static string intToString(int str);
	// char * 是否为空
	static bool IsEmptyOfChar (char* str);

	// string类型转Point3f
	static Point3f StringToPoint3f(string str);

	// char*类型转空
	static string charToString(const char* src);

	// 字符串去掉空格
	static string trim(string src);

	// 字符串转浮点型
	static float stringToFloat(string& str);

	// 获得32位唯一16进制字符串
	static string getGuuidNo();

	// json字符串中文乱码
	static string JsonStringToUTF_8(string src);
	// wchar转string
	static string WcharToString(WCHAR *wchar);
	
	// 根据编号获得样板文件路径
	static string GetSampleDataPath(string no);

	// 根据编号获得样板文件路径
	static int GetJsonIntByKey(Json::Value data,string key);

	// 根据编号获得样板文件路径
	static float GetJsonFloatByKey(Json::Value data, string key);

	// 根据编号获得样板文件路径
	static string GetStringFloatByKey(Json::Value data, string key);

	// 字符串装数组
	static vector<int> StringToVector(string str,string key);
	// 房间名称变换
	static int ChangeRoomId(int room_id);
	//UTF8编码转GBK
	static std::string Utf8Togbk(std::string utfString);
	//GBK编码转UTF8
	static std::string gbkToUtf8(std::string gbkstirng);
	// bim模型id转化为构件id
	static int BimToModel(int id);
	// 构件id转化为bim模型id
	static int ModelToBim(int id);

	// 数据库文件路径
	static std::string db_path;
	// 数据库样板数据路径
	static std::string sample_data_path;
	// 日志配置路径
	static std::string log_path;
	// dump路径
	static std::string dump_path;
};

