/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:户型中的墙标
*  简要描述:
*
*  当前版本:
*  作者:冯小翼
*  创建日期:2018-09-05
*  说明:
**************************************************/
#pragma once

#include "TokenStream.h"
#include "MetisRegionResolver.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "../DatabaseHelper.h"
class SingleCase;
//家具属性依赖或无依赖,对于依赖的家具可能具有两种属性
enum FormulaType
{
    INDEPENDENT,
    DEPENDENT
};

struct MetisFuinitureFormula
{
    FormulaType  formulaType;
	ETokenType Type;
	ETokenType Operator;
	int		id;
	float     Dist;
	float     Ratio;
};

 
class MetisFormulaContainer
{
public:
	MetisFormulaContainer();
    ~MetisFormulaContainer()= default;
	/*****************************************************************
	* 函数名：ConstructWeightMapWithIndependent
	*
	*  @param:   区域解析器指针 , 单区域指针
	*
	*  @return:	成功返回true
	*****************************************************************/
	bool   ConstructWeightMapWithIndependent( MetisRegionResolver*pregionResolverRef, SingleCase*cases);
	/*----!!!!!!!!!!!!!!!!!!!!!!!!测试使用!!!!!!!!!!!!!!!!!!!!!!----*/
    bool   ConstructWeightMapIndependentTest(MetisRegionResolver*pregionResolverRef, SingleCase*cases);
    /*****************************************************************
    * 函数名：ConstructWeightMapWithAllType
    *
    *  @param:   区域解析器指针 , 单区域指针
    *
    *  @return:	成功返回true
    *****************************************************************/
    bool   ConstructWeightMapWithAllType(MetisRegionResolver*pregionResolverRef, SingleCase*cases);
	/*----!!!!!!!!!!!!!!!!!!!!!!!!测试使用!!!!!!!!!!!!!!!!!!!!!!----*/
    bool  ConstructWeightMapWithAllTypeTest(MetisRegionResolver*pregionResolverRef, SingleCase*cases);
    
	//常用结构
	using FormulaContainer = std::vector< std::vector<std::shared_ptr<MetisNode>>>;
	using MultiContainer = std::vector<FurnitureWeight>;
	using FurnitureFormulaArray= std::vector<MetisFuinitureFormula>;
	using LayoutMap = std::map<int, SingleFurnitureSingleRuleLayoutData>;
private:
	//生成无依赖的家具
	bool GenerateFormula();
    //生成有依赖或者无依赖的
    bool GenerateAllFormulaType();
	//户型权重图
	bool ConstructFurnitureWeightRegion(LayoutMap&per_case,FormulaContainer&node_container);
	//家具权重
	bool ConstructWeightRegion(FormulaContainer &container);
	//靠墙
	void CalculateCloseWall(const MetisFuinitureFormula&formulaRef,FormulaContainer&container);
	//靠窗
	void CalculateCloseWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//靠门
	void CalculateCloseDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//避门
	void CalculateDivergeDoor(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//闭窗
	void CalculateDivergeWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//靠有窗的墙
	void CalculateCloseWallWithWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//靠有门的墙
	void CalculateCloseWallWithDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//靠有门和窗的墙
	void CalculateCloseWallWithWinDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//靠无门和窗的墙
	void CalculateCloseWallNothing(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//靠角落的墙
	void CalculateCloseWallCorner(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//间隔墙
	void CalculateIntervalWall(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//间隔Win的墙
	void CalculateIntervalWithWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//间隔Door的墙
	void CalculateIntervalWithDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//间隔Door的墙
	void CalculateIntervalWithDoorWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//间隔无洞的墙
	void CalculateIntervalWithNothing(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//对着门的墙
	void CalculateWallParallelWithDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//对着窗的墙
	void CalculateWallParallelWithWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//门左边的墙段
	void CalculateWallSectionOnLeftOfDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//门右边的墙段
	void CalculateWallSectionOnRightOfDoor(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//门左边的墙段
	void CalculateWallSectionOnLeftOfWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//门右边的墙段
	void CalculateWallSectionOnRightOfWin(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//门开门方向的墙
	void CalculateWallOnOrientationOfDoor(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//非开门方向的墙
	void CalculateWallOnReverseOfDoor(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//窗的右边所有墙
	void CalculateWallOnLeftOfWin(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//窗的左边所有墙
	void CalculateWallOnRightOfWin(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//门的扇形区域
	void CalculateSectorRegionWithDoor(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //窗的矩形区域
	void CalculateSectorRegionWithWin(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //开门方向扇形区域
	void CalculateSectorRegionOnOrientationOfDoor(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //非开门方向扇形区域
	void CalculateSectorRegionOnReverseOfDoor(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //离门最近的墙角
	void CalculateCornerWithNearestDistToDoor(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //离门最远的墙角
	void CalculateCornerWithFarestDistToDoor(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//阳角
	void CalculateClosePositiveCorner(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//阴角
	void CalculateCloseNegativeCorner(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//靠实墙
	void CalculateCloseActualWall(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	
	//马桶点位
	void CalculateToiletRegion(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//靠近虚墙
	void CalculateCloseVirtualWall(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//靠近入户门
	void CalculateCloseEntranceDoor(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//中心点辐射
	void CalculateCentreRadiation(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//闭阳光门
	void CalculateDivergeSunshineDoor(const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//避开非正常门(一般指非阳光门)
	void CalculateDivergeNormalDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//靠入光口左边墙
	void CalculateCloseWallOnLeftOfLightEntrance(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//靠入光口左边墙
	void CalculateCloseWallOnRightOfLightEntrance(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//靠入光口墙
	void CalculateCloseWallWithLightEntrance(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);
	//strip行区域
	void CalculateWeightStripBetweenMaxWallWithCentre(const MetisFuinitureFormula&formulaRef, FormulaContainer&container);


	//家具方向的墙体
	void FCalculateOrientationWall(const LayoutMap&map,const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //家具左方向的墙体
    void FCalculateLeftWall(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //家具右方向的墙体
    void FCalculateRightWall(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//家具反方向的墙体
    void FCalculateReverseWall(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //家具方向的墙段
	void FCalculateOrientationWallSection(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //家具左方向的墙段
    void FCalculateLeftWallSection(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //家具右方向的墙段
    void FCalculateRightWallSection(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //家具反方向的墙段
    void FCalculateReverseWallSection(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//家具正反向辐射中心点向两边扩散
	void FCalculateOrientationWallRadiation(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	
	//家具方向左墙段
	void FCalculateOrientationWallSectionOnLeft(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //家具方向右墙段
    void FCalculateOrientationWallSectionOnRight(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
	//家具反方向左墙段
    void FCalculateReverseWallSectionOnLeft(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //家具反方向右墙段
    void FCalculateReverseWallSectionOnRight(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //家具方向矩形区域
	void FCalculateOrientataionLineRegion(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //家具左方向矩形区域
    void FCalculateLeftLineRegion(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //家具右方向矩形区域
    void FCalculateRightLineRegion(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //家具反方向矩形区域
    void FCalculateReverseLineRegion(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //家具方向扇形区域
	void FCalculateOrientataionSectorRegion(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //家具左方向扇形区域
    void FCalculateLeftSectorRegion(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //家具右方向扇形区域
    void FCalculateRightSectorRegion(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
    //家具反方向扇形区域
    void FCalculateReverseSectorRegion(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container);
 
    //获取依赖方向向量
	void GetOrientationFromLayout(const SingleFurnitureSingleRuleLayoutData&Ref,Point3f*Orientation,Point3f*Start,Point3f*End);
	void GetLeftFromLayout(const SingleFurnitureSingleRuleLayoutData&Ref, Point3f*Orientation, Point3f*Start, Point3f*End);
	void GetRightFromLayout(const SingleFurnitureSingleRuleLayoutData&Ref, Point3f*Orientation, Point3f*Start, Point3f*End);
	void GetReverseFromLayout(const SingleFurnitureSingleRuleLayoutData&Ref, Point3f*Orientation, Point3f*Start, Point3f*End);
private:
    //填充数组
	void FillNodeArray(const MetisFuinitureFormula&formulaRef,FormulaContainer&container);

    TokenStream 					m_stream;
	FurnitureFormulaArray			m_furniture_formula_array;
	MetisRegionResolver*			p_region_resolver;
	SingleCase*						p_single_case;
	shared_ptr<DatabaseHelper>		p_database_helper;
 
};