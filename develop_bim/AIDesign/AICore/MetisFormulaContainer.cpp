#include "MetisFormulaContainer.h"
#include "../ComMath.h"
#include "../ComMath.h"
#include "../CaseMatch/MetisRegionMatcher.h"
#include <vector>
#include "../Log/easylogging++.h"

MetisFormulaContainer::MetisFormulaContainer()
{
	p_database_helper = std::make_shared<DatabaseHelper>();
}

bool MetisFormulaContainer::ConstructWeightMapWithIndependent(MetisRegionResolver*pregionResolverRef, SingleCase*cases)
{
	if (!(p_single_case = cases))
		return false;

	MetisRegionMatcher Matcher;
	if (!Matcher.FindHighSimilarityOnServerTemplate(p_single_case, 0))
	{
		LOG(ERROR) << "Not find similarity template(" << p_single_case->single_room.GetNo()<<")";
		return false;
	}

	auto home = Matcher.GetHighSimilarityTemplate();
	if (!(p_region_resolver = pregionResolverRef))
		return false;
	p_region_resolver->independent_regulation_map.clear();
    if(!p_database_helper)
		return false;
	//获取所有非依赖性的规则
	for (auto model : pregionResolverRef->independent_model_container)
	{
		int id = model.id;
		auto regular_list = p_database_helper->searchRegualrtionsStructByModelandSample(id, home.no);
		if (!regular_list.size())
		{
			LOG(ERROR) << "Not find regualrtions (id=" << id << " home_no= " << home.no << " )";
			return false;
		}
			
		MultiContainer multi_container;
		for (auto regular_struct : regular_list)
		{	 		 
			//生成token
			if (!m_stream.ProductTokenStream(regular_struct.regualrtion))
			{
				LOG(ERROR) << "Create token error:"<< regular_struct.regualrtion;
				return false;
			}
				
			//解析token
			if (!GenerateFormula())
			{
				LOG(ERROR) << "Analyze token error:" << regular_struct.regualrtion;
				return false;
			}
				
			//初始化节点
			FurnitureWeight Weight;
			FormulaContainer node_container;
			node_container.resize(p_region_resolver->max_row);
			for (int i = 0; i < p_region_resolver->max_row; ++i) {
				node_container[i].resize(p_region_resolver->max_rank);
				for (int j = 0; j < p_region_resolver->max_rank; ++j) {
					std::shared_ptr<MetisNode> snode = std::make_shared<MetisNode>();
					snode->Weight = 0.f;
					node_container[i][j] = snode;
				}
			}
			if (!ConstructWeightRegion(node_container))
				return false;
			//移动到权重图 和规则id
			Weight.regulation_map = std::move(node_container);
			Weight.rule_no = regular_struct.no;
			multi_container.push_back(std::move(Weight));
		}
		pregionResolverRef->independent_regulation_map.insert(std::make_pair(id, std::move(multi_container)));
	}
	return true; 
}

// 解析非依赖性规则-测试
bool   MetisFormulaContainer::ConstructWeightMapIndependentTest(MetisRegionResolver*pregionResolverRef, SingleCase*cases)
{
    if (!(p_single_case = cases))
        return false;
	MetisRegionMatcher Matcher;
	if (!Matcher.FindHighSimilarityOnServerTemplate(p_single_case, 0))
		return false;
	auto home = Matcher.GetHighSimilarityTemplate();

    if (!(p_region_resolver = pregionResolverRef))
        return false;
    p_region_resolver->independent_regulation_map.clear();
    if (!p_database_helper)
        return false;
    std::vector<FurnitureWeight> multi_container;
    //获取所有非依赖性的规则

	std::string bwll = "#rule=+DDoor * 100.000000*50.000000$";
    if (!m_stream.ProductTokenStream(bwll))
        return false;
    //解析token
    if (!GenerateFormula())
        return false;
    //初始化节点
	FormulaContainer node_container;
	node_container.resize(p_region_resolver->max_row);
	for (int i = 0; i < p_region_resolver->max_row; ++i) {
		node_container[i].resize(p_region_resolver->max_rank);
		for (int j = 0; j < p_region_resolver->max_rank; ++j) {
			std::shared_ptr<MetisNode> snode = std::make_shared<MetisNode>();
			snode->Weight = 0.f;
			node_container[i][j] = snode;
		}
	}
    if (!ConstructWeightRegion(node_container))
        return false;
	FurnitureWeight weight;
	weight.rule_no = "";
	weight.regulation_map = std::move(node_container);
    multi_container.push_back(std::move(weight));
   pregionResolverRef->independent_regulation_map.insert(std::make_pair(0, std::move(multi_container)));
    return true;
}

// 解析依赖性规则
bool   MetisFormulaContainer::ConstructWeightMapWithAllType(MetisRegionResolver*pregionResolverRef, SingleCase*cases)
{

    if (!(p_single_case = cases))
        return false;
	MetisRegionMatcher Matcher;
	if (!Matcher.FindHighSimilarityOnServerTemplate(p_single_case, 0))
	{
		LOG(ERROR) << "Not find similarity template(" << p_single_case->single_room.GetNo() << ")";
		return false;
	}
		
	auto home = Matcher.GetHighSimilarityTemplate();

    if (!(p_region_resolver = pregionResolverRef))
        return false;
    p_region_resolver->attach_regulation_map.clear();
    //获取所有公式
	// int 是模型id   std::pair.first 是规则描述no std::pair.seconed 是对应规则 
    std::map<int, std::vector<RegualrtionsStruct>> model_formula;
    for (auto &model : p_region_resolver->attach_model_container)
    {
		auto regular_list = p_database_helper->searchRegualrtionsStructByModelandSample(model.id, home.no);
	
        //没有找到对应公式
		if (!regular_list.size())
		{
			LOG(ERROR) << "Not find regualrtions (id=" << model.id << " home_no= " << home.no << " )";
			return false;
		}
        model_formula.insert(std::make_pair(model.id, std::move(regular_list)));
    }
    //对于每一套方案
    for (auto &per_case : p_region_resolver->independent_furniture_layout_combo_data)
    {
        std::map<int, MultiContainer> per_case_weight;
        for (auto &model : p_region_resolver->attach_model_container)
        {
            //搜索ID 拿到对应的一套规则
            auto iter = model_formula.find(model.id);
            auto string_one_model = iter->second;
            MultiContainer multi_container;
            for (auto &regualrtion_package : string_one_model)
            {
                //生成token
				if (!m_stream.ProductTokenStream(regualrtion_package.regualrtion))
				{
					LOG(ERROR) << "Create token error:" << regualrtion_package.regualrtion;
					return false;
				}
                    
                //解析token
				if (!GenerateAllFormulaType())
				{
					LOG(ERROR) << "Analyze token error:" << regualrtion_package.regualrtion;
					return false;
				}
                //初始化节点
                FormulaContainer node_container;
				node_container.resize(p_region_resolver->max_row);
				for (int i = 0; i < p_region_resolver->max_row; ++i) {
					node_container[i].resize(p_region_resolver->max_rank);
					for (int j = 0; j < p_region_resolver->max_rank; ++j) {
						std::shared_ptr<MetisNode> snode = std::make_shared<MetisNode>();
						snode->Weight = 0.f;
						node_container[i][j]=snode;
					}
				}
                //确保所有依赖的在case中能找到
                bool find = true;
                for (auto formula : m_furniture_formula_array)
                {
                    if (formula.formulaType == FormulaType::DEPENDENT) {
                        if (per_case.find(formula.id) == per_case.end())
                            find = false;
                    }      
                }
                //没找到存在的组件布局完整
                if (!find)
                    continue;
                if (!ConstructFurnitureWeightRegion(per_case, node_container))
                    return false;     
				FurnitureWeight weight;
				//插入模型规则 和描述
				weight.regulation_map = std::move(node_container);
				weight.rule_no = std::move(regualrtion_package.no);
                multi_container.push_back(std::move(weight));
            }
            per_case_weight.insert(std::make_pair(model.id, std::move(multi_container)));
        }
        p_region_resolver->attach_regulation_map.push_back(std::move(per_case_weight));
    }
    return true;
}

// 解析依赖性规则-测试
bool  MetisFormulaContainer::ConstructWeightMapWithAllTypeTest(MetisRegionResolver*pregionResolverRef, SingleCase*cases)
{
    if (!(p_single_case = cases))
        return false;
    if (!(p_region_resolver = pregionResolverRef))
        return false;
    p_region_resolver->attach_regulation_map.clear();
    //获取所有公式
    LayoutMap map;
    SingleFurnitureSingleRuleLayoutData data;
    data.rotation_degree = 90;
    data.translation_point = Point3f(0, -150, 0);
    data.furniture_vertex_points.push_back(Point3f(-50, -50, 0) + data.translation_point);
    data.furniture_vertex_points.push_back(Point3f(50, -50, 0) + data.translation_point);
    data.furniture_vertex_points.push_back(Point3f(50, 50, 0) + data.translation_point);
    data.furniture_vertex_points.push_back(Point3f(-50, 50, 0) + data.translation_point);

    map.insert(std::make_pair(0, data));

    p_region_resolver->independent_furniture_layout_combo_data.push_back(map);
   //搜索ID 拿到对应的一套规则
    std::string string= "#rule=+0@FOrientationWall*100*1+ CWin*100*1$";
           MultiContainer multi_container;
      
                //生成token
                if (!m_stream.ProductTokenStream(string))
                    return false;
                //解析token
                if (!GenerateAllFormulaType())
                    return false;
                //初始化节点
				FormulaContainer node_container;
				node_container.resize(p_region_resolver->max_row);
				for (int i = 0; i < p_region_resolver->max_row; ++i) {
					node_container[i].resize(p_region_resolver->max_rank);
					for (int j = 0; j < p_region_resolver->max_rank; ++j) {
						std::shared_ptr<MetisNode> snode = std::make_shared<MetisNode>();
						snode->Weight = 0.f;
						node_container[i][j] = snode;
					}
				}
     
                if (!ConstructFurnitureWeightRegion(map, node_container))
                    return false;
				FurnitureWeight weight;
				weight.regulation_map = std::move(node_container);
				weight.rule_no = "";
                multi_container.push_back(weight);
            std::map<int, MultiContainer> per_case_weight;
            per_case_weight.insert(std::make_pair(0, std::move(multi_container)));
 
        p_region_resolver->attach_regulation_map.push_back(std::move(per_case_weight));
 
    return true;
}

bool MetisFormulaContainer::GenerateFormula()
{
	m_furniture_formula_array.clear();
	auto &tokenStream = m_stream.m_stream;
	if (!tokenStream.size())
		return false;
	size_t index = 0;
	if (!tokenStream[index].Type==E_POUND)
		return false;
	if (++index >= tokenStream.size())
		return false;
	if (!tokenStream[index].IsIdentity()|| tokenStream[index].Name!="rule")
		return false;
	if (++index >= tokenStream.size())
		return false;
	if (!tokenStream[index++].IsEqul())
		return false;
	if (index >= tokenStream.size())
		return false;
	for (auto i = index; i < tokenStream.size();i)
	{
		MetisFuinitureFormula formula;
		if ((tokenStream[i].Type != E_Plus && tokenStream[i].Type != E_Minus))
			return false;
		if (tokenStream[i].Type == E_Plus)
			formula.Operator = E_Plus;
		if (tokenStream[i].Type == E_Minus)
			formula.Operator = E_Minus;
		if (++i >= tokenStream.size())
			return false;
		if (!tokenStream[i].IsEntity())
			return false;
		formula.Type = tokenStream[i].Type;
		if (++i >= tokenStream.size())
			return false;
		if (tokenStream[i].Type != E_Mul || ++i >= tokenStream.size())
			return false;
		if (!tokenStream[i].IsNumber())
			return false;
		formula.Dist = tokenStream[i].Number;
		if (++i >= tokenStream.size())
			return false;
		if (tokenStream[i].Type != E_Mul || ++i >= tokenStream.size())
			return false;
		if (!tokenStream[i].IsNumber())
			return false;
		formula.Ratio = tokenStream[i].Number;
		if (++i >= tokenStream.size())
			return false;
        m_furniture_formula_array.push_back(formula);
		if (tokenStream[i].Type == E_DOLLAR) {	 
			if (++i != tokenStream.size())
				return false;
			return true;
		}
	}
	return true;
}

bool MetisFormulaContainer::GenerateAllFormulaType()
{
    auto &tokenStream = m_stream.m_stream;
    if (!tokenStream.size())
        return false;
    int index = 0;
    if (!tokenStream[index].Type == E_POUND)
        return false;
    if (++index >= tokenStream.size())
        return false;
    if (!tokenStream[index].IsIdentity() || tokenStream[index].Name != "rule")
        return false;
    if (++index >= tokenStream.size())
        return false;
    if (!tokenStream[index++].IsEqul())
        return false;
    if (index >= tokenStream.size())
        return false;
    m_furniture_formula_array.clear();
    for (auto i = index; i < tokenStream.size(); i)
    {
        MetisFuinitureFormula formula;
        if ((tokenStream[i].Type != E_Plus && tokenStream[i].Type != E_Minus))
            return false;
        if (tokenStream[i].Type == E_Plus)
            formula.Operator = E_Plus;
        if (tokenStream[i].Type == E_Minus)
            formula.Operator = E_Minus;
        if (++i >= tokenStream.size())
            return false;
        if (!tokenStream[i].IsNumber() && !tokenStream[i].IsEntity())
            return false;
        //有依赖的
        if (tokenStream[i].IsNumber()) {
            formula.formulaType = FormulaType::DEPENDENT;
            formula.id = (int)tokenStream[i].Number;
            if (++i >= tokenStream.size())
                return false;
            if (tokenStream[i].Type != E_AT)
                return false;
            if (++i >= tokenStream.size())
                return false;
            if (!tokenStream[i].IsEntity())
                return false;
            formula.Type = tokenStream[i].Type;
            if (++i >= tokenStream.size())
                return false;
            if (tokenStream[i].Type != E_Mul || ++i >= tokenStream.size())
                return false;
            if (!tokenStream[i].IsNumber())
                return false;
            formula.Dist = tokenStream[i].Number;
            if (++i >= tokenStream.size())
                return false;
            if (tokenStream[i].Type != E_Mul || ++i >= tokenStream.size())
                return false;
            if (!tokenStream[i].IsNumber())
                return false;
            formula.Ratio = tokenStream[i].Number;
            m_furniture_formula_array.push_back(formula);
        }
        //无依赖的
        else
        {
            formula.formulaType = FormulaType::INDEPENDENT;
            formula.Type = tokenStream[i].Type;
            if (++i >= tokenStream.size())
                return false;
            if (tokenStream[i].Type != E_Mul || ++i >= tokenStream.size())
                return false;
            if (!tokenStream[i].IsNumber())
                return false;
            formula.Dist = tokenStream[i].Number;
            if (++i >= tokenStream.size())
                return false;
            if (tokenStream[i].Type != E_Mul || ++i >= tokenStream.size())
                return false;
            if (!tokenStream[i].IsNumber())
                return false;
            formula.Ratio = tokenStream[i].Number;
            m_furniture_formula_array.push_back(formula);
        }
        if (++i >= tokenStream.size())
            return false;  
        if (tokenStream[i].Type == E_DOLLAR) {
            if (++i != tokenStream.size())
                return false;
            return true;
        }
    }
    return true;
}

bool MetisFormulaContainer::ConstructFurnitureWeightRegion(LayoutMap&single_case, FormulaContainer&node_container)
{
	for (auto Formula : m_furniture_formula_array)
	{
		switch (Formula.Type)
		{
		case E_CloseWall:
			CalculateCloseWall(Formula, node_container);
			break;
		case E_CloseWin:
			CalculateCloseWin(Formula, node_container);
			break;
		case E_CloseDoor:
			CalculateCloseDoor(Formula, node_container);
			break;
		case E_DivergeDoor:
			CalculateDivergeDoor(Formula, node_container);
			break;
		case E_DivergeWin:
			CalculateDivergeWin(Formula, node_container);
			break;
		case E_CloseWallWithWin:
			CalculateCloseWallWithWin(Formula, node_container);
			break;
		case E_CloseWallWithDoor:
			CalculateCloseWallWithDoor(Formula, node_container);
			break;
		case E_CloseWallWithWinDoor:
			CalculateCloseWallWithWinDoor(Formula, node_container);
			break;
		case E_CloseWallWithNothing:
			CalculateCloseWallNothing(Formula, node_container);
			break;
		case E_CloseCorner:
			CalculateCloseWallCorner(Formula, node_container);
			break;
		case E_IntervalWall:
			CalculateIntervalWall(Formula, node_container);
			break;
		case	E_IntervalWallWithWin:
			CalculateIntervalWithWin(Formula, node_container);
			break;
		case	E_IntervalWallWithDoor:
			CalculateIntervalWithDoor(Formula, node_container);
			break;
		case	E_IntervalWallWithWinDoor:
			CalculateIntervalWithDoorWin(Formula, node_container);
			break;
		case	E_IntervalWallWithNothing:
			CalculateIntervalWithNothing(Formula, node_container);
			break;
		case E_WallParallelWithDoor:
			CalculateWallParallelWithDoor(Formula, node_container);
			break;
		case E_WallSectionOnLeftOfDoor:
			CalculateWallSectionOnLeftOfDoor(Formula, node_container);
			break;
		case	E_WallSectionOnRightOfDoor:
			CalculateWallSectionOnRightOfDoor(Formula, node_container);
			break;
		case	E_WallOnOrientationOfDoor:
			CalculateWallOnOrientationOfDoor(Formula, node_container);
			break;
		case	E_WallOnReverseOfDoor:
			CalculateWallOnReverseOfDoor(Formula, node_container);
			break;
		case	E_SectorRegionWithDoor:
			CalculateSectorRegionWithDoor(Formula, node_container);
			break;
		case	E_SectorRegionOnOrientationOfDoor:
			CalculateSectorRegionOnOrientationOfDoor(Formula, node_container);
			break;
		case	E_SectorRegionOnReverseOfDoor:
			CalculateSectorRegionOnReverseOfDoor(Formula, node_container);
			break;
		case	E_WallParallelWithWin:
			CalculateWallParallelWithWin(Formula, node_container);
			break;
		case	E_WallSectionOnLeftOfWin:
			CalculateWallSectionOnLeftOfWin(Formula, node_container);
			break;
		case	E_WallSectionOnRightOfWin:
			CalculateWallSectionOnRightOfWin(Formula, node_container);
			break;
		case	E_WallOnLeftOfWin:
			CalculateWallOnLeftOfWin(Formula, node_container);
			break;
		case	E_WallOnRightOfWin:
			CalculateWallOnRightOfWin(Formula, node_container);
			break;
		case	E_SectorRegionWithWin:
			CalculateSectorRegionWithWin(Formula, node_container);
			break;
		case	E_CornerWithNearestDistToDoor:
			CalculateCornerWithNearestDistToDoor(Formula, node_container);
			break;
		case	E_CornerWithFarestDistToDoor:
			CalculateCornerWithFarestDistToDoor(Formula, node_container);
			break;
		case E_ClosePositiveCorner:
			CalculateClosePositiveCorner(Formula, node_container);
			break;
		case E_CloseNegativeCorner:
			CalculateCloseNegativeCorner(Formula, node_container);
			break;
		case E_CloseActualWall:
			CalculateCloseActualWall(Formula, node_container);
				break;
		case E_ToiletCircleRegion:
			CalculateToiletRegion(Formula, node_container);
			break;
		case E_CloseVirtualWall:
			CalculateCloseVirtualWall(Formula, node_container);
			break;
		case E_CentreRadiation:
			CalculateCentreRadiation(Formula, node_container);
			break;
		case E_CloseEntranceDoor:
			CalculateCloseEntranceDoor(Formula, node_container);
			break;
		case E_DivergeSunshineDoor:
			CalculateDivergeSunshineDoor(Formula, node_container);
			break;
		case	E_DivergeNormalDoor:
			CalculateDivergeNormalDoor(Formula, node_container);
			break;
		case E_CloseWallOnLeftOfLightEntrance:
			CalculateCloseWallOnLeftOfLightEntrance(Formula, node_container);
			break;
		case E_CloseWallOnRightOfLightEntrance:
			CalculateCloseWallOnRightOfLightEntrance(Formula, node_container);
			break;
		case E_CloseWallWithLightEntrance:
			CalculateCloseWallWithLightEntrance(Formula, node_container);
			break;
		case E_WeightStripBetweenMaxWallWithCentre:
			CalculateWeightStripBetweenMaxWallWithCentre(Formula, node_container);
			break;
		case	E_FOrientationWall:
			FCalculateOrientationWall(single_case, Formula, node_container);
			break;
		case	E_FLeftWall:
			FCalculateLeftWall(single_case, Formula, node_container);
			break;
		case	E_FRightWall:
			FCalculateRightWall(single_case, Formula, node_container);
			break;
		case	E_FReverseWall:
			FCalculateReverseWall(single_case, Formula, node_container);
			break;
		case	E_FOrientationWallSection:
			FCalculateOrientationWallSection(single_case, Formula, node_container);
			break;
		case	E_FLeftWallSection:
			FCalculateLeftWallSection(single_case, Formula, node_container);
			break;
		case	E_FRightWallSection:
			FCalculateRightWallSection(single_case, Formula, node_container);
			break;
		case	E_FReverseWallSection:
			FCalculateReverseWallSection(single_case, Formula, node_container);
			break;
		case E_FOrientationWallRadiation:
			FCalculateOrientationWallRadiation(single_case, Formula, node_container);
			break;
		case	E_FOrientationWallSectionOnLeft:
			FCalculateOrientationWallSectionOnLeft(single_case, Formula, node_container);
			break;
		case	E_FOrientationWallSectionOnRight:
			FCalculateOrientationWallSectionOnRight(single_case, Formula, node_container);
			break;
		case	E_FReverseWallSectionOnLeft:
			FCalculateReverseWallSectionOnLeft(single_case, Formula, node_container);
			break;
		case	E_FReverseWallSectionOnRight:
			FCalculateReverseWallSectionOnRight(single_case, Formula, node_container);
			break;
		case	E_FOrientationLineRegion:
			FCalculateOrientataionLineRegion(single_case, Formula, node_container);
			break;
		case	E_FLeftLineRegion:
			FCalculateLeftLineRegion(single_case, Formula, node_container);
			break;
		case	E_FRightLineRegion:
			FCalculateRightLineRegion(single_case, Formula, node_container);
			break;
		case	E_FReverseLineRegion:
			FCalculateReverseLineRegion(single_case, Formula, node_container);
			break;
		case	E_FOrientationSectorRegion:
			FCalculateOrientataionSectorRegion(single_case, Formula, node_container);
			break;
		case	E_FLeftSectorRegion:
			FCalculateLeftSectorRegion(single_case, Formula, node_container);
			break;
		case	E_FRightSectorRegion:
			FCalculateRightSectorRegion(single_case, Formula, node_container);
			break;
		case	E_FReverseSectorRegion:
			FCalculateReverseSectorRegion(single_case, Formula, node_container);
			break;
		default:
			break;
		}
		auto iter = single_case.find(Formula.id);
		if (iter != single_case.end())
		{
			for (auto row = 0; row < p_region_resolver->max_row; ++row) {
				for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
					auto &container_node = node_container[row][rank];
					auto Point3 = Point3f(container_node->center_point.x, container_node->center_point.y, 0);
					if (container_node->bInside)
						if (ComMath::PointInPolygon(Point3, iter->second.furniture_vertex_points))
							container_node->Weight = 0.f;
				}
			}
		}
	}
	return true;
}

bool MetisFormulaContainer::ConstructWeightRegion(FormulaContainer &node_container)
{
	for (auto Formula : m_furniture_formula_array)
	{
			switch (Formula.Type)
			{
			case E_CloseWall:
				CalculateCloseWall(Formula, node_container);
				break;
			case E_CloseWin:
				CalculateCloseWin(Formula, node_container);
				break;
			case E_CloseDoor:
				CalculateCloseDoor(Formula, node_container);
				break;
			case E_DivergeDoor:
				CalculateDivergeDoor(Formula, node_container);
				break;
			case E_DivergeWin:
				CalculateDivergeWin(Formula, node_container);
				break;
			case E_CloseWallWithWin:
				CalculateCloseWallWithWin(Formula, node_container);
				break;
			case E_CloseWallWithDoor:
				CalculateCloseWallWithDoor(Formula, node_container);
				break;
			case E_CloseWallWithWinDoor:
				CalculateCloseWallWithWinDoor(Formula, node_container);
				break;
			case E_CloseWallWithNothing:
				CalculateCloseWallNothing(Formula, node_container);
				break;
			case E_CloseCorner:
				CalculateCloseWallCorner(Formula, node_container);
				break;
			case E_IntervalWall:
				CalculateIntervalWall(Formula, node_container);
				break;
			case	E_IntervalWallWithWin:
				CalculateIntervalWithWin(Formula, node_container);
				break;
			case	E_IntervalWallWithDoor:
				CalculateIntervalWithDoor(Formula, node_container);
				break;
			case	E_IntervalWallWithWinDoor:
				CalculateIntervalWithDoorWin(Formula, node_container);
				break;
			case	E_IntervalWallWithNothing:
				CalculateIntervalWithNothing(Formula, node_container);
				break;
			case E_WallParallelWithDoor:
				CalculateWallParallelWithDoor(Formula, node_container);
				break;
			case E_WallSectionOnLeftOfDoor:
				CalculateWallSectionOnLeftOfDoor(Formula, node_container);
				break;
			case	E_WallSectionOnRightOfDoor:
				CalculateWallSectionOnRightOfDoor(Formula, node_container);
				break;
			case	E_WallOnOrientationOfDoor:
				CalculateWallOnOrientationOfDoor(Formula, node_container);
				break;
			case	E_WallOnReverseOfDoor:
				CalculateWallOnReverseOfDoor(Formula, node_container);
				break;
			case	E_SectorRegionWithDoor:
				CalculateSectorRegionWithDoor(Formula, node_container);
				break;
			case	E_SectorRegionOnOrientationOfDoor:
				CalculateSectorRegionOnOrientationOfDoor(Formula, node_container);
				break;
			case	E_SectorRegionOnReverseOfDoor:
				CalculateSectorRegionOnReverseOfDoor(Formula, node_container);
				break;
			case	E_WallParallelWithWin:
				CalculateWallParallelWithWin(Formula, node_container);
				break;
			case	E_WallSectionOnLeftOfWin:
				CalculateWallSectionOnLeftOfWin(Formula, node_container);
				break;
			case	E_WallSectionOnRightOfWin:
				CalculateWallSectionOnRightOfWin(Formula, node_container);
				break;
			case	E_WallOnLeftOfWin:
				CalculateWallOnLeftOfWin(Formula, node_container);
				break;
			case	E_WallOnRightOfWin:
				CalculateWallOnRightOfWin(Formula, node_container);
				break;
			case	E_SectorRegionWithWin:
				CalculateSectorRegionWithWin(Formula, node_container);
				break;
			case	E_CornerWithNearestDistToDoor:
				CalculateCornerWithNearestDistToDoor(Formula, node_container);
				break;
			case	E_CornerWithFarestDistToDoor:
				CalculateCornerWithFarestDistToDoor(Formula, node_container);
				break;
			case E_ClosePositiveCorner:
				CalculateClosePositiveCorner(Formula, node_container);
				break;
			case E_CloseNegativeCorner:
				CalculateCloseNegativeCorner(Formula, node_container);
				break;
			case E_CloseActualWall:
				CalculateCloseActualWall(Formula, node_container);
				break;
			case	E_ToiletCircleRegion:
				CalculateToiletRegion(Formula, node_container);
				break;
			case E_CloseVirtualWall:
				CalculateCloseVirtualWall(Formula, node_container);
				break;
			case E_CentreRadiation:
				CalculateCentreRadiation(Formula, node_container);
				break;
			case E_CloseEntranceDoor:
				CalculateCloseEntranceDoor(Formula, node_container);
				break;
			case E_DivergeSunshineDoor:
				CalculateDivergeSunshineDoor(Formula, node_container);
				break;
			case	E_DivergeNormalDoor:
				CalculateDivergeNormalDoor(Formula, node_container);
				break;
			case E_CloseWallOnLeftOfLightEntrance:
				CalculateCloseWallOnLeftOfLightEntrance(Formula, node_container);
				break;
			case E_CloseWallOnRightOfLightEntrance:
				CalculateCloseWallOnRightOfLightEntrance(Formula, node_container);
				break;
			case E_CloseWallWithLightEntrance:
				CalculateCloseWallWithLightEntrance(Formula, node_container);
				break;
			case E_WeightStripBetweenMaxWallWithCentre:
				CalculateWeightStripBetweenMaxWallWithCentre(Formula, node_container);
				break;
			default:
				break;
			} 
	}	 
	return true;
}

void MetisFormulaContainer::CalculateCloseWall(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
    p_region_resolver->ClearLeafWeight();
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 10.f);
			for (auto& Wall : p_single_case->wall_list)
			{
				TestPoint.z = Wall.start_corner.point.z;
				//获取投影点
				auto projection = ComMath::getPointToLinePedal(TestPoint, Wall.start_corner.point, Wall.end_corner.point);
				//没有落在墙体范围内直接返回
				if (!ComMath::isPointOnLine(projection, Wall.start_corner.point, Wall.end_corner.point))
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, Wall.start_corner.point, Wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateCloseWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 10.f);
			for (auto &wall : p_single_case->wall_list)
			{
				for (auto& pWin : wall.window_list)
				{
					TestPoint.z = pWin.start_point.z;
					//获取投影点
					auto projection = ComMath::getPointToLinePedal(TestPoint, pWin.start_point, pWin.end_point);
					//没有落在窗户范围内直接跳过
					if (!ComMath::isPointOnLine(projection, pWin.start_point, pWin.end_point))
						continue;
					float Dist = ComMath::getPointToLineDis(TestPoint, pWin.start_point, pWin.end_point);
					float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
					if (LinearR > nodeRef->Weight)
						nodeRef->Weight = LinearR;
				}
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateCloseDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 10.f);
			for (auto &wall : p_single_case->wall_list)
			{
				for (auto &door : wall.door_list)
				{
					TestPoint.z = door.start_point.z;
					//获取投影点
					auto projection = ComMath::getPointToLinePedal(TestPoint, door.start_point, door.end_point);
					//没有落在窗户范围内直接跳过
					if (!ComMath::isPointOnLine(projection, door.start_point, door.end_point))
						continue;
					float Dist = ComMath::getPointToLineDis(TestPoint, door.start_point, door.end_point);
					float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
					if (LinearR > nodeRef->Weight)
						nodeRef->Weight = LinearR;
				}
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateDivergeDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeightToValue(formulaRef.Ratio);
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 10.f);
			for (auto &wall : p_single_case->wall_list)
			{
				for (auto &door : wall.door_list)
				{
					TestPoint.z = door.start_point.z;
					//获取投影点
					auto projection = ComMath::getPointToLinePedal(TestPoint, door.start_point, door.end_point);
					//落在门范围内直接跳过
					if (ComMath::isPointOnLine(projection, door.start_point, door.end_point))
						nodeRef->Weight = 0.f;
					auto V1 = ComMath::getTwoPointDistance(door.start_point, projection);
					auto V2 = ComMath::getTwoPointDistance(door.end_point, projection);
					if (V1 <= formulaRef.Dist || V2 <= formulaRef.Dist)
						nodeRef->Weight = 0.f;
				}
			}
		}
	}

 
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateDivergeWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeightToValue(formulaRef.Ratio);
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			for (auto &wall : p_single_case->wall_list)
			{
				for (auto &win : wall.window_list)
				{
					auto projection = ComMath::getPointToLinePedal(TestPoint, win.start_point, win.end_point);
					//落在门范围内直接跳过
					if (ComMath::isPointOnLine(projection, win.start_point, win.end_point))
						nodeRef->Weight = 0.f;
					auto V1 = ComMath::getTwoPointDistance(win.start_point, projection);
					auto V2 = ComMath::getTwoPointDistance(win.end_point, projection);
					if (V1 <= formulaRef.Dist || V2 <= formulaRef.Dist)
						nodeRef->Weight = 0.f;
				}
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateCloseWallWithWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			for (auto& Wall : p_single_case->wall_list)
			{
				if (Wall.window_list.size() == 0)
					continue;
				TestPoint.z = Wall.start_corner.point.z;
				//获取投影点
				auto projection = ComMath::getPointToLinePedal(TestPoint, Wall.start_corner.point, Wall.end_corner.point);
				//没落在范围内直接跳过
				if (!ComMath::isPointOnLine(projection, Wall.start_corner.point, Wall.end_corner.point))
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, Wall.start_corner.point, Wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateCloseWallWithDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			for (auto& Wall : p_single_case->wall_list)
			{
				if (Wall.door_list.size() == 0)
					continue;
				TestPoint.z = Wall.start_corner.point.z;
				//获取投影点
				auto projection = ComMath::getPointToLinePedal(TestPoint, Wall.start_corner.point, Wall.end_corner.point);
				//没落在范围内直接跳过
				if (!ComMath::isPointOnLine(projection, Wall.start_corner.point, Wall.end_corner.point))
					continue;
				Point3f V0 = Point3f(Wall.start_corner.point.x, Wall.start_corner.point.y, 0.f);
				Point3f V1 = Point3f(Wall.end_corner.point.x, Wall.end_corner.point.y, 0.f);
				float Dist = ComMath::getPointToLineDis(TestPoint, V0, V1);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateCloseWallWithWinDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			for (auto& Wall : p_single_case->wall_list)
			{
				if (!Wall.door_list.size() || !Wall.window_list.size())
					continue;
				TestPoint.z = Wall.start_corner.point.z;
				//获取投影点
				auto projection = ComMath::getPointToLinePedal(TestPoint, Wall.start_corner.point, Wall.end_corner.point);
				//没落在范围内直接跳过
				if (!ComMath::isPointOnLine(projection, Wall.start_corner.point, Wall.end_corner.point))
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, Wall.start_corner.point, Wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateCloseWallNothing(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			for (auto& Wall : p_single_case->wall_list)
			{
				if (Wall.door_list.size() != 0 || Wall.window_list.size() != 0)
					continue;
				//获取投影点
				auto projection = ComMath::getPointToLinePedal(TestPoint, Wall.start_corner.point, Wall.end_corner.point);
				//没落在范围内直接跳过
				if (!ComMath::isPointOnLine(projection, Wall.start_corner.point, Wall.end_corner.point))
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, Wall.start_corner.point, Wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateIntervalWall(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			bool out = false;
			for (auto& Wall : p_single_case->wall_list)
			{
				float Dist = ComMath::getPointToLineDis(TestPoint, Wall.start_corner.point, Wall.end_corner.point);
				if (Dist < formulaRef.Dist || Dist > formulaRef.Dist + 200)
					out = true;
			}
			if (!out)
				nodeRef->Weight = 1.f*formulaRef.Ratio;
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::CalculateIntervalWithWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			bool out = false;
			for (auto& Wall : p_single_case->wall_list)
			{
				if (!Wall.window_list.size())
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, Wall.start_corner.point, Wall.end_corner.point);
				if (Dist < formulaRef.Dist || Dist > formulaRef.Dist + 250)
					out = true;
			}
			if (!out)
				nodeRef->Weight = 1.f*formulaRef.Ratio;
		}
	}
	FillNodeArray(formulaRef,container);
}
 
void MetisFormulaContainer::CalculateIntervalWithDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			bool out = false;
			for (auto& Wall : p_single_case->wall_list)
			{
				if (!Wall.door_list.size())
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, Wall.start_corner.point, Wall.end_corner.point);
				if (Dist < formulaRef.Dist || Dist > formulaRef.Dist + 250)
					out = true;
			}
			if (!out)
				nodeRef->Weight = 1.f*formulaRef.Ratio;
		}
	}
	FillNodeArray(formulaRef,container);
}
 
void MetisFormulaContainer::CalculateIntervalWithDoorWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			bool out = false;
			for (auto& Wall : p_single_case->wall_list)
			{
				if (!Wall.door_list.size() || !Wall.window_list.size())
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, Wall.start_corner.point, Wall.end_corner.point);
				if (Dist < formulaRef.Dist || Dist > formulaRef.Dist + 250)
					out = true;
			}
			if (!out)
				nodeRef->Weight = 1.f*formulaRef.Ratio;
		}
	}
	FillNodeArray(formulaRef,container);
}
 
void MetisFormulaContainer::CalculateIntervalWithNothing(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			bool out = false;
			for (auto& Wall : p_single_case->wall_list)
			{
				if (Wall.door_list.size() || Wall.window_list.size())
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, Wall.start_corner.point, Wall.end_corner.point);
				if (Dist < formulaRef.Dist || Dist > formulaRef.Dist + 250)
					out = true;
			}
			if (!out)
				nodeRef->Weight = 1.f*formulaRef.Ratio;
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateCloseWallCorner(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			auto &corner_list = p_single_case->corner_list;
			for (auto corner:  corner_list)
			{
				auto Dist = ComMath::getTwoPointDistance(corner.point, TestPoint);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateWallParallelWithDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	//确定墙体
	for (auto s_wall : p_single_case->wall_list) {
		for (auto door : s_wall.door_list)
		{
			Point3f Line1 = door.end_point - door.start_point;
			for (auto wall : p_single_case->wall_list)
			{
				Point3f Line2 = wall.end_corner.point - wall.start_corner.point;
				float angle = ComMath::GetAngleBetweenTwoLines(Line1, Line2);
				//45度到135度 （垂直）只处理平行
				if (0.7853f < angle&&angle < 2.3559f)
					continue;
				//检查门的投影是不是落在墙体
				auto projection1 = ComMath::getPointToLinePedal(door.start_point, wall.start_corner.point, wall.end_corner.point);
				auto projection2 = ComMath::getPointToLinePedal(door.end_point, wall.start_corner.point, wall.end_corner.point);
				auto b1 = ComMath::isPointOnLine(projection1, wall.start_corner.point, wall.end_corner.point);
				auto b2 = ComMath::isPointOnLine(projection2, wall.start_corner.point, wall.end_corner.point);
				if (!b1 && !b2)
					continue;
				b1 = ComMath::isPointOnLine(door.start_point, wall.start_corner.point, wall.end_corner.point);
				b2 = ComMath::isPointOnLine(door.end_point, wall.start_corner.point, wall.end_corner.point);
				//门依附于墙跳过
				if (b1 || b2)
					continue;
                auto distToWin = ComMath::getPointToLineDis(wall.start_corner.point, door.start_point, door.end_point);
				for (auto row = 0; row < p_region_resolver->max_row; ++row) {
					for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
						auto &nodeRef = p_region_resolver->base_nodes[row][rank];
						if (!nodeRef->bInside)
							continue;
						Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
						//测试点落在墙体范围内
						auto projection1 = ComMath::getPointToLinePedal(TestPoint, wall.start_corner.point, wall.end_corner.point);
						auto b1 = ComMath::isPointOnLine(projection1, wall.start_corner.point, wall.end_corner.point);
						if (!b1)
							continue;
						//在另一侧跳过!
						if (ComMath::getPointToLineDis(TestPoint, door.start_point, door.end_point) > distToWin)
							continue;
						float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
						float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
						if (LinearR > nodeRef->Weight)
							nodeRef->Weight = LinearR;
					}
				}
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateWallParallelWithWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	//确定墙体
	for (auto s_wall : p_single_case->wall_list) {
		for (auto win : s_wall.window_list)
		{
			Point3f Line1 = win.end_point - win.start_point;
			for (auto wall : p_single_case->wall_list)
			{
				Point3f Line2 = wall.end_corner.point - wall.start_corner.point;
				float angle = ComMath::GetAngleBetweenTwoLines(Line1, Line2);
				//45度到135度 （垂直）只处理平行
				if (0.7853f < angle&&angle < 2.3559f)
					continue;
				//检查门的投影是不是落在墙体
				auto projection1 = ComMath::getPointToLinePedal(win.start_point, wall.start_corner.point, wall.end_corner.point);
				auto projection2 = ComMath::getPointToLinePedal(win.end_point, wall.start_corner.point, wall.end_corner.point);
				auto b1 = ComMath::isPointOnLine(projection1, wall.start_corner.point, wall.end_corner.point);
				auto b2 = ComMath::isPointOnLine(projection2, wall.start_corner.point, wall.end_corner.point);
				if (!b1 && !b2)
					continue;
				b1 = ComMath::isPointOnLine(win.start_point, wall.start_corner.point, wall.end_corner.point);
				b2 = ComMath::isPointOnLine(win.end_point, wall.start_corner.point, wall.end_corner.point);
				//门依附于墙跳过
				if (b1 || b2)
					continue;
                auto distToWin = ComMath::getPointToLineDis(wall.start_corner.point, win.start_point, win.end_point);
				for (auto row = 0; row < p_region_resolver->max_row; ++row) {
					for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
						auto &nodeRef = p_region_resolver->base_nodes[row][rank];
						if (!nodeRef->bInside)
							continue;
						Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
						//测试点落在墙体范围内
						auto projection1 = ComMath::getPointToLinePedal(TestPoint, wall.start_corner.point, wall.end_corner.point);
						auto b1 = ComMath::isPointOnLine(projection1, wall.start_corner.point, wall.end_corner.point);
						if (!b1)
							continue;
						//在另一侧跳过!
						if (ComMath::getPointToLineDis(TestPoint, win.start_point, win.end_point) > distToWin)
							continue;
						//获取
						float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
						float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
						if (LinearR > nodeRef->Weight)
							nodeRef->Weight = LinearR;
					}
				}
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateWallSectionOnLeftOfDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto wall : p_single_case->wall_list)
	{
		if (!wall.door_list.size())
			continue;
		for (auto row = 0; row < p_region_resolver->max_row; ++row) {
			for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
				auto &nodeRef = p_region_resolver->base_nodes[row][rank];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, wall.start_corner.point.z);
				//检查投影区域
				auto projection1 = ComMath::getPointToLinePedal(TestPoint, wall.start_corner.point, wall.end_corner.point);
				auto bproj = ComMath::isPointBetweenPoint(projection1, wall.start_corner.point, wall.end_corner.point);
				if (!bproj)
					continue;
				//检查投影到门区域，舍去此离散点
				bproj = false;
				for (auto door : wall.door_list) {
					if (ComMath::isPointBetweenPoint(projection1, door.start_point, door.end_point))
						bproj = true;
				}
				if (bproj)
					continue;
				//计算与门叉乘方向,确定在左边还是右边
				bproj = false;
				for (auto door : wall.door_list) {
					auto point = TestPoint - door.start_point;
					point = ComMath::Cross(point, door.direction);
					//左手坐标系，V = -cross（V）
					if (point.z < 0)
						continue;
					float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
					float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
					if (LinearR > nodeRef->Weight)
						nodeRef->Weight = LinearR;
				}
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateWallSectionOnRightOfDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto wall : p_single_case->wall_list)
	{
		if (!wall.door_list.size())
			continue;
		for (auto row = 0; row < p_region_resolver->max_row; ++row) {
			for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
				auto &nodeRef = p_region_resolver->base_nodes[row][rank];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				//检查投影区域
				auto projection1 = ComMath::getPointToLinePedal(TestPoint, wall.start_corner.point, wall.end_corner.point);
				auto bproj = ComMath::isPointBetweenPoint(projection1, wall.start_corner.point, wall.end_corner.point);
				if (!bproj)
					continue;
				//检查投影到门区域，舍去此离散点
				bproj = false;
				for (auto door : wall.door_list) {
					if (ComMath::isPointBetweenPoint(projection1, door.start_point, door.end_point))
						bproj = true;
				}
				if (bproj)
					continue;
				//计算与门叉乘方向,确定在左边还是右边
				bproj = false;
				for (auto door : wall.door_list) {
					auto point = TestPoint - door.start_point;
					point = ComMath::Cross(point, door.direction);
					//左边跳过
					if (point.z > 0)
						continue;
					float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
					float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
					if (LinearR > nodeRef->Weight)
						nodeRef->Weight = LinearR;
				}
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateWallSectionOnLeftOfWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto wall : p_single_case->wall_list)
	{
		if (!wall.window_list.size())
			continue;
		for (auto row = 0; row < p_region_resolver->max_row; ++row) {
			for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
				auto &nodeRef = p_region_resolver->base_nodes[row][rank];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				//检查投影区域
				auto projection1 = ComMath::getPointToLinePedal(TestPoint, wall.start_corner.point, wall.end_corner.point);
				auto bproj = ComMath::isPointBetweenPoint(projection1, wall.start_corner.point, wall.end_corner.point);
				if (!bproj)
					continue;
				//检查投影到门区域，舍去此离散点
				bproj = false;
				for (auto win : wall.window_list) {
					if (ComMath::isPointBetweenPoint(projection1, win.start_point, win.end_point))
						bproj = true;
				}
				if (bproj)
					continue;
				//计算与门叉乘方向,确定在左边还是右边
				bproj = false;
				for (auto win : wall.window_list) {
					auto point = TestPoint - win.start_point;
					point = ComMath::Cross(point, win.direction);
					//右边跳过
					if (point.z < 0)
						continue;
					float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
					float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
					if (LinearR > nodeRef->Weight)
						nodeRef->Weight = LinearR;
				}
			}
		}
	}
	FillNodeArray(formulaRef,container);
}
 
void MetisFormulaContainer::CalculateWallSectionOnRightOfWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto wall : p_single_case->wall_list)
	{
		if (!wall.window_list.size())
			continue;
		for (auto row = 0; row < p_region_resolver->max_row; ++row) {
			for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
				auto &nodeRef = p_region_resolver->base_nodes[row][rank];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				//检查投影区域
				auto projection1 = ComMath::getPointToLinePedal(TestPoint, wall.start_corner.point, wall.end_corner.point);
				auto bproj = ComMath::isPointBetweenPoint(projection1, wall.start_corner.point, wall.end_corner.point);
				if (!bproj)
					continue;
				//检查投影到门区域，舍去此离散点
				bproj = false;
				for (auto win : wall.window_list) {
					if (ComMath::isPointBetweenPoint(projection1, win.start_point, win.end_point))
						bproj = true;
				}
				if (bproj)
					continue;
				//计算与门叉乘方向,确定在左边还是右边
				bproj = false;
				for (auto win : wall.window_list) {
					auto point = TestPoint - win.start_point;
					point = ComMath::Cross(point, win.direction);
					//右边跳过
					if (point.z > 0)
						continue;
					float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
					float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
					if (LinearR > nodeRef->Weight)
						nodeRef->Weight = LinearR;
				}
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateWallOnOrientationOfDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto s_wall : p_single_case->wall_list)
	{
		for (auto door : s_wall.door_list)
		{
			//除去所在墙体
			for (auto wall : p_single_case->wall_list)
			{
				//检查平行 	 
				Point3f Line1 = wall.end_corner.point - wall.start_corner.point;
				Point3f Line2 = door.end_point - door.start_point;
				float angle = ComMath::GetAngleBetweenTwoLines(Line1, Line2);
				//45度到135度 （垂直）
				if (0.7853f > angle || angle > 2.3559f)
					continue;
				auto bproj = ComMath::isPointBetweenPoint(door.start_point, wall.start_corner.point, wall.end_corner.point);
				if (bproj)
					continue;
				auto centre_point = (wall.end_corner.point + wall.start_corner.point) * 0.5f;
				centre_point = ComMath::Cross(centre_point - door.start_point, door.direction);
				if (door.right_open)
					if (centre_point.z > 0)
						continue;
				if (!door.right_open)
					if (centre_point.z < 0)
						continue;
				for (auto row = 0; row < p_region_resolver->max_row; ++row) {
					for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
						auto &nodeRef = p_region_resolver->base_nodes[row][rank];
						if (!nodeRef->bInside)
							continue;
						Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
						//确定投影在墙体范围内
						auto projection1 = ComMath::getPointToLinePedal(TestPoint, wall.start_corner.point, wall.end_corner.point);
						auto bproj = ComMath::isPointBetweenPoint(projection1, wall.start_corner.point, wall.end_corner.point);
						if (!bproj)
							continue;
						float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
						float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
						if (LinearR > nodeRef->Weight)
							nodeRef->Weight = LinearR;
					}
				}
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateWallOnReverseOfDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto s_wall : p_single_case->wall_list)
	{
		for (auto door : s_wall.door_list)
		{
			//除去所在墙体
			for (auto wall : p_single_case->wall_list)
			{
				//检查平行 	 
				Point3f Line1 = wall.end_corner.point - wall.start_corner.point;
				Point3f Line2 = door.end_point - door.start_point;
				float angle = ComMath::GetAngleBetweenTwoLines(Line1, Line2);
				//45度到135度 （垂直）
				if (0.7853f > angle || angle > 2.3559f)
					continue;
				auto bproj = ComMath::isPointBetweenPoint(door.start_point, wall.start_corner.point, wall.end_corner.point);
				if (bproj)
					continue;
				auto centre_point = (wall.end_corner.point + wall.start_corner.point) * 0.5f;
				centre_point = ComMath::Cross(centre_point - door.start_point, door.direction);
				if (door.right_open)
					if (centre_point.z < 0)
						continue;
				if (!door.right_open)
					if (centre_point.z > 0)
						continue;
				for (auto row = 0; row < p_region_resolver->max_row; ++row) {
					for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
						auto &nodeRef = p_region_resolver->base_nodes[row][rank];
						if (!nodeRef->bInside)
							continue;
						Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
						auto projection1 = ComMath::getPointToLinePedal(TestPoint, wall.start_corner.point, wall.end_corner.point);
						auto bproj = ComMath::isPointBetweenPoint(projection1, wall.start_corner.point, wall.end_corner.point);
						if (!bproj)
							continue;
						float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
						float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
						if (LinearR > nodeRef->Weight)
							nodeRef->Weight = LinearR;
					}
				}
			}
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::CalculateWallOnLeftOfWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto s_wall : p_single_case->wall_list)
	{
		for (auto win : s_wall.window_list)
		{
			//除去所在墙体
			for (auto wall : p_single_case->wall_list)
			{
				//检查平行 	 
				Point3f Line1 = wall.end_corner.point - wall.start_corner.point;
				Point3f Line2 = win.end_point - win.start_point;
				float angle = ComMath::GetAngleBetweenTwoLines(Line1, Line2);
				//45度到135度 （垂直）
				if (0.7853f > angle || angle > 2.3559f)
					continue;
				auto bproj = ComMath::isPointBetweenPoint(win.start_point, wall.start_corner.point, wall.end_corner.point);
				if (bproj)
					continue;
				auto centre_point = (win.start_point + win.end_point) * 0.5f;
				centre_point = ComMath::Cross(wall.start_corner.point - centre_point, win.direction);
				if (centre_point.z < 0)
					continue;
				for (auto row = 0; row < p_region_resolver->max_row; ++row) {
					for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
						auto &nodeRef = p_region_resolver->base_nodes[row][rank];
						if (!nodeRef->bInside)
							continue;
						Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
						//投影在墙体范围内
						auto projection1 = ComMath::getPointToLinePedal(TestPoint, wall.start_corner.point, wall.end_corner.point);
						auto bproj = ComMath::isPointBetweenPoint(projection1, wall.start_corner.point, wall.end_corner.point);
						if (!bproj)
							continue;
						float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
						float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
						if (LinearR > nodeRef->Weight)
							nodeRef->Weight = LinearR;
					}
				}
			}
		}
	}
	FillNodeArray(formulaRef,container);
}
 
void MetisFormulaContainer::CalculateWallOnRightOfWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto s_wall : p_single_case->wall_list)
	{
		for (auto win : s_wall.window_list)
		{
			//除去所在墙体
			for (auto wall : p_single_case->wall_list)
			{
				//检查平行 	 
				Point3f Line1 = wall.end_corner.point - wall.start_corner.point;
				Point3f Line2 = win.end_point - win.start_point;
				float angle = ComMath::GetAngleBetweenTwoLines(Line1, Line2);
				//45度到135度 （垂直）
				if (0.7853f > angle || angle > 2.3559f)
					continue;
				auto bproj = ComMath::isPointBetweenPoint(win.start_point, wall.start_corner.point, wall.end_corner.point);
				if (bproj)
					continue;
				auto centre_point = (win.start_point + win.end_point) * 0.5f;
				centre_point = ComMath::Cross(wall.start_corner.point - centre_point, win.direction);
				if (centre_point.z > 0)
					continue;
				for (auto row = 0; row < p_region_resolver->max_row; ++row) {
					for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
						auto &nodeRef = p_region_resolver->base_nodes[row][rank];
						if (!nodeRef->bInside)
							continue;
						Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
						auto projection1 = ComMath::getPointToLinePedal(TestPoint, wall.start_corner.point, wall.end_corner.point);
						auto bproj = ComMath::isPointBetweenPoint(projection1, wall.start_corner.point, wall.end_corner.point);
						if (!bproj)
							continue;
						float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
						float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
						if (LinearR > nodeRef->Weight)
							nodeRef->Weight = LinearR;
					}
				}
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateSectorRegionWithDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto s_wall : p_single_case->wall_list)
	{
		for (auto door : s_wall.door_list)
		{
			for (auto row = 0; row < p_region_resolver->max_row; ++row) {
				for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
					auto &nodeRef = p_region_resolver->base_nodes[row][rank];
					if (!nodeRef->bInside)
						continue;
					Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
					TestPoint.z = door.start_point.z;
					//获取投影点
					auto projection = ComMath::getPointToLinePedal(TestPoint, door.start_point, door.end_point);
					//没有落在窗户范围内直接跳过
					auto bproj = ComMath::isPointOnLine(projection, door.start_point, door.end_point);
					if (bproj)
					{
						float Dist = ComMath::getPointToLineDis(TestPoint, door.start_point, door.end_point);
						if (Dist < formulaRef.Dist) {
							float  LinearR = 1.f*formulaRef.Ratio;
							if (LinearR > nodeRef->Weight)
								nodeRef->Weight = LinearR;
						}
					}
					else
					{
						//获取centre点
						auto centre = (door.end_point + door.start_point)*0.5f;
						auto line1 = door.start_point - centre;
						auto line2 = door.end_point - centre;
						line1 = ComMath::Cross(line1, door.direction);
						line2 = ComMath::Cross(line2, door.direction);
						auto line3 = TestPoint - centre;
						line3 = ComMath::Cross(line3, door.direction);
						Point3f Povit;
						if (line3.z*line1.z > 0)
							Povit = door.start_point;
						else if (line3.z*line2.z > 0)
							Povit = door.end_point;
						float Dist = ComMath::getTwoPointDistance(TestPoint, Povit);
						if (Dist < formulaRef.Dist) {
							float  LinearR = 1.f*formulaRef.Ratio;
							if (LinearR > nodeRef->Weight)
								nodeRef->Weight = LinearR;
						}
					}
				}
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateSectorRegionWithWin(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto s_wall : p_single_case->wall_list)
	{
		for (auto win : s_wall.window_list) {
			for (auto row = 0; row < p_region_resolver->max_row; ++row) {
				for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
					auto &nodeRef = p_region_resolver->base_nodes[row][rank];
					if (!nodeRef->bInside)
						continue;
					Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
					TestPoint.z = win.start_point.z;
					//获取投影点
					auto projection = ComMath::getPointToLinePedal(TestPoint, win.start_point, win.end_point);
					//没有落在窗户范围内直接跳过
					auto bproj = ComMath::isPointOnLine(projection, win.start_point, win.end_point);
					if (bproj)
					{
						float Dist = ComMath::getPointToLineDis(TestPoint, win.start_point, win.end_point);
						if (Dist < formulaRef.Dist) {
							float  LinearR = 1.f*formulaRef.Ratio;
							if (LinearR > nodeRef->Weight)
								nodeRef->Weight = LinearR;
						}
					}
					else
					{
						//获取centre点
						auto centre = (win.end_point + win.start_point)*0.5f;
						auto line1 = win.start_point - centre;
						auto line2 = win.end_point - centre;
						line1 = ComMath::Cross(line1, win.direction);
						line2 = ComMath::Cross(line2, win.direction);
						auto line3 = TestPoint - centre;
						line3 = ComMath::Cross(line3, win.direction);
						Point3f Povit;
						if (line3.z*line1.z > 0)
							Povit = win.start_point;
						else if (line3.z*line2.z > 0)
							Povit = win.end_point;
						float Dist = ComMath::getTwoPointDistance(TestPoint, Povit);
						if (Dist < formulaRef.Dist) {
							float  LinearR = 1.f*formulaRef.Ratio;
							if (LinearR > nodeRef->Weight)
								nodeRef->Weight = LinearR;
						}
					}
				}
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateSectorRegionOnOrientationOfDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto s_wall : p_single_case->wall_list) {
		for (auto door : s_wall.door_list)
		{
			for (auto row = 0; row < p_region_resolver->max_row; ++row) {
				for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
					auto &nodeRef = p_region_resolver->base_nodes[row][rank];
					if (!nodeRef->bInside)
						continue;
					Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
					TestPoint.z = door.start_point.z;
					//获取投影点
					auto projection = ComMath::getPointToLinePedal(TestPoint, door.start_point, door.end_point);
					//没有落在窗户范围内直接跳过
					auto bproj = ComMath::isPointOnLine(projection, door.start_point, door.end_point);
					if (bproj)
						continue;
					//获取centre点
					auto centre = (door.end_point + door.start_point)*0.5f;
					auto line1 = door.start_point - centre;
					auto line2 = door.end_point - centre;
					line1 = ComMath::Cross(line1, door.direction);
					line2 = ComMath::Cross(line2, door.direction);
					auto line3 = TestPoint - centre;
					line3 = ComMath::Cross(line3, door.direction);
					Point3f Povit;
					if (!door.right_open)
					{
						if (line3.z < 0)
							continue;
						if (line3.z*line1.z > 0)
							Povit = door.start_point;
						else if (line3.z*line2.z > 0)
							Povit = door.end_point;
					}
					else
					{
						if (line3.z > 0)
							continue;
						if (line3.z*line1.z > 0)
							Povit = door.start_point;
						else if (line3.z*line2.z > 0)
							Povit = door.end_point;
					}
					float Dist = ComMath::getTwoPointDistance(TestPoint, Povit);
					if (Dist < formulaRef.Dist) {
						float  LinearR = 1.f*formulaRef.Ratio;
						if (LinearR > nodeRef->Weight)
							nodeRef->Weight = LinearR;
					}
				}
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateSectorRegionOnReverseOfDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto s_wall : p_single_case->wall_list) {
		for (auto door : s_wall.door_list)
		{
			for (auto row = 0; row < p_region_resolver->max_row; ++row) {
				for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
					auto &nodeRef = p_region_resolver->base_nodes[row][rank];
					if (!nodeRef->bInside)
						continue;
					Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
					TestPoint.z = door.start_point.z;
					//获取投影点
					auto projection = ComMath::getPointToLinePedal(TestPoint, door.start_point, door.end_point);
					//落在窗户范围内直接跳过
					auto bproj = ComMath::isPointOnLine(projection, door.start_point, door.end_point);
					if (bproj)
						continue;
					//获取centre点
					auto centre = (door.end_point + door.start_point)*0.5f;
					auto line1 = door.start_point - centre;
					auto line2 = door.end_point - centre;
					line1 = ComMath::Cross(line1, door.direction);
					line2 = ComMath::Cross(line2, door.direction);
					auto line3 = TestPoint - centre;
					line3 = ComMath::Cross(line3, door.direction);
					Point3f Povit;
					if (!door.right_open)
					{
						if (line3.z > 0)
							continue;
						if (line3.z*line1.z > 0)
							Povit = door.start_point;
						else if (line3.z*line2.z > 0)
							Povit = door.end_point;
					}
					else
					{
						if (line3.z < 0)
							continue;
						if (line3.z*line1.z > 0)
							Povit = door.start_point;
						else if (line3.z*line2.z > 0)
							Povit = door.end_point;
					}
					float Dist = ComMath::getTwoPointDistance(TestPoint, Povit);
					if (Dist < formulaRef.Dist) {
						float  LinearR = 1.f*formulaRef.Ratio;
						if (LinearR > nodeRef->Weight)
							nodeRef->Weight = LinearR;
					}
				}
			}
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateCornerWithNearestDistToDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	auto &cornor_list = p_single_case->corner_list;
	Point3f Point;
	float Nearest = FLT_MAX;
	for (auto s_wall : p_single_case->wall_list)
	{
		for (auto door : s_wall.door_list) {
			for (auto i = 0; i < cornor_list.size(); ++i)
			{
				auto Dist = ComMath::getTwoPointDistance(cornor_list[i].point, (door.start_point + door.end_point)*0.5f);
				if (Dist < Nearest) {
					Point = cornor_list[i].point;
					Nearest = Dist;
				}
			}
		}
	}
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			float Dist = ComMath::getTwoPointDistance(TestPoint, Point);
			float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
			if (LinearR > nodeRef->Weight)
				nodeRef->Weight = LinearR;
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateCornerWithFarestDistToDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	auto &cornor_list = p_single_case->corner_list;
	Point3f Point;
	float Farest = -FLT_MAX;
	for (auto s_wall : p_single_case->wall_list)
	{
		for (auto door : s_wall.door_list) {
			for (auto i = 0; i < cornor_list.size(); ++i)
			{
				auto Dist = ComMath::getTwoPointDistance(cornor_list[i].point, (door.start_point + door.end_point)*0.5f);
				if (Dist > Farest) {
					Point = cornor_list[i].point;
					Farest = Dist;
				}
			}
		}
	}
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			float Dist = ComMath::getTwoPointDistance(TestPoint, Point);
			float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
			if (LinearR > nodeRef->Weight)
				nodeRef->Weight = LinearR;
		}
	}
	FillNodeArray(formulaRef,container);
}

void MetisFormulaContainer::CalculateClosePositiveCorner(const MetisFuinitureFormula&formularRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto &corner : p_single_case->corner_list)
	{
		//solve wall relatived with this corner
		std::vector<Wall> walls;
		for (auto &wall : p_single_case->wall_list)
		{
			if (wall.is_virtual)
				continue;
			if (wall.start_corner.point == corner.point
				|| wall.end_corner.point == corner.point)
			{
				walls.push_back(wall);
			}
		}
		if (walls.size() != 2)
			continue;

		// direction of wall pointing to corner
		Point3f direction;
		if (walls[0].start_corner.point == corner.point)
			direction = ComMath::Normalize(corner.point - walls[0].end_corner.point);

		if (walls[0].end_corner.point == corner.point)
			direction = ComMath::Normalize(corner.point - walls[0].start_corner.point);
		//negative
		if (!ComMath::PointInPolygon(corner.point + direction, p_single_case->single_room.point_list))
			continue;
		for (int i = 0; i < p_region_resolver->max_row; ++i) {
			for (int j = 0; j < p_region_resolver->max_rank; ++j) {
				auto nodeRef = p_region_resolver->base_nodes[i][j];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				float Dist = ComMath::getTwoPointDistance(TestPoint, corner.point);
				float LinearR = (1 - Dist / formularRef.Dist)*formularRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formularRef, container);
}
 
void MetisFormulaContainer::CalculateCloseNegativeCorner(const MetisFuinitureFormula&formularRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto &corner : p_single_case->corner_list)
	{
		//solve wall relatived with this corner
		std::vector<Wall> walls;
		for (auto &wall : p_single_case->wall_list)
		{
			if (wall.is_virtual)
				continue;
			if (wall.start_corner.point == corner.point
				|| wall.end_corner.point == corner.point)
			{
				walls.push_back(wall);
			}
		}
		if (walls.size() != 2)
			continue;

		// direction of wall pointing to corner
		Point3f direction;
		if (walls[0].start_corner.point == corner.point)
			direction = ComMath::Normalize(corner.point - walls[0].end_corner.point);

		if (walls[0].end_corner.point == corner.point)
			direction = ComMath::Normalize(corner.point - walls[0].start_corner.point);
		//positive
		if (ComMath::PointInPolygon(corner.point + direction, p_single_case->single_room.point_list))
			continue;
		for (int i = 0; i < p_region_resolver->max_row; ++i) {
			for (int j = 0; j < p_region_resolver->max_rank; ++j) {
				auto nodeRef = p_region_resolver->base_nodes[i][j];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				float Dist = ComMath::getTwoPointDistance(TestPoint, corner.point);
				float LinearR = (1 - Dist / formularRef.Dist)*formularRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formularRef, container);
}

void MetisFormulaContainer::CalculateCloseActualWall(const MetisFuinitureFormula&formularRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 10.f);
			for (auto& Wall : p_single_case->wall_list)
			{
				if(Wall.is_virtual)
					continue;
				TestPoint.z = Wall.start_corner.point.z;
				//获取投影点
				auto projection = ComMath::getPointToLinePedal(TestPoint, Wall.start_corner.point, Wall.end_corner.point);
				//没有落在墙体范围内直接返回
				if (!ComMath::isPointOnLine(projection, Wall.start_corner.point, Wall.end_corner.point))
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, Wall.start_corner.point, Wall.end_corner.point);
				float LinearR = (1 - Dist / formularRef.Dist)*formularRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formularRef, container);
}

void MetisFormulaContainer::CalculateToiletRegion(const MetisFuinitureFormula&formularRef, FormulaContainer&container) {
	p_region_resolver->ClearLeafWeight();
	for (auto ref:p_single_case->floor_box_list)
	{
		if (ref.floor_box_type != FB_STOOL)
			continue;
		for (int i = 0; i < p_region_resolver->max_row; ++i) {
			for (int j = 0; j < p_region_resolver->max_rank; ++j) {
				auto nodeRef = p_region_resolver->base_nodes[i][j];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				float Dist = ComMath::getTwoPointDistance(TestPoint, ref.center);
				float LinearR = (1 - Dist / formularRef.Dist)*formularRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
		FillNodeArray(formularRef, container);
	}
}

void MetisFormulaContainer::CalculateCloseVirtualWall(const MetisFuinitureFormula&formularRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (int i = 0; i < p_region_resolver->max_row; ++i) {
		for (int j = 0; j < p_region_resolver->max_rank; ++j) {
			auto nodeRef = p_region_resolver->base_nodes[i][j];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 10.f);
			for (auto& wall : p_single_case->wall_list)
			{
				if (!wall.is_virtual)
					continue;
				TestPoint.z = wall.start_corner.point.z;
				//获取投影点
				auto projection = ComMath::getPointToLinePedal(TestPoint, wall.start_corner.point, wall.end_corner.point);
				//没有落在墙体范围内直接返回
				if (!ComMath::isPointOnLine(projection, wall.start_corner.point, wall.end_corner.point))
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
				float LinearR = (1 - Dist / formularRef.Dist)*formularRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formularRef, container);
}

void MetisFormulaContainer::CalculateCloseEntranceDoor(const MetisFuinitureFormula&formularRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	float dist_max = -FLT_MAX;
	float dist_min = FLT_MAX;


	for (Wall&wall : p_single_case->wall_list)
	{
		Point3f centre = (wall.start_corner.point + wall.end_corner.point) * 0.5f;
		auto dist = ComMath::getTwoPointDistance(centre, p_single_case->entrance_door.pos);
		if (dist > dist_max)
			dist_max = dist;
		if (dist < dist_min)
			dist_min = dist;
	}

	for (Wall&wall : p_single_case->wall_list)
	{
		Point3f centre = (wall.start_corner.point + wall.end_corner.point) * 0.5f;
		auto dist = ComMath::getTwoPointDistance(centre, p_single_case->entrance_door.pos);

		float weight = 1 - (dist / dist_max);
		for (int i = 0; i < p_region_resolver->max_row; ++i) {
			for (int j = 0; j < p_region_resolver->max_rank; ++j) {
				auto nodeRef = p_region_resolver->base_nodes[i][j];
				if (!nodeRef->bInside)
					continue;
				if (nodeRef->Weight != 0)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				auto projection = ComMath::getPointToLinePedal(TestPoint, wall.start_corner.point, wall.end_corner.point);
				//没有落在墙体范围内直接返回
				if (!ComMath::isPointOnLine(projection, wall.start_corner.point, wall.end_corner.point))
					continue;
				float dist_to_wall = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
				float LinearR = (1 - dist_to_wall / formularRef.Dist)*formularRef.Ratio*weight;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formularRef, container);
}

void MetisFormulaContainer::CalculateCentreRadiation(const MetisFuinitureFormula&formularRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	std::vector<Point3f>point_list = p_single_case->single_room.getPointList();
	//计算AABB
	Point3f maxf = Point3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	Point3f minf = Point3f(FLT_MAX, FLT_MAX, FLT_MAX);
	for (auto &point : point_list)
	{
		maxf = ComMath::Point3fMax(point, maxf);
		minf = ComMath::Point3fMin(point, minf);
	}

	auto centrePoint = (maxf + minf)*0.5f;
	for (int i = 0; i < p_region_resolver->max_row; ++i) {
		for (int j = 0; j < p_region_resolver->max_rank; ++j) {
			auto nodeRef = p_region_resolver->base_nodes[i][j];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			float dist = ComMath::getTwoPointDistance(TestPoint, centrePoint);
			float LinearR = (1 - dist / formularRef.Dist)*formularRef.Ratio;
			if (LinearR > nodeRef->Weight)
				nodeRef->Weight = LinearR;
		}
	}
	FillNodeArray(formularRef, container);
}

void MetisFormulaContainer::CalculateDivergeSunshineDoor(const MetisFuinitureFormula&formularRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeightToValue(formularRef.Ratio);

	for (int i = 0; i < p_region_resolver->max_row; ++i) {
		for (int j = 0; j < p_region_resolver->max_rank; ++j) {
			auto nodeRef = p_region_resolver->base_nodes[i][j];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			for (auto &wall : p_single_case->wall_list)
			{
				for (auto &door : wall.door_list)
				{
					if (door.door_layout_type != DoorLayoutType::D_SUN_LAYOUT)
						continue;
					TestPoint.z = door.start_point.z;
					//获取投影点
					auto projection = ComMath::getPointToLinePedal(TestPoint, door.start_point, door.end_point);
					//落在门范围内直接跳过
					if (ComMath::isPointOnLine(projection, door.start_point, door.end_point))
						nodeRef->Weight = 0.f;
					auto V1 = ComMath::getTwoPointDistance(door.start_point, projection);
					auto V2 = ComMath::getTwoPointDistance(door.end_point, projection);
					if (V1 <= formularRef.Dist || V2 <= formularRef.Dist)
						nodeRef->Weight = 0.f;
				}
			}
		}
	}
	FillNodeArray(formularRef, container);

}

void MetisFormulaContainer::CalculateDivergeNormalDoor(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{

	p_region_resolver->ClearLeafWeightToValue(formulaRef.Ratio);

	for (int i = 0; i < p_region_resolver->max_row; ++i) {
		for (int j = 0; j < p_region_resolver->max_rank; ++j) {
			auto nodeRef = p_region_resolver->base_nodes[i][j];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			for (auto &wall : p_single_case->wall_list)
			{
				for (auto &door : wall.door_list)
				{
					if (door.door_layout_type == DoorLayoutType::D_SUN_LAYOUT)
						continue;
					TestPoint.z = door.start_point.z;
					//获取投影点
					auto projection = ComMath::getPointToLinePedal(TestPoint, door.start_point, door.end_point);
					//落在门范围内直接跳过
					if (ComMath::isPointOnLine(projection, door.start_point, door.end_point))
						nodeRef->Weight = 0.f;
					auto V1 = ComMath::getTwoPointDistance(door.start_point, projection);
					auto V2 = ComMath::getTwoPointDistance(door.end_point, projection);
					if (V1 <= formulaRef.Dist || V2 <= formulaRef.Dist)
						nodeRef->Weight = 0.f;
				}
			}
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::CalculateCloseWallOnLeftOfLightEntrance(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	//找到入户光口
	Opening *opening = nullptr;
	Wall*which_wall = nullptr;
	for (auto &s_wall : p_single_case->wall_list)
	{
		bool find = false;
		for (auto &windows : s_wall.window_list)
		{
			if (windows.is_light) {
				opening = &windows;
				which_wall = &s_wall;
				find = true;
				break;
			}
		}
		if (find)
			break;
		for (auto &door : s_wall.door_list)
		{
			if (door.is_light) {
				opening = &door;
				which_wall = &s_wall;
				find = true;
				break;
			}
		}
		if (find)
			break;
	}
	//并没有找到相关的入光口,如果有只应该有一个入光口
	if (!opening)
		return;
	auto &open = *opening;
	for (auto &s_wall : p_single_case->wall_list)
	{
		if (&s_wall == which_wall)
			continue;
		Point3f Line1 = s_wall.end_corner.point - s_wall.start_corner.point;
		Point3f Line2 = open.end_point - open.start_point;
		float angle = ComMath::GetAngleBetweenTwoLines(Line1, Line2);
		//45度到135度 （垂直）
		if (0.7853f > angle || angle > 2.3559f)
			continue;
		auto centre_point = (open.start_point + open.end_point) * 0.5f;
		centre_point = ComMath::Cross(s_wall.start_corner.point - centre_point, open.direction);
		if (centre_point.z < 0)
			continue;
		for (int i = 0; i < p_region_resolver->max_row; ++i) {
			for (int j = 0; j < p_region_resolver->max_rank; ++j) {
				auto nodeRef = p_region_resolver->base_nodes[i][j];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				//投影在墙体范围内
				auto projection1 = ComMath::getPointToLinePedal(TestPoint, s_wall.start_corner.point, s_wall.end_corner.point);
				auto bproj = ComMath::isPointBetweenPoint(projection1, s_wall.start_corner.point, s_wall.end_corner.point);
				if (!bproj)
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, s_wall.start_corner.point, s_wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}

	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::CalculateCloseWallOnRightOfLightEntrance(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	//找到入户光口
	Opening *opening = nullptr;
	Wall*which_wall = nullptr;
	for (auto &s_wall : p_single_case->wall_list)
	{
		bool find = false;
		for (auto &windows : s_wall.window_list)
		{
			if (windows.is_light) {
				opening = &windows;
				which_wall = &s_wall;
				find = true;
				break;
			}
		}
		if (find)
			break;
		for (auto &door : s_wall.door_list)
		{
			if (door.is_light) {
				opening = &door;
				which_wall = &s_wall;
				find = true;
				break;
			}
		}
		if (find)
			break;
	}
	//并没有找到相关的入光口,如果有只应该有一个入光口
	if (!opening)
		return;
	auto &open = *opening;
	for (auto &s_wall : p_single_case->wall_list)
	{
		if (&s_wall == which_wall)
			continue;
		Point3f Line1 = s_wall.end_corner.point - s_wall.start_corner.point;
		Point3f Line2 = open.end_point - open.start_point;
		float angle = ComMath::GetAngleBetweenTwoLines(Line1, Line2);
		//45度到135度 （垂直）
		if (0.7853f > angle || angle > 2.3559f)
			continue;
		auto centre_point = (open.start_point + open.end_point) * 0.5f;
		centre_point = ComMath::Cross(s_wall.start_corner.point - centre_point, open.direction);
		if (centre_point.z > 0)
			continue;
		for (int i = 0; i < p_region_resolver->max_row; ++i) {
			for (int j = 0; j < p_region_resolver->max_rank; ++j) {
				auto nodeRef = p_region_resolver->base_nodes[i][j];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				//投影在墙体范围内
				auto projection1 = ComMath::getPointToLinePedal(TestPoint, s_wall.start_corner.point, s_wall.end_corner.point);
				auto bproj = ComMath::isPointBetweenPoint(projection1, s_wall.start_corner.point, s_wall.end_corner.point);
				if (!bproj)
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, s_wall.start_corner.point, s_wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}

	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::CalculateCloseWallWithLightEntrance(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	//找到入户光口
	Wall*which_wall = nullptr;
	for (auto &s_wall : p_single_case->wall_list)
	{
		bool find = false;
		for (auto &windows : s_wall.window_list)
		{
			if (windows.is_light) {
				which_wall = &s_wall;
				find = true;
				break;
			}
		}
		if (find)
			break;
		for (auto &door : s_wall.door_list)
		{
			if (door.is_light) {
				which_wall = &s_wall;
				find = true;
				break;
			}
		}
		if (find)
			break;
	}
	//并没有找到相关的入光口,如果有只应该有一个入光口
	if (!which_wall)
		return;
	auto &s_wall = *which_wall;
	for (int i = 0; i < p_region_resolver->max_row; ++i) {
		for (int j = 0; j < p_region_resolver->max_rank; ++j) {
			auto nodeRef = p_region_resolver->base_nodes[i][j];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			//投影在墙体范围内
			auto projection1 = ComMath::getPointToLinePedal(TestPoint, s_wall.start_corner.point, s_wall.end_corner.point);
			auto bproj = ComMath::isPointBetweenPoint(projection1, s_wall.start_corner.point, s_wall.end_corner.point);
			if (!bproj)
				continue;
			float Dist = ComMath::getPointToLineDis(TestPoint, s_wall.start_corner.point, s_wall.end_corner.point);
			float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
			if (LinearR > nodeRef->Weight)
				nodeRef->Weight = LinearR;
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::CalculateWeightStripBetweenMaxWallWithCentre(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	//获取最长的墙体
	float max_length = 0.f;
	Wall*wall = nullptr;
	for (auto &s_wall : p_single_case->wall_list)
	{
		auto dist = ComMath::getTwoPointDistance(s_wall.start_corner.point, s_wall.end_corner.point);
		if (dist > max_length)
		{
			max_length = dist;
			wall = &s_wall;
		}
	}
	if (!wall)
		return;
	//获取AABB
	Point3f s_min = Point3f(FLT_MAX, FLT_MAX, FLT_MAX);
	Point3f s_max = -s_min;
	auto point_list = p_single_case->single_room.getPointList();
	for (auto &point : point_list) {
		s_min = ComMath::Point3fMin(s_min, point);
		s_max = ComMath::Point3fMax(s_max, point);
	}
	auto centre = (s_min + s_max)*0.5f;

	auto direction =ComMath::getNormalByLineGenerally(wall->start_corner.point, wall->end_corner.point);
	auto wall_centre = (wall->start_corner.point + wall->end_corner.point)*0.5f+ direction*20.f;
	if (!ComMath::PointInPolygon(wall_centre, point_list))
		direction = -direction;
	auto positive = wall->start_corner.point - wall->end_corner.point;
	auto negitive = -positive;

	const float float_max = 999999.9f;
	positive = centre + positive* float_max;
	negitive = centre + negitive* float_max;


	for (int i = 0; i < p_region_resolver->max_row; ++i) {
		for (int j = 0; j < p_region_resolver->max_rank; ++j) {
			auto nodeRef = p_region_resolver->base_nodes[i][j];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			if (ComMath::Dot(TestPoint - centre, direction) < 0)
				continue;
			float Dist = ComMath::getPointToLineDis(TestPoint, positive, negitive);
			if (!ComMath::IsSameScalar(Dist, formulaRef.Dist, 5.f))
				continue;
			if (formulaRef.Ratio > nodeRef->Weight)
				nodeRef->Weight = formulaRef.Ratio;
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::FCalculateOrientationWall(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{ 
	p_region_resolver->ClearLeafWeight();
	for (auto &wall : p_single_case->wall_list) {
		auto iter = map.find(formulaRef.id);
		auto &layout = iter->second;
		Point3f orientation;
		Point3f beg;
		Point3f end;
		//获取方向和起始 结束点坐标
		GetOrientationFromLayout(layout, &orientation, &beg, &end);
		Point3f Line2 = wall.end_corner.point - wall.start_corner.point;
		float angle = ComMath::GetAngleBetweenTwoLines(orientation, Line2);
		//45度到135度（垂直）,非（垂直）跳过
		if (0.7853f > angle||angle > 2.3559f)
			continue;
		//方向
		Line2 = wall.start_corner.point - beg;
		//反方向的墙体跳过
		if (ComMath::Dot(Line2, orientation) < 0)
			continue;
		for (auto row = 0; row < p_region_resolver->max_row; ++row) {
			for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
				auto &nodeRef = p_region_resolver->base_nodes[row][rank];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				auto projection1 = ComMath::getPointToLinePedal(TestPoint, wall.start_corner.point, wall.end_corner.point);
				auto bproj = ComMath::isPointBetweenPoint(projection1, wall.start_corner.point, wall.end_corner.point);
				if (!bproj)
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::FCalculateLeftWall(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{

	p_region_resolver->ClearLeafWeight();
	for (auto &wall : p_single_case->wall_list) {
		auto iter = map.find(formulaRef.id);
		auto &layout = iter->second;
		Point3f orientation;
		Point3f beg;
		Point3f end;
		//获取方向和起始 结束点坐标
		GetLeftFromLayout(layout, &orientation, &beg, &end);
		Point3f Line2 = wall.end_corner.point - wall.start_corner.point;
		float angle = ComMath::GetAngleBetweenTwoLines(orientation, Line2);
		//45度到135度（垂直）,非（垂直）跳过
		if (0.7853f > angle || angle > 2.3559f)
			continue;
		//方向
		Line2 = wall.start_corner.point - beg;
		//反方向的墙体跳过
		if (ComMath::Dot(Line2, orientation) < 0)
			continue;
		for (auto row = 0; row < p_region_resolver->max_row; ++row) {
			for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
				auto &nodeRef = p_region_resolver->base_nodes[row][rank];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				auto projection1 = ComMath::getPointToLinePedal(TestPoint, wall.start_corner.point, wall.end_corner.point);
				auto bproj = ComMath::isPointBetweenPoint(projection1, wall.start_corner.point, wall.end_corner.point);
				if (!bproj)
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef, container);
 
}

void MetisFormulaContainer::FCalculateRightWall(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto &wall : p_single_case->wall_list) {
		auto iter = map.find(formulaRef.id);
		auto &layout = iter->second;
		Point3f orientation;
		Point3f beg;
		Point3f end;
		//获取方向和起始 结束点坐标
		GetRightFromLayout(layout, &orientation, &beg, &end);
		Point3f Line2 = wall.end_corner.point - wall.start_corner.point;
		float angle = ComMath::GetAngleBetweenTwoLines(orientation, Line2);
		//45度到135度（垂直）,非（垂直）跳过
		if (0.7853f > angle || angle > 2.3559f)
			continue;
		//方向
		Line2 = wall.start_corner.point - beg;
		//反方向的墙体跳过
		if (ComMath::Dot(Line2, orientation) < 0)
			continue;
		for (auto row = 0; row < p_region_resolver->max_row; ++row) {
			for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
				auto &nodeRef = p_region_resolver->base_nodes[row][rank];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				auto projection1 = ComMath::getPointToLinePedal(TestPoint, wall.start_corner.point, wall.end_corner.point);
				auto bproj = ComMath::isPointBetweenPoint(projection1, wall.start_corner.point, wall.end_corner.point);
				if (!bproj)
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::FCalculateReverseWall(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto &wall : p_single_case->wall_list) {
		auto iter = map.find(formulaRef.id);
		auto &layout = iter->second;
		Point3f orientation;
		Point3f beg;
		Point3f end;
		//获取方向和起始 结束点坐标
		GetReverseFromLayout(layout, &orientation, &beg, &end);
		Point3f Line2 = wall.end_corner.point - wall.start_corner.point;
		float angle = ComMath::GetAngleBetweenTwoLines(orientation, Line2);
		//45度到135度（垂直）,非（垂直）跳过
		if (0.7853f > angle || angle > 2.3559f)
			continue;
		//方向
		Line2 = wall.start_corner.point - beg;
		//反方向的墙体跳过
		if (ComMath::Dot(Line2, orientation) < 0)
			continue;
		for (auto row = 0; row < p_region_resolver->max_row; ++row) {
			for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
				auto &nodeRef = p_region_resolver->base_nodes[row][rank];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				auto projection1 = ComMath::getPointToLinePedal(TestPoint, wall.start_corner.point, wall.end_corner.point);
				auto bproj = ComMath::isPointBetweenPoint(projection1, wall.start_corner.point, wall.end_corner.point);
				if (!bproj)
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::FCalculateOrientationWallSection(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	auto iter = map.find(formulaRef.id);
	auto &layout = iter->second;
	Point3f orientation;
	Point3f beg;
	Point3f end;
	//获取方向和起始 结束点坐标
	GetOrientationFromLayout(layout, &orientation, &beg, &end);
	for (auto &wall : p_single_case->wall_list) {
		Point3f Line2 = wall.end_corner.point - wall.start_corner.point;
		float angle = ComMath::GetAngleBetweenTwoLines(orientation, Line2);
		//45度到135度（垂直）,非（垂直）跳过
		if (0.7853f > angle || angle > 2.3559f)
			continue;
		//方向
		Line2 = wall.start_corner.point - beg;
		//反方向的墙体跳过
		if (ComMath::Dot(Line2, orientation) < 0)
			continue;
		for (auto row = 0; row < p_region_resolver->max_row; ++row) {
			for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
				auto &nodeRef = p_region_resolver->base_nodes[row][rank];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				//如果没有落在投影区域跳过
				auto point = ComMath::getPointToLinePedal(TestPoint, beg, end);
				if (!ComMath::isPointBetweenPoint(point, beg, end))
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::FCalculateLeftWallSection(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto &wall : p_single_case->wall_list) {
		auto iter = map.find(formulaRef.id);
		auto &layout = iter->second;
		Point3f orientation;
		Point3f beg;
		Point3f end;
		//获取方向和起始 结束点坐标
		GetLeftFromLayout(layout, &orientation, &beg, &end);
		Point3f Line2 = wall.end_corner.point - wall.start_corner.point;
		float angle = ComMath::GetAngleBetweenTwoLines(orientation, Line2);
		//45度到135度（垂直）,非（垂直）跳过
		if (0.7853f > angle || angle > 2.3559f)
			continue;
		//方向
		Line2 = wall.start_corner.point - beg;
		//反方向的墙体跳过
		if (ComMath::Dot(Line2, orientation) < 0)
			continue;
		for (auto row = 0; row < p_region_resolver->max_row; ++row) {
			for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
				auto &nodeRef = p_region_resolver->base_nodes[row][rank];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				//如果没有落在投影区域跳过
				auto point = ComMath::getPointToLinePedal(TestPoint, beg, end);
				if (!ComMath::isPointBetweenPoint(point, beg, end))
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::FCalculateRightWallSection(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto &wall : p_single_case->wall_list) {
		auto iter = map.find(formulaRef.id);
		auto &layout = iter->second;
		Point3f orientation;
		Point3f beg;
		Point3f end;
		//获取方向和起始 结束点坐标
		GetRightFromLayout(layout, &orientation, &beg, &end);
		Point3f Line2 = wall.end_corner.point - wall.start_corner.point;
		float angle = ComMath::GetAngleBetweenTwoLines(orientation, Line2);
		//45度到135度（垂直）,非（垂直）跳过
		if (0.7853f > angle || angle > 2.3559f)
			continue;
		//方向
		Line2 = wall.start_corner.point - beg;
		//反方向的墙体跳过
		if (ComMath::Dot(Line2, orientation) < 0)
			continue;
		for (auto row = 0; row < p_region_resolver->max_row; ++row) {
			for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
				auto &nodeRef = p_region_resolver->base_nodes[row][rank];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				//如果没有落在投影区域跳过
				auto point = ComMath::getPointToLinePedal(TestPoint, beg, end);
				if (!ComMath::isPointBetweenPoint(point, beg, end))
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::FCalculateReverseWallSection(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto &wall : p_single_case->wall_list) {
		auto iter = map.find(formulaRef.id);
		auto &layout = iter->second;
		Point3f orientation;
		Point3f beg;
		Point3f end;
		//获取方向和起始 结束点坐标
		GetReverseFromLayout(layout, &orientation, &beg, &end);
		Point3f Line2 = wall.end_corner.point - wall.start_corner.point;
		float angle = ComMath::GetAngleBetweenTwoLines(orientation, Line2);
		//45度到135度（垂直）,非（垂直）跳过
		if (0.7853f > angle || angle > 2.3559f)
			continue;
		//方向
		Line2 = wall.start_corner.point - beg;
		//反方向的墙体跳过
		if (ComMath::Dot(Line2, orientation) < 0)
			continue;
		for (auto row = 0; row < p_region_resolver->max_row; ++row) {
			for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
				auto &nodeRef = p_region_resolver->base_nodes[row][rank];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				//如果没有落在投影区域跳过
				auto point = ComMath::getPointToLinePedal(TestPoint, beg, end);
				if (!ComMath::isPointBetweenPoint(point, beg, end))
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::FCalculateOrientationWallRadiation(const LayoutMap&map, const MetisFuinitureFormula&formularRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	Point3f orientation;
	Point3f beg;
	Point3f end;
	auto iter = map.find(formularRef.id);
	auto &layout = iter->second;
	//获取方向和起始 结束点坐标
	GetOrientationFromLayout(layout, &orientation, &beg, &end);
	for (auto &wall : p_single_case->wall_list) {
		Point3f Line2 = wall.end_corner.point - wall.start_corner.point;
		float angle = ComMath::GetAngleBetweenTwoLines(orientation, Line2);
		//45度到135度（垂直）,非（垂直）跳过
		if (0.7853f > angle || angle > 2.3559f)
			continue;
		//方向
		Line2 = wall.start_corner.point - beg;
		//反方向的墙体跳过
		if (ComMath::Dot(Line2, orientation) < 0)
			continue;
		for (int i = 0; i < p_region_resolver->max_row; ++i) {
			for (int j = 0; j < p_region_resolver->max_rank; ++j) {
				auto nodeRef = p_region_resolver->base_nodes[i][j];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				Point3f vertical = ComMath::getPointToLinePedal(layout.translation_point, wall.start_corner.point, wall.end_corner.point);
				auto Dist = ComMath::getTwoPointDistance(TestPoint, vertical);
				if (Dist > formularRef.Dist)
					continue;
				float LinearR = (1 - Dist / formularRef.Dist)*formularRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formularRef, container);
}

void MetisFormulaContainer::FCalculateOrientationWallSectionOnLeft(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto &wall : p_single_case->wall_list) {
		auto iter = map.find(formulaRef.id);
		auto &layout = iter->second;
		Point3f orientation;
		Point3f beg;
		Point3f end;
		//获取方向和起始 结束点坐标
		GetOrientationFromLayout(layout, &orientation, &beg, &end);
		Point3f Line2 = wall.end_corner.point - wall.start_corner.point;
		float angle = ComMath::GetAngleBetweenTwoLines(orientation, Line2);
		//45度到135度（垂直）,非（垂直）跳过
		if (0.7853f > angle || angle > 2.3559f)
			continue;
		//方向
		Line2 = wall.start_corner.point - beg;
		//反方向的墙体跳过
		if (ComMath::Dot(Line2, orientation) < 0)
			continue;
		for (auto row = 0; row < p_region_resolver->max_row; ++row) {
			for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
				auto &nodeRef = p_region_resolver->base_nodes[row][rank];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				//如果落在投影区域跳过
				auto point = ComMath::getPointToLinePedal(TestPoint, beg, end);
				if (ComMath::isPointBetweenPoint(point, beg, end))
					continue;
				//落在右边区域的跳过
				point = TestPoint - end;
				point = ComMath::Cross(point, orientation);
				if (point.z < 0)
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::FCalculateOrientationWallSectionOnRight(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto &wall : p_single_case->wall_list) {
		auto iter = map.find(formulaRef.id);
		auto &layout = iter->second;
		Point3f orientation;
		Point3f beg;
		Point3f end;
		//获取方向和起始 结束点坐标
		GetOrientationFromLayout(layout, &orientation, &beg, &end);
		Point3f Line2 = wall.end_corner.point - wall.start_corner.point;
		float angle = ComMath::GetAngleBetweenTwoLines(orientation, Line2);
		//45度到135度（垂直）,非（垂直）跳过
		if (0.7853f > angle || angle > 2.3559f)
			continue;
		//方向
		Line2 = wall.start_corner.point - beg;
		//反方向的墙体跳过
		if (ComMath::Dot(Line2, orientation) < 0)
			continue;
		for (auto row = 0; row < p_region_resolver->max_row; ++row) {
			for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
				auto &nodeRef = p_region_resolver->base_nodes[row][rank];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				//如果落在投影区域跳过
				auto point = ComMath::getPointToLinePedal(TestPoint, beg, end);
				if (ComMath::isPointBetweenPoint(point, beg, end))
					continue;
				//落在右边区域的跳过
				point = TestPoint - end;
				point = ComMath::Cross(point, orientation);
				if (point.z> 0)
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef, container);
}


void MetisFormulaContainer::FCalculateReverseWallSectionOnLeft(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto &wall : p_single_case->wall_list) {
		auto iter = map.find(formulaRef.id);
		auto &layout = iter->second;
		Point3f orientation;
		Point3f beg;
		Point3f end;
		//获取方向和起始 结束点坐标
		GetReverseFromLayout(layout, &orientation, &beg, &end);
		Point3f Line2 = wall.end_corner.point - wall.start_corner.point;
		float angle = ComMath::GetAngleBetweenTwoLines(orientation, Line2);
		//45度到135度（垂直）,非（垂直）跳过
		if (0.7853f > angle || angle > 2.3559f)
			continue;
		//方向
		Line2 = wall.start_corner.point - beg;
		//反方向的墙体跳过
		if (ComMath::Dot(Line2, orientation) < 0)
			continue;
		for (auto row = 0; row < p_region_resolver->max_row; ++row) {
			for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
				auto &nodeRef = p_region_resolver->base_nodes[row][rank];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				//如果落在投影区域跳过
				auto point = ComMath::getPointToLinePedal(TestPoint, beg, end);
				if (ComMath::isPointBetweenPoint(point, beg, end))
					continue;
				//落在右边区域的跳过
				point = TestPoint - end;
				point = ComMath::Cross(point, orientation);
				if (point.z < 0)
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef, container);
}


void MetisFormulaContainer::FCalculateReverseWallSectionOnRight(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	for (auto &wall : p_single_case->wall_list) {
		auto iter = map.find(formulaRef.id);
		auto &layout = iter->second;
		Point3f orientation;
		Point3f beg;
		Point3f end;
		//获取方向和起始 结束点坐标
		GetReverseFromLayout(layout, &orientation, &beg, &end);
		Point3f Line2 = wall.end_corner.point - wall.start_corner.point;
		float angle = ComMath::GetAngleBetweenTwoLines(orientation, Line2);
		//45度到135度（垂直）,非（垂直）跳过
		if (0.7853f > angle || angle > 2.3559f)
			continue;
		//方向
		Line2 = wall.start_corner.point - beg;
		//反方向的墙体跳过
		if (ComMath::Dot(Line2, orientation) < 0)
			continue;
		for (auto row = 0; row < p_region_resolver->max_row; ++row) {
			for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
				auto &nodeRef = p_region_resolver->base_nodes[row][rank];
				if (!nodeRef->bInside)
					continue;
				Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
				//如果落在投影区域跳过
				auto point = ComMath::getPointToLinePedal(TestPoint, beg, end);
				if (ComMath::isPointBetweenPoint(point, beg, end))
					continue;
				//落在右边区域的跳过
				point = TestPoint - end;
				point = ComMath::Cross(point, orientation);
				if (point.z > 0)
					continue;
				float Dist = ComMath::getPointToLineDis(TestPoint, wall.start_corner.point, wall.end_corner.point);
				float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
				if (LinearR > nodeRef->Weight)
					nodeRef->Weight = LinearR;
			}
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::FCalculateOrientataionLineRegion(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	Point3f orientation;
	Point3f beg;
	Point3f end;
	auto iter = map.find(formulaRef.id);
	auto &layout = iter->second;
	GetOrientationFromLayout(layout, &orientation, &beg, &end);
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			//如果没有落在投影区域跳过
			auto point = ComMath::getPointToLinePedal(TestPoint, beg, end);
			if (!ComMath::isPointBetweenPoint(point, beg, end))
				continue;
			//反方向的点跳过
			point = TestPoint - end;
			if (ComMath::Dot(point, orientation) < 0)
				continue;
			float Dist = ComMath::getPointToLineDis(TestPoint, beg, end);
			float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
			if (LinearR > nodeRef->Weight)
				nodeRef->Weight = LinearR;
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::FCalculateLeftLineRegion(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	Point3f orientation;
	Point3f beg;
	Point3f end;
	auto iter = map.find(formulaRef.id);
	auto &layout = iter->second;
	GetLeftFromLayout(layout, &orientation, &beg, &end);
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			//如果没有落在投影区域跳过
			auto point = ComMath::getPointToLinePedal(TestPoint, beg, end);
			if (!ComMath::isPointBetweenPoint(point, beg, end))
				continue;
			//反方向的点跳过
			point = TestPoint - end;
			if (ComMath::Dot(point, orientation) < 0)
				continue;
			float Dist = ComMath::getPointToLineDis(TestPoint, beg, end);
			float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
			if (LinearR > nodeRef->Weight)
				nodeRef->Weight = LinearR;
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::FCalculateRightLineRegion(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	Point3f orientation;
	Point3f beg;
	Point3f end;
	auto iter = map.find(formulaRef.id);
	auto &layout = iter->second;
	GetRightFromLayout(layout, &orientation, &beg, &end);
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			//如果没有落在投影区域跳过
			auto point = ComMath::getPointToLinePedal(TestPoint, beg, end);
			if (!ComMath::isPointBetweenPoint(point, beg, end))
				continue;
			//反方向的点跳过
			point = TestPoint - end;
			if (ComMath::Dot(point, orientation) < 0)
				continue;
			float Dist = ComMath::getPointToLineDis(TestPoint, beg, end);
			float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
			if (LinearR > nodeRef->Weight)
				nodeRef->Weight = LinearR;
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::FCalculateReverseLineRegion(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	Point3f orientation;
	Point3f beg;
	Point3f end;
	auto iter = map.find(formulaRef.id);
	auto &layout = iter->second;
	GetReverseFromLayout(layout, &orientation, &beg, &end);
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			//如果没有落在投影区域跳过
			auto point = ComMath::getPointToLinePedal(TestPoint, beg, end);
			if (!ComMath::isPointBetweenPoint(point, beg, end))
				continue;
			//反方向的点跳过
			point = TestPoint - end;
			if (ComMath::Dot(point, orientation) < 0)
				continue;
			float Dist = ComMath::getPointToLineDis(TestPoint, beg, end);
			float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
			if (LinearR > nodeRef->Weight)
				nodeRef->Weight = LinearR;
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::FCalculateOrientataionSectorRegion(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	Point3f orientation;
	Point3f beg;
	Point3f end;
	auto iter = map.find(formulaRef.id);
	auto &layout = iter->second;
	GetOrientationFromLayout(layout, &orientation, &beg, &end);
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			//反方向的点跳过
			auto point = TestPoint - end;
			if (ComMath::Dot(point, orientation) < 0)
				continue;
			float Dist = ComMath::getTwoPointDistance(TestPoint, (beg + end)*0.5);
			float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
			if (LinearR > nodeRef->Weight)
				nodeRef->Weight = LinearR;
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::FCalculateLeftSectorRegion(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	Point3f orientation;
	Point3f beg;
	Point3f end;
	auto iter = map.find(formulaRef.id);
	auto &layout = iter->second;
	GetLeftFromLayout(layout, &orientation, &beg, &end);
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			//反方向的点跳过
			auto point = TestPoint - end;
			if (ComMath::Dot(point, orientation) < 0)
				continue;
			float Dist = ComMath::getTwoPointDistance(TestPoint, (beg + end)*0.5);
			float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
			if (LinearR > nodeRef->Weight)
				nodeRef->Weight = LinearR;
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::FCalculateRightSectorRegion(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	Point3f orientation;
	Point3f beg;
	Point3f end;
	auto iter = map.find(formulaRef.id);
	auto &layout = iter->second;
	GetRightFromLayout(layout, &orientation, &beg, &end);
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			//反方向的点跳过
			auto point = TestPoint - end;
			if (ComMath::Dot(point, orientation) < 0)
				continue;
			float Dist = ComMath::getTwoPointDistance(TestPoint, (beg + end)*0.5);
			float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
			if (LinearR > nodeRef->Weight)
				nodeRef->Weight = LinearR;
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::FCalculateReverseSectorRegion(const LayoutMap&map, const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	p_region_resolver->ClearLeafWeight();
	Point3f orientation;
	Point3f beg;
	Point3f end;
	auto iter = map.find(formulaRef.id);
	auto &layout = iter->second;
	GetReverseFromLayout(layout, &orientation, &beg, &end);
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &nodeRef = p_region_resolver->base_nodes[row][rank];
			if (!nodeRef->bInside)
				continue;
			Point3f TestPoint = Point3f(nodeRef->center_point.x, nodeRef->center_point.y, 0);
			//反方向的点跳过
			auto point = TestPoint - end;
			if (ComMath::Dot(point, orientation) < 0)
				continue;
			float Dist = ComMath::getTwoPointDistance(TestPoint, (beg + end)*0.5);
			float LinearR = (1 - Dist / formulaRef.Dist)*formulaRef.Ratio;
			if (LinearR > nodeRef->Weight)
				nodeRef->Weight = LinearR;
		}
	}
	FillNodeArray(formulaRef, container);
}

void MetisFormulaContainer::GetOrientationFromLayout(const SingleFurnitureSingleRuleLayoutData&Ref, Point3f*Orientation, Point3f*Start, Point3f*End)
{

    *Orientation = Point3f(1, 0, 0);
    auto rotate = (float)Ref.rotation_degree / 360.f*6.283185307f;
    auto x = cosf(rotate)*Orientation->x - sinf(rotate)*Orientation->y;
    Orientation->y = sinf(rotate)*Orientation->x + cosf(rotate)*Orientation->y;
    Orientation->x = x;
    auto orientation = *Orientation;
    int index = 0;
    for (int i = 0; i < 4; ++i)
    {
        Point3f beg = Point3f(Ref.furniture_vertex_points[i].x - Ref.translation_point.x, Ref.furniture_vertex_points[i].y - Ref.translation_point.y, 0);
        Point3f end = Point3f(Ref.furniture_vertex_points[(i + 1) % 4].x - Ref.translation_point.x, Ref.furniture_vertex_points[(i + 1) % 4].y - Ref.translation_point.y, 0);
        auto angle = ComMath::GetAngleBetweenTwoLines(end - beg, *Orientation);
        //只要90度接近
        if (0.7853f > angle || angle > 2.3559f)
            continue;
        auto dot = ComMath::Dot(*Orientation, beg);
        if (dot < 0)
            continue;
        index = i;
    }
    *Start = Point3f(Ref.furniture_vertex_points[index].x, Ref.furniture_vertex_points[index].y, 0);
    *End = Point3f(Ref.furniture_vertex_points[(index + 1) % 4].x, Ref.furniture_vertex_points[(index + 1) % 4].y, 0);
}

void MetisFormulaContainer::GetLeftFromLayout(const SingleFurnitureSingleRuleLayoutData&Ref, Point3f*Orientation, Point3f*Start, Point3f*End)
{
    *Orientation = Point3f(1, 0, 0);
    auto rotate = (float)Ref.rotation_degree / 360.f*6.283185307f;
    auto x = cosf(rotate)*Orientation->x - sinf(rotate)*Orientation->y;
    Orientation->y = sinf(rotate)*Orientation->x + cosf(rotate)*Orientation->y;
    Orientation->x = x;
    auto orientation = *Orientation;
    int index = -1;
    for (int i = 0; i < 4; ++i)
    {
        Point3f beg = Point3f(Ref.furniture_vertex_points[i].x - Ref.translation_point.x, Ref.furniture_vertex_points[i].y - Ref.translation_point.y, 0);
        Point3f end = Point3f(Ref.furniture_vertex_points[(i + 1) % 4].x - Ref.translation_point.x, Ref.furniture_vertex_points[(i + 1) % 4].y - Ref.translation_point.y, 0);
        auto angle = ComMath::GetAngleBetweenTwoLines(end - beg, orientation);
        //只要90度接近
        if (0.7853f < angle && angle < 2.3559f)
            continue;
        if (ComMath::Cross((end + beg)*0.5, orientation).z < 0)
            continue;
        index = i;
        *Orientation = (end + beg)*0.5;
    }
    *Start = Point3f(Ref.furniture_vertex_points[index].x, Ref.furniture_vertex_points[index].y, 0);
    *End = Point3f(Ref.furniture_vertex_points[(index + 1) % 4].x, Ref.furniture_vertex_points[(index + 1) % 4].y, 0);

}

void MetisFormulaContainer::GetRightFromLayout(const SingleFurnitureSingleRuleLayoutData&Ref, Point3f*Orientation, Point3f*Start, Point3f*End)
{
    *Orientation = Point3f(1, 0, 0);
    auto rotate = (float)Ref.rotation_degree / 360.f*6.283185307f;
    auto x = cosf(rotate)*Orientation->x - sinf(rotate)*Orientation->y;
    Orientation->y = sinf(rotate)*Orientation->x + cosf(rotate)*Orientation->y;
    Orientation->x = x;
    auto orientation = *Orientation;
    int index = -1;
    for (int i = 0; i < 4; ++i)
    {
        Point3f beg = Point3f(Ref.furniture_vertex_points[i].x - Ref.translation_point.x, Ref.furniture_vertex_points[i].y - Ref.translation_point.y, 0);
        Point3f end = Point3f(Ref.furniture_vertex_points[(i + 1) % 4].x - Ref.translation_point.x, Ref.furniture_vertex_points[(i + 1) % 4].y - Ref.translation_point.y, 0);
        auto angle = ComMath::GetAngleBetweenTwoLines(end - beg, orientation);
        //只要90度接近
        if (0.7853f < angle && angle < 2.3559f)
            continue;
        if (ComMath::Cross((end + beg)*0.5, orientation).z > 0)
            continue;
        index = i;
        *Orientation = (end + beg)*0.5;
    }
    *Start = Point3f(Ref.furniture_vertex_points[index].x, Ref.furniture_vertex_points[index].y, 0);
    *End = Point3f(Ref.furniture_vertex_points[(index + 1) % 4].x, Ref.furniture_vertex_points[(index + 1) % 4].y, 0);

}

void MetisFormulaContainer::GetReverseFromLayout(const SingleFurnitureSingleRuleLayoutData&Ref, Point3f*Orientation, Point3f*Start, Point3f*End)
{
    *Orientation = Point3f(1, 0, 0);
    auto rotate = (float)Ref.rotation_degree / 360.f*6.283185307f;
    auto x = cosf(rotate)*Orientation->x - sinf(rotate)*Orientation->y;
    Orientation->y = sinf(rotate)*Orientation->x + cosf(rotate)*Orientation->y;
    Orientation->x = x;
    int index = -1;
    for (int i = 0; i < 4; ++i)
    {
        Point3f beg = Point3f(Ref.furniture_vertex_points[i].x - Ref.translation_point.x, Ref.furniture_vertex_points[i].y - Ref.translation_point.y, 0);
        Point3f end = Point3f(Ref.furniture_vertex_points[(i + 1) % 4].x - Ref.translation_point.x, Ref.furniture_vertex_points[(i + 1) % 4].y - Ref.translation_point.y, 0);
        auto angle = ComMath::GetAngleBetweenTwoLines(end - beg, *Orientation);
        //只要90度接近
        if (0.7853f > angle || angle > 2.3559f)
            continue;
        auto dot = ComMath::Dot(*Orientation, beg);
        if (dot > 0)
            continue;
        index = i;
    }
    *Orientation = -*Orientation;
    *Start = Point3f(Ref.furniture_vertex_points[index].x, Ref.furniture_vertex_points[index].y, 0);
    *End = Point3f(Ref.furniture_vertex_points[(index + 1) % 4].x, Ref.furniture_vertex_points[(index + 1) % 4].y, 0);
}

void MetisFormulaContainer::FillNodeArray(const MetisFuinitureFormula&formulaRef, FormulaContainer&container)
{
	for (auto row = 0; row < p_region_resolver->max_row; ++row) {
		for (auto rank = 0; rank < p_region_resolver->max_rank; ++rank) {
			auto &node = p_region_resolver->base_nodes[row][rank];
			auto &container_node = container[row][rank];
			if (!node->bInside)
				continue;
			float temp = container_node->Weight;
			*(container_node) = *node;
			if (formulaRef.Operator == E_Plus)
				container_node->Weight = temp + container_node->Weight;
			if (formulaRef.Operator == E_Minus)
				container_node->Weight = temp - container_node->Weight;
		}
	}
}