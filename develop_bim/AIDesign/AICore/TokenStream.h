/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:
*  简要描述:
*
*  当前版本: V1.0.0
*  作者:冯小翼
*  创建日期:2018-9-5
*  说明:
*      语法解析Token 流解析
**************************************************/
#pragma once
#include "Token.h"
#include <vector>
#include<string>
#include <unordered_map>
#define INIT_MAX_TOKEN_STREAM  4096
class  MetisFormulaContainer;
class TokenStream
{
public:
	TokenStream();
	~TokenStream();
	friend class MetisFormulaContainer;
public:
	/**删除拷贝构造*/
	TokenStream(const TokenStream&Stream) = delete;
	/**删除赋值*/
	void operator =(const TokenStream&Stream) = delete;
public:
	/*****************************************************************
	*  函数名: ProductTokenStream
	*  功能描述:将字符串转换为Token
	*
	*  @param formular(输入公式字符流)
	*
	*  @return  返回false 错误
*****************************************************************/

	bool ProductTokenStream(const std::string& formular);
private:
	/*****************************************************************
*  函数名: ParseTokenType
*  功能描述:将字符串转换为Token类型
*
*  @param 输入Token原始字符串
*
*  @return  返回false 错误
*****************************************************************/
	bool ParseTokenType(const std::string& identity);
/*****************************************************************
*  函数名: ParseTokenType
*  功能描述:将字符串转换为Token数字
*
*  @param 输入Token原始字符串
*
*  @return  返回false 错误
*****************************************************************/
	bool ParseTokenNumber(const std::string&identity);
/*****************************************************************
*  函数名: ParseTokenType
*  功能描述:将字符串转换为Token操作符
*
*  @param 输入Token原始字符串
*
*  @return  返回false 错误
*****************************************************************/
	bool ParseTokenOperator(const std::string&identity);
private:
	/**添加所有保留关键字*/
	void AddAllReseverKeyword();
	/**Token流*/
	std::vector<Token>	m_stream;
	/**关键字和token类型对应关系,用来加速构建token*/
	std::unordered_map<std::string, ETokenType> m_token_resever;
};

