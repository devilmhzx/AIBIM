#pragma once
/*************************************************
*  Copyright(c) 2018 - 2020 DaBanJia
*  All rights reserved.
*
* 文件名称:组合生成
* 简要描述 : 针对客厅主位沙发、辅位沙发、茶几进行组合
*
* 当前版本 : V1.0.0
* 作者 : jirenze 
* 创建日期 : 2019 - 02 - 24
* 说明 :
		返回新的Model.id = 90001
**************************************************/
#include "Model.h"
#include "CommonSettings.h"
#include <math.h>
#include <memory>
#include "ComStruct.h"
struct ModelPackageJ
{
	/** 包的序号 */
	string package_no;

	/** 虚拟模型 */
	Model virtual_model;

	/** 原模型数据 */
	vector<Model> package_model_list;

	/** 相对位置 */
	map<int, Point3f> relative_position;
};


class CombinedGenerationJ
{
public:
	CombinedGenerationJ();
	~CombinedGenerationJ();

	/** 初始化模型包 连接模型包方法 */
	bool InitModelPackage(vector<Model>& in_model);

	/** 解析包 */
	bool AnalyzePackage(vector<ModelLayout>& in_Layout);

	Model GetVirtualModel();
	
	/** result */
	bool result;

private:

	/** Package 1 主位沙发 茶几 */
	void SetPackage1(vector<Model>& in_model);

	/** Package 2 主位沙发 茶几 左辅位沙发 */
	void SetPackage2(vector<Model>& in_model);

	/** Package 3 主位沙发 茶几 */
	void SetPackage3(vector<Model>& in_model);

	/** Package 4 主位沙发 茶几 左辅位沙发 */
	void SetPackage4(vector<Model>& in_model);

	/** 进行生成打包处理 */
	void GeneratePackage(vector<Model>& in_model);

	/** Package 1 主位沙发 茶几 */
	void GetPackage(shared_ptr<ModelPackageJ>& in_model_package);


private:
	/** 包的编号和方法 */
	map<const string, int> package_function;

	/** 组合包中的模型Id */
	vector<int> package_model_id;

	/** 包的标示号  检索执行打哪个包 */
	string read_id_str;

	/** 客厅包的内部家具相对位置和角度 */
	/*Point3f major_sofa_position;
	Point3f left_sofa_position;
	Point3f right_sofa_position;
	Point3f tea_table_position;*/

	/** 主位沙发 茶几 */
	const string package1_id="10161019";
	/** 主位沙发 茶几 辅位沙发-左 */
	const string package2_id="101610171019";
	/** 主位沙发 茶几 辅位沙发-右 */
	const string package3_id="101610181019";
	/** 主位沙发 茶几 辅位沙发-左 辅位沙发-右 */
	const string package4_id="1016101710181019";

	/** pi */
	const double pi = acos(-1);

	/** 完成的包 */
	shared_ptr<ModelPackageJ> end_model_package;
};