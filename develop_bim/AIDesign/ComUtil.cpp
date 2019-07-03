#include "ComUtil.h"
#include <iostream>

// 数据库文件路径
std::string ComUtil::db_path = "";
// 数据库样板数据路径
std::string ComUtil::sample_data_path = "";
// 日志配置路径
std::string ComUtil::log_path = "";
// dump路径
std::string ComUtil::dump_path = "";

ComUtil::ComUtil()
{
}


ComUtil::~ComUtil()
{
}

// 整型转字符串
string ComUtil::intToString(int str)
{
	char des[8];
	_itoa(str, des, 16);
	return des;
}
// char * 是否为空
bool ComUtil::IsEmptyOfChar(char* str)
{
	if (str == NULL)
	{
		return true;
	}

	if (str == "")
	{
		return true;
	}

	if (strlen(str) == 0)
	{
		return true;
	}

	return false;
}

// char*类型转空
string ComUtil::charToString(const char* src)
{
	if (src == NULL)
	{
		return "";
	}

	if (src == "")
	{
		return "";
	}

	if (strlen(src) == 0)
	{
		return "";
	}

	return src;
}
Point3f ComUtil::StringToPoint3f(string str)
{
	Point3f temp = Point3f(0.0,0.0,0.0);
	int x_index = int(str.find("X="));
	int y_index = int(str.find("Y="));
	int z_index = int(str.find("Z="));

	if (x_index < 0 || y_index < 0 || z_index < 0)
	{
		return Point3f(0.0, 0.0, 0.0);
	}

	string str_x = str.substr(x_index + 2, y_index - x_index - 2);
	string str_y = str.substr(y_index + 2, z_index - y_index - 2);
	string str_z = str.substr(z_index + 2);

	str_x = trim(str_x);
	str_y = trim(str_y);
	str_z = trim(str_z);

	temp.x = stringToFloat(str_x);
	temp.y = stringToFloat(str_y);
	temp.z = stringToFloat(str_z);

	return temp;
}

string ComUtil::trim(string src)
{
	if (src.empty())
	{
		return src;
	}

	src.erase(0, src.find_first_not_of(" "));
	src.erase(src.find_last_not_of(" ") + 1);
	return src;

}

float ComUtil::stringToFloat(string& str)
{
// 	istringstream iss(str);
// 	float num;
// 	iss >> num;
// 	return num;
	/*str = str + "\0";*/
	return float(std::atof(str.c_str()));
}

// 获得32位唯一16进制字符串
string ComUtil::getGuuidNo()
{
	char buf[64] = { 0 };
	GUID guid;
	CoCreateGuid(&guid);
	sprintf_s(buf, sizeof(buf), "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1],
		guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);

	return std::string(buf);

}


// json字符串中文乱码
string ComUtil::JsonStringToUTF_8(string src)
{
	//解决中文转码问题
	int len = static_cast<int>(strlen(src.c_str()) + 1);
	char outch[MAX_PATH];
	WCHAR * wChar = new WCHAR[len];
	wChar[0] = 0;
	MultiByteToWideChar(CP_UTF8, 0, src.c_str(), len, wChar, len);
	WideCharToMultiByte(CP_ACP, 0, wChar, len, outch, len, 0, 0);
	delete[] wChar;
	char* pchar = (char*)outch;

	len = static_cast<int>(strlen(pchar) + 1);
	WCHAR outName[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, pchar, len, outName, len);
	
	return WcharToString(outName);

}

// wchar转string
string ComUtil::WcharToString(WCHAR *wchar)
{
	string result = "";
	WCHAR * w_text = wchar;
	DWORD dw_num = WideCharToMultiByte(CP_OEMCP, NULL, w_text, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte的运用
	char *ps_text; // psText为char*的临时数组，作为赋值给std::string的中间变量
	ps_text = new char[dw_num];
	WideCharToMultiByte(CP_OEMCP, NULL, w_text, -1, ps_text, dw_num, NULL, FALSE);// WideCharToMultiByte的再次运用
	result = ps_text;// std::string赋值
	delete[]ps_text;// psText的清除
	
	return result;
}

// 根据编号获得样板文件路径
string ComUtil::GetSampleDataPath(string no)
{
	string name = no + ".txt"; 
	return ComUtil::sample_data_path + name;
}

// 根据编号获得样板文件路径
 int  ComUtil::GetJsonIntByKey(Json::Value data, string key)
{
	 if (data.isMember(key) && data[key].isInt())
	 {
		 return data[key].asInt();
	 }

	 if (data.isMember(key) && data[key].isDouble())
	 {
		 double tmp = data[key].asDouble();
		 return ComMath::RoundD(tmp);
	 }

	/* if (data.isMember(key) && data[key].isString())
	 {
		 double tmp = data[key].asDouble();
		 return ComMath::RoundD(tmp);
	 }*/


	 return 0;
}

// 根据编号获得样板文件路径
 float  ComUtil::GetJsonFloatByKey(Json::Value data, string key)
{
	 if (data.isMember(key) && data[key].isDouble())
	 {
		 float tmp = static_cast<float>(data[key].asDouble());
		 return tmp;
	 }
	 else
	 {
		 return 0;
	 }
}

// 根据编号获得样板文件路径
string  ComUtil::GetStringFloatByKey(Json::Value data, string key)
{
	if (data.isMember(key) && data[key].isDouble())
	{
		return  data[key].asString();
	}
	else
	{
		return "";
	}
}

// 字符串装数组
vector<int> ComUtil::StringToVector(string str, string key)
{
	vector<int> result;
	if ("" == str)
	{
		return result;
	}

	//方便截取最后一段数据
	string strs = str + key;

	size_t pos = strs.find(key);
	size_t size = strs.size();

	while (pos != std::string::npos)
	{
		std::string tmp_str = strs.substr(0, pos);
		result.push_back(atoi(tmp_str.c_str()));
		strs = strs.substr(pos + 1, size);
		pos = strs.find(key);
	}

	return result;
}
// 房间编号变换
int ComUtil::ChangeRoomId(int room_id)
{
	switch (room_id)
	{
	case NOT_NAME:
		return NOT_NAME;
	case ZHUWO:
		return ZHUWO;
	case CIWO:
		return ZHUWO;
	case ERTONGFANG:
		return ZHUWO;
	case SHUFANG:
		return SHUFANG;
	case KETING:
		return KETING;
	case CHUWUJIAN:
		return CHUWUJIAN;
	case WEISHENGJIAN:
		return WEISHENGJIAN;
	case YANGTAI:
		return YANGTAI;
	case CHUFANG:
		return CHUFANG;
	case CANTING:
		return CANTING;
	case XUANGUAN:
		return XUANGUAN;
	case XIYIJIAN:
		return XIYIJIAN;
	case ZOULANG:
		return ZOULANG;
	case GUODAO:
		return GUODAO;
	case KEWO:
		return ZHUWO;
	case ZHUWEISHENGJIAN:
		return WEISHENGJIAN;
	case KEWEISHENGJIAN:
		return WEISHENGJIAN;
	case WOSHIYANGTAI:
		return YANGTAI;
	case HOUYANGTAI:
		return YANGTAI;
	case KECANTING:
		return KETING;
	default:
		return NOT_NAME;
	}
}
//UTF8编码转GBK
std::string ComUtil::Utf8Togbk(std::string utfString)
{
	if (utfString == "")
	{
		return "";
	}

	string strOutUTF8 = "";
	WCHAR * str1;
	int n = MultiByteToWideChar(CP_UTF8, 0, utfString.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_UTF8, 0, utfString.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_ACP, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n];
	WideCharToMultiByte(CP_ACP, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;
}
//GBK编码转UTF8
std::string ComUtil::gbkToUtf8(std::string gbkstirng)
{
	string strOutUTF8 = "";
	WCHAR * str1;
	int n = MultiByteToWideChar(CP_ACP, 0, gbkstirng.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, gbkstirng.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;
}

// bim模型id转化为构件id
int ComUtil::BimToModel(int id)
{
	int new_id = 0;
	switch (id)
	{
		// 双人床
	    case 278634:
			new_id = 10001;
			break;
		// 电视柜
		case 288799:
			new_id = 10043;
			break;
		// 左床头柜
		case 276063:
			new_id = 10006;
			break;
		// 右床头柜
		case 276065:
			new_id = 10005;
			break;
		// 衣柜
		case 289452:
			new_id = 10012;
			break;
	default:
		break;
	}
	return new_id;
}
// 构件id转化为bim模型id
int ComUtil::ModelToBim(int id)
{
	int new_id = 0;
	switch (id)
	{
		// 双人床
		case 10001:
			new_id = 278634;
			break;
		// 电视柜
		case 10043:
			new_id = 288799;
			break;
		// 左床头柜
		case 10006:
			new_id = 276063;
			break;
		// 右床头柜
		case 10005:
			new_id = 276065;
			break;
		// 衣柜
		case 10012:
			new_id = 289452;
			break;
	default:
		break;
	}

	return new_id;
}

