/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:区域解析器
*  简要描述:对区域进行操作
*
*  当前版本: V1.0.0
*  作者:jirenze
*  创建日期:2018-09-05
*  说明:
**************************************************/
#pragma once
#include "MetisNode.h"
#include "../SingleCase.h"
#include "../Model.h"
#include "../Corner.h"
#include "../Point2f.h"
#include "../Point3f.h"
#include "../Box2D.h"
#include <memory>
#include <vector>
#include <map>

#define Maker_Limit 5

/** 权重图和规则ID */
struct FurnitureWeight
{
	/** 规则权重图 */
	vector<vector<shared_ptr<MetisNode>>> regulation_map;

	/** 规则的no */
	string rule_no;
};


/** 每个规则的布局数据 */
struct SingleFurnitureAllRuleLayoutData
{
	/*****************************
	 *      vetor存有两套        *
	*****************************/

	/** 分数 */
	vector<float> vaild_scores;
	
	/** 转换点，即家具的旋转点位 */
	vector<Point3f> translation_points;
	
	/** 旋转角度 */
	vector<int> rotation_degrees;
	
	/** 家具的盒子的四个点 */
	vector<vector<Point3f>> furniture_vertex_pointss;
	
	/** 家具ID */
	vector<int> model_ids;

	/** 家具的width */
	vector<float> widths;

	/** 规则no */
	vector<string> rule_nos;
};

struct SingleFurnitureSingleRuleLayoutData
{
	/** 分数 */
	float vaild_score;

	/** 转换点，即家具的旋转点位 */
	Point3f translation_point;

	/** 旋转角度 */
	int rotation_degree;

	/** 家具的盒子的四个点 */
	vector<Point3f> furniture_vertex_points;

	/** 家具ID */
	int model_id;

	/** 家具的width */
	float width;

	/** 规则ID */
	string rule_no;
};

/** 布局方案细化出来 */
struct ResultData
{
	/** 总分 */
	float total_vaild_score;

	/** 单个布局结果map */
	map<int, SingleFurnitureSingleRuleLayoutData> single_layout_result;
};

// 处理家具吸附性的数据
struct AdsorptivityData
{
	int start_furniture_index;
	int end_furniture_index;
	Wall *p_wall;
};

/*********************************
*     对房间按照区域处理          
*     每个房间即为一个区域        
*     存储家具布局信息
*********************************/
class MetisRegionResolver
{
public:

	MetisRegionResolver() :generate_solution_num(2) {};

	MetisRegionResolver(SingleCase* singlecase);

	~MetisRegionResolver() {};

	/** 构造初始化 */
	void InitGenerateRegion(SingleCase* singlecase);

	/** 家具识别分类 分为独立性和依赖性家具 */
	void SortFurniture(vector<Model> &model_list);

	/** 整理区域盒子 */
	void ArrangeRegionBox(vector<Corner> &corner_list);

	/** 功能描述:创建二维指针数组 */
	void CreateArray2D(Box2D &room_box);

	/** 添加bInside标记 */
	void AddBinside();

	/** 添加bIndoor标记 */
	void AddBindoor();

	/** 添加bInfloorbox标记 */
	void AddBinfloorbox();

	/** 构建标准门所占80*length的区域 */
	void CreateNormalDoorRegion(Door &door, vector<Point3f>& temp_door_corner);

	/** 构建推拉门门所占20*length的区域 */
	void CreateSlidingDoorRegion(Door &door, vector<Point3f>& temp_door_corner);

	/**清理节点权重 */
	void ClearLeafWeight();

	/**清理权重指定值 */
	void ClearLeafWeightToValue(float Value);

	/** 添加一个独立性家具有效信息数组 */
	void AddIndependentLayoutDataArray(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_weight_data);

	/** 添加一个依赖性家具有效信息数组 */
	void AddAttachLayoutDataArray(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_weight_data);

	/** 处理客厅组合模型 */
	Model ProcessingLivingCombinedModel(vector<Model>& in_model);

public:
	/**房间覆盖区域节点 */
	vector<vector<shared_ptr<MetisNode>>> base_nodes;

	/** 当前解析的区域 */
	SingleCase*  p_region;

	/** 房间内需要按照规则摆放的家具 */
	vector<Model> independent_model_container;

	/** 房间内依附性的家具 */
	vector<Model> attach_model_container;

	/** 独立性家具公式权重map */
	map<int, vector<FurnitureWeight> > independent_regulation_map;

	/** 所有家具对应的所有布局权重结果 */
	map<int, vector<SingleFurnitureAllRuleLayoutData>> independent_furniture_all_rule_weight;

	//独立性家具的软布局容器,(规则匹配成布局)
	vector<map<int, SingleFurnitureAllRuleLayoutData>> independent_furniture_rule_layout;

	/** 具有权重家具的所有摆放方案，返给公式解析器 */
	vector<map<int, SingleFurnitureSingleRuleLayoutData>> independent_furniture_layout_combo_data;

	/** 依附性家具的权重map */
	vector< map<int, vector< FurnitureWeight >>> attach_regulation_map;

	/** 依附性家具权重结果 */
	vector < map<int, vector<SingleFurnitureAllRuleLayoutData>>> attach_furniture_all_rule_weight;

	/** 布局方案 */
	vector< vector<map<int, SingleFurnitureSingleRuleLayoutData>> > furniture_layout_result_vector;

	/** 所有家具的摆放方案 （排序完毕） */
	vector<Model> model_container;
	vector<map<int, SingleFurnitureSingleRuleLayoutData>> all_furniture_weight_combo_data;

	/** max_row */
	int max_row;

	/** max_rank */
	int max_rank;

	/** 房间点位 */
	vector<Point3f> room_corner;

	/** 10cm一格 */
	static const int BoxSize = 10;

	/** 测试使用：分数 */
	float score;

	/** 门占区域进深格子数 */
	static const int door_region_depth = 2;

	/** 客厅组合家具 */
	vector<Model> living_room_model;

private:
	/** 覆盖房间的盒子 */
	Box2D room_box;

	/** 每套规则生成方案数目 */
	const int generate_solution_num;

public:
	// 处理定制家具的边角问题
	bool ModifyCustomModel(SingleFurnitureSingleRuleLayoutData &model_layout, map<int, SingleFurnitureSingleRuleLayoutData> &design_layout);

	// 获得延展后的模型
	bool GetScaleModel(SingleFurnitureSingleRuleLayoutData &model_layout, map<int, SingleFurnitureSingleRuleLayoutData> design_layout);

	//  检测与家具的碰撞
	bool CheckModelWithLayout(SingleFurnitureSingleRuleLayoutData model_layout, map<int, SingleFurnitureSingleRuleLayoutData> &design_layout);

	//  家具吸附性处理
	bool CheckFurnitureAdsorption(map<int, SingleFurnitureSingleRuleLayoutData> &design_layout);

private:
	// 根据家具编号获得家具信息
	Model *GetModelInfoById(int model_id);

	// 检测模型与户型的碰撞
	// 使用时必须结合获得延展模型的操作，否则报错
	bool MetisRegionResolver::CheckModelWithHouse(Point3f p0, Point3f p1, Point3f p2, Point3f p3, Point3f &pA, Point3f &pB);

	// 家具是否需要吸附性处理，如果需要返回吸附性的点
	bool FurnitureAdsorption(SingleFurnitureSingleRuleLayoutData model_layout, AdsorptivityData &adsorption);

	// 家具是否靠近该墙体
	bool IsFurnitureOnWall(Point3f start_point, Point3f end_point, Wall *p_wall);
};
