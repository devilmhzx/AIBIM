/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:组合生成
*  简要描述:对于可组合家具，进行家具组合生成
*
*  当前版本: V1.0.0
*  作者:jirenze/fengxiaoyi
*  创建日期:2018-12-05
*  说明:
**************************************************/
#pragma once
#include "Model.h"
#include <unordered_map>
#include <set>
#include <vector>


/** 包代表的模型 */
struct VirtualModel
{
	std::string virtual_no;
	Model		 model;
};

/** 包的原始数据 */
struct  ModelPackage
{
	VirtualModel									virtual_model;
	std::unordered_map<int, Point3f>	local_location;
};

/** 包相关数据结构 */
struct  PackageStruct
{
	VirtualModel			  virtual_model;
	std::vector<Model> mode_list;
};

class SingleCase;
class CombinedGeneration
{

public:
	CombinedGeneration() = default;
	~CombinedGeneration() = default;
	/**
	*	函数名:  初始化模型包
	*	参数:		无
	*	返回:		无
	*/
	static void		InitializeModelPackage();
	/**
	* 函数 :  通过SingleCase获取可能生成的包
	* 返回:   包结构数组
	*/
	static std::vector<PackageStruct>  GetCacheModelListBySingleCase(SingleCase*single_case);
	/**
	*	函数名:  获取模型组的虚拟包
	*	参数: 输入模型的数组 
	*	返回: 虚拟模型包,ID使用沙发的标识符 
	*/
	static bool		GetCacheByModelGroup(const std::vector<int> &group, VirtualModel&p_model);
	/**
	*	函数名:  通过虚拟包获取模型组
	*  参数:		 虚拟模型输入
	*  返回:		 模型组(包含虚拟包)
	*/
	static bool		GetModelGroupByVirtualModel(const VirtualModel&p_model, std::vector<Model> &group);
	/** 解包返回 */
private:
	/* 生成唯一标识 */
	static std::string  GetUniqueID(std::vector<int>&model);
	/* 处理方向 */

	/****** 客厅包 *******/
	/**
	* @辅位沙发-左
	* @三人沙发
	* @茶几
	* @辅位沙发-右
	*/
	static void		    ProcessRotateByPackage_1(const VirtualModel&p_model, std::vector<Model> &group);

	static void		    ProcessRotateByPackage_2(const VirtualModel&p_model, std::vector<Model> &group);
	/**
	* @三人沙发
	* @茶几
	* @辅助-右
	*/
	static void		    ProcessRotateByPackage_3(const VirtualModel&p_model, std::vector<Model> &group);
	/**
	* @三人沙发
	* @茶几
	*/
	static void		    ProcessRotateByPackage_4(const VirtualModel&p_model, std::vector<Model> &group);
	
	static std::string  packag_no[4];
	/** 包数据 */
	static std::unordered_map<std::string, ModelPackage>  cache_package;
	/** 会客包 */
	static std::set<int> visitor_package;
};