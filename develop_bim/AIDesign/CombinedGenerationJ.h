#pragma once
/*************************************************
*  Copyright(c) 2018 - 2020 DaBanJia
*  All rights reserved.
*
* �ļ�����:�������
* ��Ҫ���� : ��Կ�����λɳ������λɳ�����輸�������
*
* ��ǰ�汾 : V1.0.0
* ���� : jirenze 
* �������� : 2019 - 02 - 24
* ˵�� :
		�����µ�Model.id = 90001
**************************************************/
#include "Model.h"
#include "CommonSettings.h"
#include <math.h>
#include <memory>
#include "ComStruct.h"
struct ModelPackageJ
{
	/** ������� */
	string package_no;

	/** ����ģ�� */
	Model virtual_model;

	/** ԭģ������ */
	vector<Model> package_model_list;

	/** ���λ�� */
	map<int, Point3f> relative_position;
};


class CombinedGenerationJ
{
public:
	CombinedGenerationJ();
	~CombinedGenerationJ();

	/** ��ʼ��ģ�Ͱ� ����ģ�Ͱ����� */
	bool InitModelPackage(vector<Model>& in_model);

	/** ������ */
	bool AnalyzePackage(vector<ModelLayout>& in_Layout);

	Model GetVirtualModel();
	
	/** result */
	bool result;

private:

	/** Package 1 ��λɳ�� �輸 */
	void SetPackage1(vector<Model>& in_model);

	/** Package 2 ��λɳ�� �輸 ��λɳ�� */
	void SetPackage2(vector<Model>& in_model);

	/** Package 3 ��λɳ�� �輸 */
	void SetPackage3(vector<Model>& in_model);

	/** Package 4 ��λɳ�� �輸 ��λɳ�� */
	void SetPackage4(vector<Model>& in_model);

	/** �������ɴ������ */
	void GeneratePackage(vector<Model>& in_model);

	/** Package 1 ��λɳ�� �輸 */
	void GetPackage(shared_ptr<ModelPackageJ>& in_model_package);


private:
	/** ���ı�źͷ��� */
	map<const string, int> package_function;

	/** ��ϰ��е�ģ��Id */
	vector<int> package_model_id;

	/** ���ı�ʾ��  ����ִ�д��ĸ��� */
	string read_id_str;

	/** ���������ڲ��Ҿ����λ�úͽǶ� */
	/*Point3f major_sofa_position;
	Point3f left_sofa_position;
	Point3f right_sofa_position;
	Point3f tea_table_position;*/

	/** ��λɳ�� �輸 */
	const string package1_id="10161019";
	/** ��λɳ�� �輸 ��λɳ��-�� */
	const string package2_id="101610171019";
	/** ��λɳ�� �輸 ��λɳ��-�� */
	const string package3_id="101610181019";
	/** ��λɳ�� �輸 ��λɳ��-�� ��λɳ��-�� */
	const string package4_id="1016101710181019";

	/** pi */
	const double pi = acos(-1);

	/** ��ɵİ� */
	shared_ptr<ModelPackageJ> end_model_package;
};