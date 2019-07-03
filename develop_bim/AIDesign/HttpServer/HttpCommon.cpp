#include "HttpCommon.h"


std::string HttpCommon::HouseLayout = "single_house";
std::string HttpCommon::Design = "original_design";
std::string HttpCommon::Pressure = "pressure";
std::string HttpCommon::DesignSize = "design_size";
std::string HttpCommon::DesignScore = "design_score";
std::string HttpCommon::AllHouse = "houselayout";
std::string HttpCommon::AllDesign = "design";

MetisAllocator<HttpReply>HttpCommon::reply_pool;


HttpCommon::HttpCommon()
{
}


HttpCommon::~HttpCommon()
{
}

std::string HttpCommon::ConstructScoreString(int designIndex)
{
	std::string Out;
	char order[32];
	_itoa_s(designIndex, order, 10);
	Out = HttpCommon::DesignScore + order + "=";
	return Out;
}
std::string HttpCommon::ConstructDesignString(int designIndex)
{
	std::string Out;
	char order[32];
	_itoa_s(designIndex, order, 10);
	Out = HttpCommon::Design + order + "=";
	return Out;
}

// 通过编号获得错误信息
std::string HttpCommon::GetMsgByCode(int code)
{
	switch (code)
	{
	case suc_code:
		return "";
	case err_code:
		return "";
	default:
		return "";
	}
}

// 解析参数
map<string, string>HttpCommon::AnalyseAutoDesignParams(string str)
{
	map<string, string>params;
	vector<string> param_list = splitWithStl(str, "&");
	for (int i = 0; i < param_list.size(); i++)
	{
		string tmp_string = param_list[i];
		int first_index = static_cast<int>(tmp_string.find(HttpCommon::HouseLayout));
		if (first_index == -1)
		{
			continue;
		}
		else
		{
			int first_length = static_cast<int>(first_index + HttpCommon::HouseLayout.length() + 1);
			std::string value = tmp_string.substr(first_length);
			params[HttpCommon::HouseLayout] = value;
		}
	}

	for (int i = 0; i < param_list.size(); i++)
	{
		string tmp_string = param_list[i];
		int sec_index = static_cast<int>(tmp_string.find(HttpCommon::Design));
		if (sec_index == -1)
		{
			continue;
		}
		else
		{
			int sec_length = static_cast<int>(sec_index + HttpCommon::Design.length() + 1);
			std::string value = tmp_string.substr(sec_length);
			params[HttpCommon::Design] = value;
		}
	}

	return params;
}

// 解析户型评估参数
map<string, string>HttpCommon::AnalyseDesignEvaluateParams(string str)
{
	map<string, string>params;
	vector<string> param_list = splitWithStl(str, "&");
	for (unsigned int i = 0; i < param_list.size(); i++)
	{
		string tmp_string = param_list[i];
		int first_index = static_cast<int>(tmp_string.find(HttpCommon::AllHouse));
		if (first_index == -1)
		{
			continue;
		}
		else
		{
			int first_length = static_cast<int>(first_index + HttpCommon::AllHouse.length() + 1);
			std::string value = tmp_string.substr(first_length);
			params[HttpCommon::AllHouse] = value;
		}
	}

	for (unsigned int i = 0; i < param_list.size(); i++)
	{
		string tmp_string = param_list[i];
		int sec_index = static_cast<int>(tmp_string.find(HttpCommon::AllDesign));
		if (sec_index == -1)
		{
			continue;
		}
		else
		{
			int sec_length = static_cast<int>(sec_index + HttpCommon::AllDesign.length() + 1);
			std::string value = tmp_string.substr(sec_length);
			params[HttpCommon::AllDesign] = value;
		}
	}

	return params;
}

// 字符串分割
std::vector<std::string> HttpCommon::splitWithStl(const std::string &str, const std::string &pattern)
{
	std::vector<std::string> resVec;

	if ("" == str)
	{
		return resVec;
	}
	//方便截取最后一段数据
	std::string strs = str + pattern;

	size_t pos = strs.find(pattern);
	size_t size = strs.size();

	while (pos != std::string::npos)
	{
		std::string x = strs.substr(0, pos);
		resVec.push_back(x);
		strs = strs.substr(pos + 1, size);
		pos = strs.find(pattern);
	}

	return resVec;
}


string HttpCommon::UrlDecode(const std::string& uri)
{
	const unsigned char *ptr = (const unsigned char *)uri.c_str();
	string ret;
	ret.reserve(uri.length());

	for (; *ptr; ++ptr)
	{
		if (*ptr == '%')
		{
			if (*(ptr + 1))
			{
				char a = *(ptr + 1);
				char b = *(ptr + 2);
				if (!((a >= 0x30 && a < 0x40) || (a >= 0x41 && a < 0x47))) continue;
				if (!((b >= 0x30 && b < 0x40) || (b >= 0x41 && b < 0x47))) continue;
				char buf[3];
				buf[0] = a;
				buf[1] = b;
				buf[2] = 0;
				ret += (char)strtoul(buf, NULL, 16);
				ptr += 2;
				continue;
			}
		}
		if (*ptr == '+')
		{
			ret += ' ';
			continue;
		}
		ret += *ptr;
	}

	return ret;
}
