/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:动态布局算法
*  简要描述:
*
*  当前版本:V1.0.0
*  作者:jirenze
*  创建日期:2018-09-06
*  说明:
**************************************************/

#pragma once
#include "MetisFormulaContainer.h"
#include "MetisRegionResolver.h"
#include "../Model.h"
#include "../ComStruct.h"

/************************************
*
*    对家具进行布局，处理布局信息
*
*************************************/
class MetisDynamicLayout
{
public:

	/** 进行区域权重解析 */
	static void CalculateIndependentExcellentRegion(shared_ptr<MetisRegionResolver>& region_resolver);

	/** 对依附性家具进行摆放 */
	static void CalculateAttachExcellentRegion(shared_ptr<MetisRegionResolver>& region_resolver);

	/** 独立性家具 */
	/** 检索区域权重图,筛选合适的解 默认取5个,并排序 */
	static void PickIndependentSuitableResult(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_weight_data, FurnitureWeight& furniture_weight_map, Model& furniture, shared_ptr<MetisRegionResolver>& region_resolver);

	/** 0°遍历权重图 */
	static void TraveseRoomLeaf_Independent_0(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_weight_data, FurnitureWeight& furniture_weight_map, Model& furniture, shared_ptr<MetisRegionResolver>& region_resolver, float& min_score, int& min_score_index);

	/** 270°遍历权重图 */
	static void TraveseRoomLeaf_Independent_270(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_weight_data, FurnitureWeight& furniture_weight_map, Model& furniture, shared_ptr<MetisRegionResolver>& region_resolver, float& min_score, int& min_score_index);

	/** 依附性家具 */
	/** 检索区域权重图,筛选合适的解 默认取5个,并排序 */
	static void PickAttachSuitableResult(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_weight_data, FurnitureWeight& furniture_weight_map, Model &furniture, shared_ptr<MetisRegionResolver> & region_resolver);

	/** 0°遍历权重图 */
	static void TraveseRoomLeaf_Attach_0(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_weight_data, FurnitureWeight& furniture_weight_map, Model &furniture, shared_ptr<MetisRegionResolver> &region_resolver, float &min_score, int &min_score_index);

	/** 270°遍历权重图 */
	static void TraveseRoomLeaf_Attach_270(vector<SingleFurnitureAllRuleLayoutData>& single_furniture_weight_data, FurnitureWeight& furniture_weight_map, Model &furniture, shared_ptr<MetisRegionResolver> &region_resolver, float &min_score, int &min_score_index);

	/** 筛选出数组中分数最小值的Index */
	static int GetMinScoreIndex(vector<float> &vaild_score);

	/** 对筛选出的数组数据按照分数由小到大进行排序 */
	static void SortExcellent(vector<SingleFurnitureAllRuleLayoutData> &furniture_layout_data);

	/** 解析独立家具的单个规则 */
	static void CalculateIndependentFurnitureSingleRule(shared_ptr<MetisRegionResolver> & region_resolver);

	/** 匹配独立性家具的所有规则 */
	static void MatchIndependentAllRule(int furniture_index, map<int, SingleFurnitureAllRuleLayoutData> temp_single_furniture_all_map, shared_ptr<MetisRegionResolver>& region_resolver);

	/** 所有独立家具的单个规则的布局组合 */
	static void TraveseIndependentFurnitureSingleRule(map<int, SingleFurnitureAllRuleLayoutData>& single_rule_layout, int furniture_index, map<int, SingleFurnitureSingleRuleLayoutData> temp_single_furniture_map, shared_ptr<MetisRegionResolver>& region_resolver);

	/** 独立家具方案所有组合的方案筛选 */
	static void PickIndependentFurnitureLayout(shared_ptr<MetisRegionResolver>& region_resolver);

	/** 匹配依附性家具 */
	static void MatchAttachFurniture(shared_ptr<MetisRegionResolver>& region_resolver);

	/** 好方案排序 */
	static void SortProject(shared_ptr<MetisRegionResolver>& region_resolver);

	/** 修改家具信息 */
	static void ReviseFurnitureInformaiton(shared_ptr<MetisRegionResolver> & region_resolver, SingleCase* single_case);

	/** 强规则 */
	static void ScalableFurniture(shared_ptr<MetisRegionResolver> & region_resolver);

	/** 后期优化家具布局 */
	static void OptimizedFurnitureLayout(shared_ptr<MetisRegionResolver> & region_resolver);

	/** 处理床头柜方向 */
	static void ManageBedside(shared_ptr<MetisRegionResolver> & region_resolver);

	/** 数据传递给singcase */
	static void TransferToSingcase(shared_ptr<MetisRegionResolver>& region_resolver, SingleCase* single_case);

	/** 测试延展构件ID */
	static const int scale_model_id = 10010;

	/** 每套规则生成方案数量 */
	static const int solution_num = 2;
};

