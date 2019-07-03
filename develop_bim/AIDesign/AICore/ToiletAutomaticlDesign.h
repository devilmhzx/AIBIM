/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  Author:fengxiaoyi
*  Create Date:2018-11.10
*  Description:���������
**************************************************/
#pragma once
#include "../SingleCase.h"
#include <set>
/* ����ģ�͵��ڲ����� */


#define		Commode_Extend    5.f
#define     DynanmicLayoutStep 10.f
#define     Commode_Dis 50
#define     Toilet_Area 4
#define     Toilet_Area 4
#define	    Sprinkler_Extend    20.f

/**��������ԡ���滻*/
//#define		Model_Sprinkler_Id	    30012
//#define		Model_Bathroom_Id	    30006
//#define		Model_Bathroom_Width	    80
//#define		Model_Bathroom_Length	    80

#define		Model_Inner_Type	    -1

struct BoundingBox
{
	Point3f Max;
	Point3f	Min;
	Point3f Direction;
	Model	model;
	Wall*	Wall;
	int		Inner_type;
	Point3f location;
	bool flag;
};

struct CornerData
{
	Corner corner;
	float distance;
};

bool operator<(const BoundingBox&left, const BoundingBox&right);

class ToiletAutomaticlDesign
{
public:
	ToiletAutomaticlDesign();
	~ToiletAutomaticlDesign();
public:
	 


	using  ToiletDesign = std::vector<std::set<BoundingBox>>;
	using  BoundingBoxSet= std::set<BoundingBox>;
public:
	bool AutoDesign(SingleCase*single_case);
private:
	// ��ʼ���ŵĲ���
	void InitLayoutDoor();
	//������Ͱ
	void LayoutCommode();
	// ��þ����Ž�Զ��ǽ��
	vector<CornerData> GetFastCornerDataWithDoor();
	// ��þ����ŽϽ���ǽ��
	vector<CornerData> GetNearCornerDataWithDoor();
	//���ֻ���
	void LayoutSprinkler();
	//����ԡ��
	void LayoutNormalBathroom();
	//����һ��ԡ��
	void LayoutBathroomWithLineShape();
	//����ԡ��
	void LayoutCabinet();
	//����ԡ��
	void LayoutBathtub();
	// ��Ⲽ���Ƿ����Ž�������ײ
	bool IsCollisionDoor(BoundingBox box);
	// ��չ���ֽ�����Ч���ֵ����
	void ExpandLayout();
	// ���ɺ�ԡ�׵Ĳ���
	void ExpandBathtubLayout();
	// �ж�ԡ���ڲ������Ƿ����
	bool CheckBathtubLayout(BoundingBoxSet&single_layout,BoundingBox bathtub);
	//����ϴ�»�
	void LayoutWasher(ToiletDesign &ref);
	//����ǽ���Զ�����ϴ�»�
	bool DynamicLayoutWasher(BoundingBoxSet&single_layout,Wall*wall,Model model);
	//���Ž����Զ�����ϴ�»�
	bool DynamicLayoutWasherByCorner(BoundingBoxSet&single_layout, Corner*corner, Model model);
	// ���ɽ��
	void FillModelToSingleCase ();
	//
	void FillModelToModelLayout(ModelLayout&layout, BoundingBoxSet&Set);
	// ���ҿɲ��ֵ�ģ��
	Model FindLayoutModel(int inner_type);
	// ͨ��ǽ������ŵĲ���
	BoundingBox CalculateDoorByWall(Door*door, Wall*wall);
	// ͨ��ǽ��ĵ������Ͱ��box
	BoundingBox CalculateCommodeByWall(Point3f stool,Model model, Wall*wall);
	// ͨ��ǽ��ĵ���㻨����box
	BoundingBox CalculateSprinklerByWall(Corner*corner, Model model, Wall*wall);
	// ͨ��ǽ��ĵ������ԡ����box
	BoundingBox CalculateNormalBathroomByWall(Corner*corner, Model model, Wall*wall);
	// ͨ��ǽ��ĵ����һ������ԡ����box
	BoundingBox CalculateBathroomWithLineShape(Corner*corner, Model model, Wall*wall);
	// ͨ��ǽ�Ǽ���ԡ�ҹ��box
	BoundingBox CalculateCabinetByCorner(Point3f corner_point, Point3f end, Model model, Wall*wall);
	// ͨ��λ���������ԡ�ҹ��box
	BoundingBox CalculateCabinetByPoint(Point3f beg, Point3f end, Model model, Wall*wall);
	// ͨ��ǽ��ĵ����ԡ�׵�box
	BoundingBox CalculateBathtubByWall(Corner *corner, Model model, Wall*wall);
	//��������ϴ�»�
	void AloneLayoutWasher(Model model);
	// ����ǽ��ķ�������
	Point3f CalculateWallNormal(Wall*wall);
	// �Ե����ݰ��վ�������
	vector<CornerData> SortCornerData(vector<CornerData> src_data, int sort_type);
	// ���������Ƿ���ײ
	bool IsCollisionBoundingBox(BoundingBox src_box, BoundingBox des_box);
	// �õ�һ������Ĳ��ַ���,Ϊ����װʹ��
	BoundingBox GetVirtualBox(int inner_type);
	// ����һ���µĲ���
	set<BoundingBox> CopyBoundingBoxSet(BoundingBoxSet src_set);
private:
	 
	std::vector<BoundingBox>		door_layout;  // �ŵĲ���
	std::vector<BoundingBox>		commode_layout; // ��Ͱ����
	std::vector<BoundingBox>		sprinkler_layout; // ��������
	std::vector<BoundingBox>		bathroom_layout; // ��ԡ������
	std::vector<BoundingBox>		cabinet_layout;  // ԡ�ҹ񲼾�
	std::vector<BoundingBox>		bathtub_layout;  // ԡ�׵Ĳ���

	ToiletDesign								case_layout;
	SingleCase*								m_single_case;
};

