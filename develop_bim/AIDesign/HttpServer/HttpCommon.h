#pragma once

#ifndef COMMONS
#define COMMONS
#include "MetisAllocator.h"
#include <vector>
//#include <assert.h>
#define WIN32_LEAN_AND_MEAN
#include"../Design.h"


#define suc_code 10000
#define param_err_code 10008
#define not_exist_code 10009
#define err_code 10010
#define house_err_code 10011
#define design_err_code 10012
#define dependent_err_code 10013
#define undependent_err_code 10014
#define not_door_err_code 10015
#define not_name_err_code 10016

/*#define suc_msg "成功"
#define param_err_msg "参数错误"
#define not_exist_msg "不存在有效的设计方案"
#define err_msg "数据请求异常"
#define house_err_msg "原始户型数据错误"
#define design_err_msg "原始设计数据错误"
#define dependent_err_msg "依赖规则解析错误"
#define undependent_err_msg "非依赖规则解析错误"
#define not_door_err_msg "户型没有门"*/

struct HttpReply
{
	HttpReply() :req(nullptr), buffer(nullptr) {}
	//http 接口类型
	int 	apiType;
	//reply object
	struct evhttp_request *req;
	//url
	struct evbuffer*	 buffer;
	//client houselauout
	std::string		client_house_layout;
	//client
	std::string		client_design;
	//client
	int 	code;
	//design
	std::vector<std::string>	design;
	Design design_entity;

	Json::Value  evaluate_design;
};

class HttpCommon
{
public:
	HttpCommon();
	~HttpCommon();
public:
	//用于请求的类型
	static std::string HouseLayout;
	static std::string Design;
	static std::string AllHouse;
	static std::string AllDesign;
	static std::string Pressure;
	//用于回复类型
	static std::string DesignSize;
	static std::string DesignOrder;
	static std::string DesignScore;
	//用于产生每个户型的得分
	static std::string ConstructScoreString(int designIndex);

	static std::string ConstructDesignString(int designIndex);
	// 通过编号获得错误信息
	static std::string GetMsgByCode(int code);
	//内存池
	static MetisAllocator<HttpReply>	reply_pool;

	// 解析自动设计参数
	static map<string, string>AnalyseAutoDesignParams(string str);
	// 解析户型评估参数
	static map<string, string>AnalyseDesignEvaluateParams(string str);
	// 字符串分割
	static std::vector<std::string> splitWithStl(const std::string &str, const std::string &pattern);

	static string UrlDecode(const std::string& uri);
	// GB2312到UTF-8的转换
	//static char* U2G(const char* utf8);
};

#endif // !COMMONS
