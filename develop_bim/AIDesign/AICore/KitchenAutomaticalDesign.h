/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:内存池模板类
*  简要描述:
*
*  当前版本: V 1.0.0
*  作者:冯小翼
*  创建日期:2018-09-29
*  说明: KitchenDesign
**************************************************/
#pragma once
#include "../SingleCase.h"
#include <unordered_map>
#include <set>
/*----------------------------
* 厨房自动设计类:KitchenDesign
-----------------------------*/
#define  Windows_Kitchen_Case			"windows"
#define  MaxWall_Kitchen_Case			"wall"
#define  Coorker_Kitchen_Case				"cooker"
//#define  CabinetRuleNo						"cabenit override here"
//#define  WatersoltRuleNo						"Watersolt override here"
//#define	  FridheRuleNo							"Fridge rule override here"
//#define  CookerRuleNo							"Cooker rule override here"


class KitchenDesign
{
public:
	KitchenDesign() = default;
	~KitchenDesign() = default;
public:
	enum WallType {
		E_WaterPointWall  ,
		E_ChmineyWall ,
		E_DoorReverseWall 
	};
	enum FurnitureType
	{
		E_Predinner = 0,
		E_Cabinet = 30016,
		E_Water_Solt = 30020,
		E_Cooker = 30022,
		E_Fridge = 20003,
	};
	struct Container
	{

		std::vector<FurnitureType>  furnitureType;
		std::vector<Point3f>		furnitureBeg;
		std::vector<Point3f>		furnitureEnd;
		Point3f						fridgeBeg;
		Point3f						fridgeEnd;
		std::set<FurnitureType>		fridgeType;
	};
	struct CabinetWall
	{
		Wall*								p_wall;
		Point3f								direction;
		std::vector<Point3f>				cabinetBeg;
		std::vector<Point3f>				cabinetEnd;
		std::vector<Point3f>				furnitureBeg;
		std::vector<Point3f>				furnitureEnd;
		std::vector<Container>				subLayout;
		std::vector<Corner*>				Corner;	
	};
	struct OneKitchenCase
	{
		  std::vector<CabinetWall> real_case;
		  std::string						  regular_description;
	};
public:
	/*--------------------------------------------------------
	*
	* AutoDesign()  1.输入单房间区域,一般是指厨房区域
	*				2.自动布局,如果布局成功 true:有方案生成
					3.无方案返回false,失败可能是厨房户型太小
	----------------------------------------------------------*/
	bool AutoDesign(SingleCase*single_case);
private:
	/*!!!!!!!!!!!!!!!!测试用!!!!!!!!!!!!!!!!!!!!!*/
	auto	Get() {
		return m_all_case;
	}
	/*生成模型坐标信息*/
	void GenerateModeLayout(SingleCase*single_case);
	/* 寻找候选墙体*/
	void SearchBenchmarkWall(SingleCase*single_case);
	/* 选择与水位相关的候选墙体*/
	void SearchWaterPointWall(SingleCase*single_case);
	/* 选择与风烟道相关的候选墙体*/
	void SearchChmineyWall(SingleCase*single_case);
	/* 选择与门方向相关的候选墙体*/
	void SearchReverseWall(SingleCase*single_case);
	/*选择基准墙体*/
	Wall* ChooseBenckWall(SingleCase*single_case);
	/*计算基准墙体的分数*/
	float CalculateBenchmarkWall(std::set<WallType>&wallType);
	/*添加非门的候选墙体*/
	void AddOtherWallToCandidateWall(Wall* wall, SingleCase*single_case);
	/*检测与门的冲突，以及走线冲突*/
	bool DetectCollisionWithDoor(SingleCase*single_case);
	/*切分有门的墙体*/
	void	WallWithDoorCollision(Wall*wall, SingleCase*single_case); 
	void WallWithoutDoorCollision(Wall*wall, std::vector<Door*>&door, SingleCase*single_case);
	/*萎缩有效墙体*/
	void ShrinkCabinetWall(SingleCase*single_case);
	/*计算柜体或者墙的方向*/
	void CalculateCabinetWallDirection(SingleCase*single_case);
	/*窗户基准布局*/
	void CaseLayoutWithWindows();
	/*最大墙体基准布局*/
	void CaseLayoutWithMaxWall();
	/*灶具基准布局*/
	void CaseLayoutWithCoorker();
	/*寻找最长的墙*/
	void FindMaxWall(CabinetWall**wall,size_t *index,float *length,std::vector<CabinetWall>&Case);
	/*寻找一个区间长度的墙体*/
	void FindCompatible(CabinetWall**wall, size_t *index, std::vector<CabinetWall>&Case, float min, float max);
	/*进行布局，包括冰箱布置*/
	void CompatibleLayout(SingleCase*single_case);
	/*布局冰箱的点位*/
	void CompatibleLayoutWithFridge(OneKitchenCase &Case, SingleCase*single_case);
	/*求出所有器具的世界坐标点*/
	void SolveAllFruniturePostion(SingleCase*single_case);
	/*计算需求最小长度*/
	float CalculateMinimumLength(std::vector<FurnitureType>&Ref);
	//计算需求最大长度*/
	float CalculateMaximumLength(std::vector<FurnitureType>&Ref);
	/*计算一个类型的最小长度*/
	float CalculateMinimumLength(FurnitureType Ref);
	/*计算一个类型的最大长度*/
	float CalculateMaximumLength(FurnitureType Ref);
	/*同上，但未使用*/
	float CalculateMinRealLength(std::vector<FurnitureType>&Ref);
	/*同上，但未使用*/
	float CalculateMaxRealLength(std::vector<FurnitureType>&Ref);

private:
	/**基准墙体*/
	std::unordered_map<Wall*, std::set<WallType>>	m_benckmark_wall;
	/**候选墙体*/
	std::vector<Wall*>												m_candidate_wall;
	/**柜体墙*/
	std::vector<CabinetWall>										m_cabinet_wall;
 
	/**所有方案布局*/
	std::vector<OneKitchenCase>								m_all_case;
	
};

