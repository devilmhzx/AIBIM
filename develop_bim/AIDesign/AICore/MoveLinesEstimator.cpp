#include "MoveLinesEstimator.h"
#include "../ComMath.h"
#include <assert.h>
#include <unordered_map>
#include "../Log/easylogging++.h"
#define CUSTOM_SIZE
const float granularity = 20.f;
const float wallinfluence =20.f;
const Point3f Zero(0.f, 0.f, 0.f);
const Point3f CorrectPoint3f(7.f, 7.f, 0);

bool operator< (const PathNode &Left, const PathNode &Right)
{
	if (Left.x == Right.x)
		return Left.y < Right.y;
	else
		return Left.x < Right.x;
	return false;
}

void  CorrectLines(std::set<int>&open, std::vector<Line>&ref,int which,Point3f direction)
{
 
	for (int i = 0; i < ref.size(); ++i)
	{
		//跳过正在处理的线条
		if (open.find(i) != open.end())
			continue;
		auto this_line = ref[which];
		this_line.start = this_line.start - direction;
		this_line.end = this_line.end - direction;
		auto &search_line = ref[i];
		bool connection_1 = false;
		if (ComMath::isPointOnLine(search_line.start, this_line.start, this_line.end))
			connection_1 = true;
		if(ComMath::isPointOnLine(search_line.end, this_line.start, this_line.end))
			connection_1 = true;

	 
		if (!connection_1) {
			continue;
		}
		auto hr = ComMath::isParallelTwoLine(Zero,direction, search_line.start ,search_line.end);
		if (hr)
		{
			auto proj = ComMath::getPointToLinePedal(this_line.start, search_line.start, search_line.end);
			
				auto dist_start = ComMath::getPointToLineDis(search_line.start, this_line.start, this_line.end);
				auto dist_end = ComMath::getPointToLineDis(search_line.end, this_line.start, this_line.end);
				if (dist_start < dist_end)
					search_line.start = search_line.start + direction;
				else
					search_line.end = search_line.end + direction;
			
			open.insert(i);
		}
		else
		{
			open.insert(i);
			search_line.start = search_line.start + direction;
			search_line.end = search_line.end + direction;
			CorrectLines(open, ref, i, direction);
		}
	}
}

MoveLinesEstimator::MoveLinesEstimator()
{
}


MoveLinesEstimator::~MoveLinesEstimator()
{
}

// 获得布局动线的数组
Json::Value MoveLinesEstimator::GetMovingLinesJson(AICase*ai_case)
{
	
	MoveLine struct_moveline;
	struct_moveline.housework_lines = GetHouseworkLines(ai_case);
	if (struct_moveline.housework_lines.size() < 1)
		LOG(INFO) << "Not housework lines.";

	struct_moveline.visitor_lines = GetVisitorLines(ai_case);
	if (struct_moveline.visitor_lines.size() < 1)
		LOG(INFO) << "Not visitor lines.";

	struct_moveline.family_lines = GetFamilyLines(ai_case);
	if (struct_moveline.family_lines.size() < 1)
		LOG(INFO) << "Not family lines.";

	 
	// 生成动线json
	Json::Value movingLine;
	Json::Value housework = GetHouseworkJson(struct_moveline.housework_lines);
	Json::Value visitor = GetVisitorJosn(struct_moveline.visitor_lines);;
	Json::Value family = GetFamilyJson(struct_moveline.family_lines);;
	Json::Value  reduceScore =GetScore(struct_moveline, ai_case);
	movingLine["version"] = 10001;
	movingLine["reduceScore"] = reduceScore;
	movingLine["housework"] = housework;
	movingLine["visitor"] = visitor;
	movingLine["family"] = family;

	return movingLine;

}

void MoveLinesEstimator::SetHouseworkModelId(std::vector<Model>&model) {
	model_no.clear();
	for (auto i : model)
		model_no.insert(i.GetNo());
}

std::vector<std::vector<EstimatorNode>>& MoveLinesEstimator::EstimateMoveLines(AICase*ai_case) {
	BuildRegionGrid(ai_case);
	SplitGridToInsideRegion(ai_case);
	DivideModelToRoom(ai_case);
	BuildRelationContainer();
	BuildDoorWithRegionRelation(ai_case);
	OpenDoorNode(ai_case);
	AddRouteRegionToRelationContainer(ai_case);
	AddRouteDoorToRelationContaier(ai_case);
	auto ref=FigureOutRelationPathWithAStart(ai_case);
	ref = FindRouteWithRestModel(ai_case);
	for (auto &path : m_door_path) {
		if(path.second.another_room.size())
			for (auto node : *path.second.another_path) {
				base_leaf_node[node.x][node.y].flag |= E_Debug;
			} 
		if (path.second.room.size()) {
			for (auto node : *path.second.path_node) {
				base_leaf_node[node.x][node.y].flag |= E_Debug;
			}
		}
	}
	for (auto &path : minor_path) {
		for (auto &node : path) {
			base_leaf_node[node.x][node.y].flag |= E_Debug;
		}
	}
	return base_leaf_node;
}

void MoveLinesEstimator::Clear() {
	base_leaf_node.clear();
	room_conrresponding_model.clear();
	m_path.clear();
	m_temp_path.clear();
	m_room_container.clear();
	m_door_path.clear();
	relation_room_door.clear();
	pass_model.clear();
	minor_path.clear();
	line_result.clear();
}
void MoveLinesEstimator::CorrectMoveLine(std::vector<Line>&dest, const std::vector<Line> &other_lines)
{
	if (!other_lines.size())
		return;
	int m = 0;
	while (true)
	{
		bool find = false;
		int    ci = 0;
		int    cj = 0;
		//遍历发现重合线条
		for (int i = 0; i < dest.size(); ++i)
		{
			for (int j = 0; j < other_lines.size(); ++j)
			{
				auto hresult = ComMath::isParallelTwoLine(dest[i].start, dest[i].end, other_lines[j].start, other_lines[j].end);
				//两个线端平行
				if (hresult) {
					//点在一条线上 认为重叠包括端点
					if (ComMath::isPointOnLine(dest[i].start, other_lines[j].start, other_lines[j].end)) {
						find = true;
						ci = i;
						cj = j;
						break;
					}
					if (ComMath::isPointOnLine(dest[i].end, other_lines[j].start, other_lines[j].end)) {
						cj = j;
						ci = i;
						find = true;
						break;
					}
					if (ComMath::isPointOnLine(other_lines[j].end, dest[i].start, dest[i].end)) {
						cj = j;
						ci = i;
						find = true;
						break;
					}
					if (ComMath::isPointOnLine(other_lines[j].end, dest[i].start, dest[i].end)) {
						cj = j;
						ci = i;
						find = true;
						break;
					}
				}
			}
			if (find)
				break;
		}
		if (!find)
			break;
		//插入第一条重叠的直线
		std::set<int> line_need_corrected;
		Point3f direction = ComMath::getNormalByLine(dest[ci].start, dest[ci].end);
		line_need_corrected.insert(ci);
		dest[ci].start = dest[ci].start + direction * granularity*0.5;
		dest[ci].end = dest[ci].end + direction * granularity*0.5;
		CorrectLines(line_need_corrected, dest, ci, direction*granularity*0.5);
	}
}


bool MoveLinesEstimator::DetectModelOnOuter(AICase*ai_case) {
	for (auto &id : model_no) {
		auto model=ai_case->design.GetModelByNo(id);
		if (!model)
			return false;
		bool inside = model->location.x <= Max.x&&model->location.x >= Min.x;
		inside&= model->location.y<= Max.y&&model->location.y >= Min.y;
		if (!inside)
			return false;
	}
	return true;
}

bool MoveLinesEstimator::EstimateModelMoveLines(AICase*ai_case) {
	MoveLinesEstimator::Clear();
	//建立网格区域
	if (!BuildRegionGrid(ai_case))
	{
		LOG(ERROR) << "Build region grid error (BuildRegionGrid).";
		return false;
	}
	 
	if (!isVaildSignleCase(ai_case))
	{
		LOG(ERROR) << "Door is out room (DetectModelOnOuter).";
		return false;
	}
	//检测模型是不是在房间外面,如果是直接返回错误
	if (!DetectModelOnOuter(ai_case))
	{
		LOG(ERROR) << "Model is out room (DetectModelOnOuter).";
		return false;
	}
	//划分网格类型: 户型区域内,户型区域外
	SplitGridToInsideRegion(ai_case);
	//无法建立节点
	if (!base_leaf_node.size())
	{
		LOG(ERROR) << "Can not build nodes (base_leaf_node).";
		return false;
	}
	//Inside-lize门的区域节点
	OpenDoorNode(ai_case);
	//模型划分到房间内
	DivideModelToRoom(ai_case);
	//如果只有一个房间
	if (!room_conrresponding_model.size())
	{
		LOG(ERROR) << "Not room (room_conrresponding_model =0).";
		return false;
	}
	if (room_conrresponding_model.size() == 1) 
	{
		LOG(INFO) << "Only one room!";
		if (!FindMoveLineWithOneRegion(ai_case))
		{
			LOG(ERROR) << "Find moveline error (FindMoveLineWithOneRegion).";
			return false;
		}
		 NodePathToLineContainerWithSignleRegion();
		 return true;
	}
	//建立路径容器容器
	BuildRelationContainer();
	//建立门和区域的关系
	if (!BuildDoorWithRegionRelation(ai_case))
	{
		LOG(ERROR) << "Build relation of door and region error (BuildDoorWithRegionRelation).";
		return false;
	}
	//路径规划(经过区域)
	if (!AddRouteRegionToRelationContainer(ai_case))
	{
		LOG(ERROR) << "Add route region to relation container error (AddRouteRegionToRelationContainer).";
		return false;
	}
	//路径规划(经过门)
	if (!AddRouteDoorToRelationContaier(ai_case))
	{
		LOG(ERROR) << "Add route door to relation container error(AddRouteDoorToRelationContaier).";
		return false;
	}
	if (!FigureOutRelationPathWithAStart(ai_case))
	{
		LOG(ERROR) << "Find relation path with a_start error (FigureOutRelationPathWithAStart).";
		return false;
	}
	if (!FindRouteWithRestModel(ai_case))
	{
		LOG(ERROR) << "Find route with model error (FindRouteWithRestModel).";
		return false;
	}
	NodePathToLineContainer();
	return true;
}

const  std::vector<Line>&MoveLinesEstimator::GetMoveLines() {
	return line_result;
}

bool MoveLinesEstimator::BuildRegionGrid(AICase*ai_case) {
	Max = Point3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	Min = Point3f(FLT_MAX, FLT_MAX, FLT_MAX);

	for (auto &ref : ai_case->single_case_list) {
		for (auto &wall : ref.wall_list) {
			Max = ComMath::Point3fMax(Max, wall.start_corner.point);
			Max = ComMath::Point3fMax(Max, wall.end_corner.point);
			Min = ComMath::Point3fMin(Min, wall.start_corner.point);
			Min = ComMath::Point3fMin(Min, wall.end_corner.point);
		}
	}
	for (auto &ref : ai_case->house.door_list)
	{
			Max = ComMath::Point3fMax(Max, ref.pos);
			Min = ComMath::Point3fMin(Min, ref.pos);
	}
	//扩展面积
	Max = Max + Point3f(granularity, granularity, granularity)*2;
	Min = Min - Point3f(granularity, granularity, granularity)*2;
	auto temp = Max - Min;
	max_row =(unsigned)ceilf(temp.y / granularity);
	max_column = (unsigned)ceilf(temp.x / granularity);

	base_leaf_node.resize(max_row);
	for (auto &ref : base_leaf_node)
		ref.resize(max_column);
	//初始化所有节点与房间外
	for(unsigned i=0;i<max_row;++i)
		for (unsigned j = 0; j < max_column; ++j) {
			base_leaf_node[i][j].centre_point = Min + Point3f(granularity*j, granularity*i, 0);
			base_leaf_node[i][j].flag = E_Outside;
		}
	if (!base_leaf_node.size())
		return false;
	return true;
}

bool MoveLinesEstimator::isVaildSignleCase(AICase*ai_case)
{
 

	for (auto door : ai_case->house.door_list)
	{
		auto pathNode = CalculateNodeIndexOnCoordinate(door.pos);
		if ((unsigned)pathNode.x < 0 || (unsigned)pathNode.x >= max_row)
			return false;
		if ((unsigned)pathNode.y < 0 || (unsigned)pathNode.y >= max_column)
			return false;
	}
	return true;
}

void MoveLinesEstimator::SplitGridToInsideRegion(AICase*ai_case) {
 
	std::vector<std::vector<Point3f>> coner_list;
	for (auto &ref : ai_case->single_case_list) {
		std::vector<Point3f> list;
		for (auto &point : ref.corner_list)
			list.push_back(point.point);
		coner_list.push_back(std::move(list));
	}

	for (unsigned i = 0; i < max_row; ++i)
		for (unsigned j = 0; j < max_column; ++j) {
			for (auto &vec_list : coner_list) {
				if (ComMath::PointInPolygon(base_leaf_node[i][j].centre_point, vec_list)) {
					base_leaf_node[i][j].flag &= ~E_Outside;
					break;
				}
			}
		}
}

EstimatorNode*MoveLinesEstimator::GetNodeAddr(PathNode Node) {
	return &base_leaf_node[Node.x][Node.y];
}

PathNode MoveLinesEstimator::CalculateNodeIndexOnCoordinate(Point3f&Ref) {
	auto Temp = Ref - Min;
	auto i = (int)ceilf(Temp.y / granularity);
	auto j= (int)ceil(Temp.x / granularity);
	return PathNode(i, j);
}

void MoveLinesEstimator::ModifyNodeToZeroState() {
	for (auto &Ref : base_leaf_node) {
		for (auto &node : Ref) {
			node.flag &= ~E_Close;
		}
	}
}

void MoveLinesEstimator::SetNodeToClose(int i, int j) {
	assert(i < (int)max_row&&j < (int)max_column);
	base_leaf_node[i][j].flag |= E_Close;
}

std::vector<Point3f> MoveLinesEstimator::CalculateModelWorldCoordinateBoundingBox(Model*smodel) {
	//求世界坐标
	std::vector<Point3f> Out;
	Model&model = *smodel;
	auto loc = Point3f(model.location.x, model.location.y, 0);
#ifdef CUSTOM_SIZE
	model.width = 60;
	model.length = 60;
#endif // !CUSTOM_SIZE
	Out.push_back(Point3f(-model.width*0.5f, model.length*0.5f, 0.f));
	Out.push_back(Point3f(model.width*0.5f, model.length*0.5f, 0.f));
	Out.push_back(Point3f(-model.width*0.5f, -model.length*0.5f, 0.f));
	Out.push_back(Point3f(model.width*0.5f, -model.length*0.5f, 0.f));	 
	float angle = model.rotation.y / 360.f*(3.1415926f * 2);
	for (auto &ref : Out) {
		auto x = cosf(angle)*ref.x - sinf(angle)*ref.y;
		ref.y = sinf(angle)*ref.x + cosf(angle)*ref.y;
		ref.x = x;
		ref = ref+model.location;
	}
	//求ABBA
	Point3f Maxf = Point3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	Point3f Minf = Point3f(FLT_MAX, FLT_MAX, FLT_MAX);
	for (auto &ref : Out) {
		Maxf = ComMath::Point3fMax(ref, Maxf);
		Minf = ComMath::Point3fMin(ref, Minf);
	}
	Out.clear();
	Out.push_back(Maxf);
	Out.push_back(Minf);
	return Out;
}

bool MoveLinesEstimator::BuildHouseworkLines(AICase*ai_case) {
	
	return true;
}
 
void MoveLinesEstimator::NodePathToLineContainer() {
	 
	std::set<PathInformation> information;
	for (auto &path : m_door_path) {
		if (path.second.another_room.size()) {
			if(information.find(path.second.another_path)== information.end())
				information.insert(path.second.another_path);
		}
		if (path.second.room.size()) {
			if (information.find(path.second.path_node) == information.end())
				information.insert(path.second.path_node);
		}
	}
 
	for (auto &path : information) {
		bool open = false;
		std::pair<OrientationNode, size_t> 	 set_temp;
		for (size_t i = 0; i < path->size(); ++i)
		{
			if (i == 0)
				continue;
			auto   &node_path = *path;
			auto orientation = MakeSureOrientationOfTurningPoint(node_path[i - 1], node_path[i]);
			if (!open) {
				set_temp = std::make_pair(orientation, i - 1);
				open = true;
			}
			if (i == path->size() - 1)
			{
				if (set_temp.first == orientation) {
					PathNode &beg = node_path[set_temp.second];
					PathNode &end = node_path[i];
					auto  beg3f = GridCoordToWorldCoord(beg.x, beg.y);
					auto  end3f = GridCoordToWorldCoord(end.x, end.y);
					line_result.push_back(Line(beg3f, end3f,beg3f -end3f));
				}
				else
				{
					PathNode &beg = node_path[set_temp.second];
					PathNode &end = node_path[i - 1];
					PathNode &finals = node_path[i];
					auto  beg3f = GridCoordToWorldCoord(beg.x, beg.y);
					auto  end3f = GridCoordToWorldCoord(end.x, end.y);
					auto  finalsv = GridCoordToWorldCoord(finals.x, finals.y);
					line_result.push_back(Line(beg3f, end3f, beg3f - end3f));
					line_result.push_back(Line(end3f, finalsv, end3f-finalsv));
				}
			}
			else
			{
				 if (set_temp.first != orientation)
				 {
					PathNode &beg = node_path[set_temp.second];
					PathNode &end = node_path[i - 1];
					auto  beg3f = GridCoordToWorldCoord(beg.x, beg.y);
					auto  end3f = GridCoordToWorldCoord(end.x, end.y);
					line_result.push_back(Line(beg3f, end3f, beg3f-end3f));
					set_temp = std::make_pair(orientation, i - 1);
				}
			}
		}
	}

	for (auto &path : minor_path) {
		bool open = false;
		std::pair<OrientationNode, size_t> 	 set_temp;
		for (size_t i = 0; i < path.size(); ++i)
		{
			if (i == 0)
				continue;
			auto   &node_path = path;
			auto orientation = MakeSureOrientationOfTurningPoint(node_path[i - 1], node_path[i]);
			if (!open) {
				set_temp = std::make_pair(orientation, i - 1);
				open = true;
			}
			if (i == path.size() - 1)
			{
				if (set_temp.first == orientation) {
					PathNode &beg = node_path[set_temp.second];
					PathNode &end = node_path[i];
					auto  beg3f = GridCoordToWorldCoord(beg.x, beg.y);
					auto  end3f = GridCoordToWorldCoord(end.x, end.y);
					line_result.push_back(Line(beg3f, end3f, beg3f - end3f));
				}
				else
				{
					PathNode &beg = node_path[set_temp.second];
					PathNode &end = node_path[i - 1];
					PathNode &finals = node_path[i];
					auto  beg3f = GridCoordToWorldCoord(beg.x, beg.y);
					auto  end3f = GridCoordToWorldCoord(end.x, end.y);
					auto  finalsv = GridCoordToWorldCoord(finals.x, finals.y);
					line_result.push_back(Line(beg3f, end3f, beg3f - end3f));
					line_result.push_back(Line(end3f, finalsv, end3f - finalsv));
				}
			}
			else
			{
				if (set_temp.first != orientation)
				{
					PathNode &beg = node_path[set_temp.second];
					PathNode &end = node_path[i - 1];
					auto  beg3f = GridCoordToWorldCoord(beg.x, beg.y);
					auto  end3f = GridCoordToWorldCoord(end.x, end.y);
					line_result.push_back(Line(beg3f, end3f, beg3f - end3f));
					set_temp = std::make_pair(orientation, i - 1);
				}
			}
		}
	}
	//去除重复线段
	CombineCoincidiceLines();
}

void MoveLinesEstimator::NodePathToLineContainerWithSignleRegion() {
	for (auto &path : minor_path) {
		std::pair<OrientationNode, size_t> 	 set_temp;
		bool open = false;
		for (size_t i = 0; i < path.size(); ++i) {
			if (i == 0)
				continue;
			auto   &node_path = path;
			auto orientation = MakeSureOrientationOfTurningPoint(node_path[i - 1], node_path[i]);
			if (!open) {
				set_temp = std::make_pair(orientation, i - 1);
				open = true;
			}
			else
			{
				//方向一样的,是一条线段
				if (set_temp.first == orientation)
					continue;
				else
				{
					PathNode &beg = node_path[set_temp.second];
					PathNode &end = node_path[i - 1];
					auto  beg3f = GridCoordToWorldCoord(beg.x, beg.y);
					auto  end3f = GridCoordToWorldCoord(end.x, end.y);
					line_result.push_back(Line(beg3f, end3f,beg3f-end3f));
					set_temp = std::make_pair(orientation, i - 1);
				}
			}
		}
	}
}

void MoveLinesEstimator::DivideModelToRoom(AICase*ai_case) {
	 //找到每个区域对应的模型
	for (auto &room : ai_case->house.room_list)
	{
		auto Room_NO = room.GetNo();
		auto model_array = ai_case->design.GetModelsByRoomNo(Room_NO);
		if (!model_array.size())
			continue;
		std::vector<ModelNode>	temp;
		for (auto &model : model_array) {
			//暂时排除橱柜,和水池等冲突
			if(model.id!= 30016)
				DisableModelRegion(&model);
			if (model_no.find(model.GetNo()) == model_no.end())
				continue;
			//模型覆盖节点区域
			auto ref = SearchNodeCoveredByModel(&model);
			auto iter = room_conrresponding_model.find(room.GetNo());
			ModelNode node;
			node.model = model.GetNo();
			node.cover_node = std::move(ref);
		    temp.push_back(node);
		}
		if (!temp.size())
			continue;
		auto iter = room_conrresponding_model.find(Room_NO);
		if (iter == room_conrresponding_model.end())
			room_conrresponding_model.insert(std::make_pair(Room_NO, std::move(temp)));
	}
}

NodeCoordinateArray MoveLinesEstimator::SearchNodeCoveredByModel(Model* model) {
	//丢弃非此model的model覆盖节点(E_Entity) 
	auto Box = CalculateModelWorldCoordinateBoundingBox(model);
	// 归一化节点坐标系
	auto maxf = Box[0];
	auto minf = Box[1];
	maxf = maxf - Min;
	minf = minf - Min;
	auto maxf_row = (unsigned)ceilf(maxf.y / granularity);
	auto maxf_column = (unsigned)ceilf(maxf.x / granularity);
	auto minf_row = (unsigned)ceilf(minf.y / granularity);
	auto minf_column = (unsigned)ceilf(minf.x / granularity);
	NodeCoordinateArray arrays;

	for(auto row=minf_row;row< maxf_row;++row)
		for (auto column = minf_column; column < maxf_column; ++column)
		{
			arrays.push_back(std::make_pair(row, column));
		}
	return arrays;
}

void	MoveLinesEstimator::DisableModelRegion(Model* model) {
	//丢弃非此model的model覆盖节点(E_Entity) 
	auto Box = CalculateModelWorldCoordinateBoundingBox(model);
	// 归一化节点坐标系
	auto maxf = Box[0];
	auto minf = Box[1];
	maxf = maxf - Min;
	minf = minf - Min;
	auto maxf_row = (unsigned)ceilf(maxf.y / granularity);
	auto maxf_column = (unsigned)ceilf(maxf.x / granularity);
	auto minf_row = (unsigned)ceilf(minf.y / granularity);
	auto minf_column = (unsigned)ceilf(minf.x / granularity);
	for (auto row = minf_row; row < maxf_row; ++row)
		for (auto column = minf_column; column < maxf_column; ++column)
		{
			if (column < max_column&&row < max_row)
				base_leaf_node[row][column].flag |= E_Entity;
		} 
}

void MoveLinesEstimator::OpenDoorNode(AICase*ai_case) {

	for (auto &room : ai_case->house.room_list) {
		AddNodeConstraitedByWall(&room);
		//RemoveNodeConstraitedByDoor(&room, ai_case);
	}

	for (auto &door : ai_case->house.door_list)
	{
		 
		auto pos1 = door.pos + door.direction*granularity*4;
		auto pos2 = door.pos -  door.direction*granularity * 4;
		auto node = CalculateNodeIndexOnCoordinate(pos1);
		auto node2= CalculateNodeIndexOnCoordinate(pos2);
		bool find_entity = false;
		if (node.x == node2.x)
		{
			auto max_ = max(node.y, node2.y);
			auto min_ = min(node.y, node2.y);
			for (int i = min_; i <= max_; ++i)
			{
				if(node.x>=0&& node.x<base_leaf_node.size())
					if (i >= 0 && i < base_leaf_node[node.x].size())
					{			
							base_leaf_node[node.x][i].flag &= ~E_Entity;			
							base_leaf_node[node.x][i].flag &= ~E_Outside;			 
					}
			}
		}

		if (node.y == node2.y)
		{
			auto max_ = max(node.x, node2.x);
			auto min_ = min(node.x, node2.x);
			for (int i = min_; i <= max_; ++i)
			{
				if (i >= 0 && i < base_leaf_node.size())
					if (node.y >= 0 && node.y < base_leaf_node[i].size())
					{
						base_leaf_node[i][node.y].flag &= ~E_Outside;
						base_leaf_node[i][node.y].flag &= ~E_Entity;
					}
			}
		}
	}
}

void MoveLinesEstimator::BuildRelationContainer() {
	std::set<Room_NO>	temp_room;
	for (auto &room : room_conrresponding_model)
	{
		RoomRelation relation;
		relation.beg_room = room.first;
		for (auto &room2 : room_conrresponding_model)
		{
			if (room.first == room2.first)
				continue;
			if (temp_room.find(room2.first) != temp_room.end())
				continue;
			relation.end_room = room2.first;
			m_room_container.push_back(relation);
		}
		temp_room.insert(room.first);
	}
}

bool MoveLinesEstimator::AddRouteRegionToRelationContainer(AICase*ai_case) { 
	for (auto &ref : m_room_container) {
		//选第一个区域内家具点进行路径搜索
		auto Model1 = ai_case->design.GetModelByNo(room_conrresponding_model.find(ref.beg_room)->second[0].model);
		auto Model2 = ai_case->design.GetModelByNo(room_conrresponding_model.find(ref.end_room)->second[0].model);
		RemoveNodeConstraitedByModel(Model1);
		RemoveNodeConstraitedByModel(Model2);
		if (!MinimumRouteModelWithModel(Model1, Model2))
			return false;
		CollectPathNodeToVector(Model2->location);
		auto AABB = CalculateModelWorldCoordinateBoundingBox(Model1);
		AddNodeConstraitedByAABB(AABB[0], AABB[1]);
		AABB = CalculateModelWorldCoordinateBoundingBox(Model2);
		AddNodeConstraitedByAABB(AABB[0], AABB[1]);
	
		//遍历所有的区域把,经过的区域放在pass region
		std::set<Room_NO> cache;//缓存已经路过的区域
		for (auto node : m_path) {
			auto point = GridCoordToWorldCoord(node.x, node.y);
			for (auto &room : ai_case->house.room_list) {
				//路径走过的区域
				if (cache.find(room.GetNo()) != cache.end())
					continue;
				if (ComMath::PointInPolygon(point, room.getPointList()))
				{
					if(ref.beg_room== room.GetNo()|| ref.end_room == room.GetNo())
						continue;
					//查询区域是否被添加进来,路径最好是顺序的
					ref.path_room.push_back(room.GetNo());
					cache.insert(room.GetNo());
				}
			}
		} 
	}
	return true;
}

bool MoveLinesEstimator::AddRouteDoorToRelationContaier(AICase*ai_case) {
	for (auto &ref : m_room_container) {
		auto Model1 = ai_case->design.GetModelByNo(room_conrresponding_model.find(ref.beg_room)->second[0].model);
		auto Model2 = ai_case->design.GetModelByNo(room_conrresponding_model.find(ref.end_room)->second[0].model);
		RemoveNodeConstraitedByModel(Model1);
		RemoveNodeConstraitedByModel(Model2);
		if (!MinimumRouteModelWithModel(Model1, Model2))
			return false;
		CollectPathNodeToVector(Model2->location);
		auto AABB = CalculateModelWorldCoordinateBoundingBox(Model1);
		AddNodeConstraitedByAABB(AABB[0], AABB[1]);
		AABB = CalculateModelWorldCoordinateBoundingBox(Model2);
		AddNodeConstraitedByAABB(AABB[0], AABB[1]);
 
		std::set<Door_NO>	cache;
		for (auto node : m_path) {
			auto point = GridCoordToWorldCoord(node.x, node.y);
			for (auto &door : ai_case->house.door_list) {
				if (door.door_type == D_SECURITY)
					continue;
				Point3f parallelDoor;
				auto angle = 90.f / 360.f*(3.1415926f * 2);
				auto x = cosf(angle)*door.direction.x - sinf(angle)*door.direction.y;
				parallelDoor.y = sinf(angle)*door.direction.x + cosf(angle)*door.direction.y;
				parallelDoor.x = x;
				parallelDoor = ComMath::Normalize(parallelDoor);
				auto point1 = door.pos + parallelDoor * (float)door.length*0.5;
				auto point2 = door.pos - parallelDoor * (float)door.length*0.5;

				Point3f points[4];
				points[0] = point1 + door.direction * granularity;
				points[1] = point1 - door.direction * granularity;
				points[2] = point2 + door.direction * granularity;
				points[3] = point2 - door.direction * granularity;
				auto maxf = Point3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
				auto minf = -maxf;
				for (int i = 0; i < 4; ++i) {
					maxf = ComMath::Point3fMax(maxf, points[i]);
					minf = ComMath::Point3fMin(minf, points[i]);
				}
				if(point.x>minf.x&&point.x<maxf.x&&point.y < maxf.y&&point.y>minf.y)
					if (cache.find(door.GetNo()) == cache.end())
					{
						cache.insert(door.GetNo());
						ref.path_door.push_back(door.GetNo());
					}
			}
		}
	}
	return true;
}

bool MoveLinesEstimator::BuildDoorWithRegionRelation(AICase*ai_case) {

	for (auto &room : ai_case->house.room_list)
	{
		auto room_no = room.GetNo();
		auto door_list = ai_case->house.GetDoorsByRoomNo(room_no);
		std::set<Door_NO> door_set;
		for (auto &door : door_list) {
			door_set.insert(door.GetNo());
		}
		//封闭区域,返回错误
		if (!door_set.size())
			return false;
		if(relation_room_door.find(room_no)== relation_room_door.end())
			relation_room_door.insert(std::make_pair(room_no, std::move(door_set)));
	}
	return true;
}

bool MoveLinesEstimator::FigureOutRelationPathWithAStart(AICase*ai_case) {
 
	for (auto &relation : m_room_container) { 
		if (!relation.path_door.size())
			return false;
		//检查起始门是不是被通过了
		bool beg_pass = IsDoorHasBeenPassed(relation.path_door[0], relation.beg_room);
		if (!beg_pass) {
			//检查此区域有没有其他的走线
			auto p_door = ai_case->house.GetDoorByDoorNo(relation.path_door[0]);
			if (!p_door)
				return false;
			auto Infomation = MoveLinesEstimator::SearchOtherPathByRoom(p_door->pos, relation.beg_room);
			//已经有走线通过,连接这个走线
			if (Infomation.get()) {

				auto node = SearchNearestNode(Infomation, p_door->pos);
				auto res =MinimumRouteModelWithModel(p_door->pos, node);
				if (!res)
					return res;
				CollectPathNodeToVector(node);
				CompressNode();
				AddNodePathToDoorPath(m_path, relation.path_door[0], relation.beg_room);
			}
			else
			{
				Room*room = ai_case->house.GetRoomByRoomNo(relation.beg_room);
				if (!room)
					return false;
				Model* model = SearchFarestModelWithDoor(p_door, room,ai_case);
				if (model) {
					RemoveNodeConstraitedByModel(model);
					auto res =MinimunRoute(p_door, model);
					if (!res)
						return false;
					CollectPathNodeToVector(model->location);
					CompressNode();
					AddNodePathToDoorPath(m_path, relation.path_door[0], relation.beg_room);
					auto AABB = CalculateModelWorldCoordinateBoundingBox(model);
					AddNodeConstraitedByAABB(AABB[0], AABB[1]);
					RemoveModel(model);
				}
			}
		}
		//中间区域节点
		//检查中间节点的个数是不是有效的
		if (relation.path_door.size() != relation.path_room.size() + 1)
			return false;
		for (auto i = 1; i < relation.path_door.size(); ++i) {
			 //先判断上个门,这个门被通过没?
			bool door1 = IsDoorHasBeenPassed(relation.path_door[i - 1], relation.path_room[i-1]);
			bool door2 = IsDoorHasBeenPassed(relation.path_door[i], relation.path_room[i-1]);
			if (door1&&door2)
				continue;
			auto p_door_i_1 = ai_case->house.GetDoorByDoorNo(relation.path_door[i - 1]);
			if (!p_door_i_1)
				return false;
			auto p_door_i = ai_case->house.GetDoorByDoorNo(relation.path_door[i]);
			if (!p_door_i)
				return false;
			auto Information = MoveLinesEstimator::SearchOtherPathByRoom(p_door_i_1->pos, relation.path_room[i - 1]);
			//此区域没有走线
			if (!Information.get())
			{	 	 
				if (!MinimumRouteModelWithModel(p_door_i_1->pos, p_door_i->pos))
					return false;
				CollectPathNodeToVector(p_door_i->pos);
				CompressNode();
				MoveLinesEstimator::PathInformation info = std::make_shared<std::vector<PathNode>>();
				*info = m_path;
				AddNodePathToDoorPath(info, relation.path_door[i - 1], relation.path_room[i - 1]);
				AddNodePathToDoorPath(info, relation.path_door[i], relation.path_room[i - 1]);
				continue;
			}
			if (!door1) {
				//检查此区域有没有其他的走线
				auto Information = MoveLinesEstimator::SearchOtherPathByRoom(p_door_i_1->pos, relation.path_room[i-1]);
				//已经有走线通过,连接这个走线
				if (Information.get()) {
					auto node = SearchNearestNode(Information, p_door_i_1->pos);
					//不需要解除节点限制
					if (!MinimumRouteModelWithModel(p_door_i_1->pos, node))
						return false;
					CollectPathNodeToVector(node);
					CompressNode();
					AddNodePathToDoorPath(m_path, relation.path_door[i - 1], relation.path_room[i-1]);
				}
			}
			else if (!door2) {
				//检查此区域有没有其他的走线
				auto Infomation = MoveLinesEstimator::SearchOtherPathByRoom(p_door_i->pos, relation.path_room[i-1]);
				//已经有走线通过,连接这个走线
				if (Infomation.get()) {
					auto node = SearchNearestNode(Infomation, p_door_i->pos);
					if (!MinimumRouteModelWithModel(p_door_i->pos, node))
						return false;
					CollectPathNodeToVector(node);
					CompressNode();
					AddNodePathToDoorPath(m_path, relation.path_door[i], relation.path_room[i - 1]);
				}
			}
		}
		bool end_pass = IsDoorHasBeenPassed(relation.path_door[relation.path_door.size()-1], relation.end_room);
		if (!end_pass) {
			//检查此区域有没有其他的走线
			auto p_door= ai_case->house.GetDoorByDoorNo(relation.path_door[relation.path_door.size() - 1]);
			auto Infomation = MoveLinesEstimator::SearchOtherPathByRoom(p_door->pos, relation.end_room);
			//已经有走线通过,连接这个走线
			if (Infomation.get()) {
				auto node = SearchNearestNode(Infomation, p_door->pos);
			 
				if (!MinimumRouteModelWithModel(p_door->pos, node))
					return false;
				CollectPathNodeToVector(node);
				CompressNode();
				AddNodePathToDoorPath(m_path, relation.path_door[relation.path_door.size() - 1], relation.end_room);
			}
			else
			{
				auto room = ai_case->house.GetRoomByRoomNo(relation.end_room);
				if (!room)
					return false;
				auto model = SearchFarestModelWithDoor(p_door, room, ai_case);
				if (!model)
					return false;
			 	if (model) {
			 		RemoveNodeConstraitedByModel(model);
					if (!MinimunRoute(p_door, model))
						return false;
			 		CollectPathNodeToVector(model->location);
			 	    CompressNode();
				 	AddNodePathToDoorPath(m_path, relation.path_door[relation.path_door.size() - 1], relation.end_room);
				 	auto AABB = CalculateModelWorldCoordinateBoundingBox(model);
				 	AddNodeConstraitedByAABB(AABB[0], AABB[1]);
					RemoveModel(model);
			 	}
			}
		}
	}
	return true;
}

bool MoveLinesEstimator::FindRouteWithRestModel(AICase*ai_case) {
	for (auto &pair : room_conrresponding_model)
	{
		for (auto model : pair.second)
		{
			if (pass_model.find(model.model) != pass_model.end())
				continue;
			auto p_model = ai_case->design.GetModelByNo(model.model);
			if (!p_model)
				return false;
			auto Infomation = MoveLinesEstimator::SearchOtherPathByRoom(p_model->location, pair.first);
			RemoveNodeConstraitedByModel(p_model);
			if (!Infomation.get())
				continue;
			auto node = SearchNearestNode(Infomation, p_model->location);
			if (!MinimumRouteModelWithModel(p_model->location, node))
				return false;
			CollectPathNodeToVector(node);
			CompressNode();
			AddNodePathToMinorPath(m_path);
			auto AABB = CalculateModelWorldCoordinateBoundingBox(p_model);
			AddNodeConstraitedByAABB(AABB[0], AABB[1]);
			pass_model.insert(model.model);
		}
	}
	return true;
}

bool MoveLinesEstimator::FindMoveLineWithOneRegion(AICase*ai_case) {
	//任意去一个门
	auto room = room_conrresponding_model.begin();
	auto door_list = ai_case->house.GetDoorsByRoomNo(room->first);
	if (!door_list.size())
		return false;
	//最远的和门走线
	auto p_room = ai_case->house.GetRoomByRoomNo(room->first);
	auto p_model = SearchFarestModelWithDoor(&door_list[0], p_room, ai_case);
	pass_model.insert(p_model->GetNo());
	RemoveNodeConstraitedByModel(p_model);
	if (!MinimumRouteModelWithModel(door_list[0].pos, p_model->location))
		return false;
	CollectPathNodeToVector(p_model->location);
	CompressNode();
	AddNodePathToMinorPath(m_path);

	auto major_path = m_path;
	PathInformation infor = std::make_shared<std::vector<PathNode>>();
	*infor = m_path;
	for (auto model : room_conrresponding_model)
	{
	  // 获取所有的
		auto p_model=SearchFarestModelWithDoor(&door_list[0], p_room, ai_case);
		if (!p_model)
			break;
		pass_model.insert(p_model->GetNo());
		auto node =SearchNearestNode(infor, p_model->location);
		auto beg = GridCoordToWorldCoord(node.x, node.y);
		RemoveNodeConstraitedByModel(p_model);
		MinimumRouteModelWithModel(door_list[0].pos, p_model->location);
		CollectPathNodeToVector(p_model->location);
		CompressNode();
		AddNodePathToMinorPath(m_path);
	}

	return true;
}

bool MoveLinesEstimator::IsDoorHasBeenPassed(Door_NO door, Room_NO room) {
	auto iter = m_door_path.find(door);
	if (iter == m_door_path.end())
		return false;
	if (iter->second.another_room != room && iter->second.room != room)
		return false;
	return true;
}
 
MoveLinesEstimator::PathInformation  MoveLinesEstimator::SearchOtherPathByRoom(Point3f Point, Room_NO room) {
	PathInformation out;   
	float maxf = FLT_MAX;
	for (auto &doorset : relation_room_door)
	{
		for (auto &doorpath : doorset.second) {
			auto iter = m_door_path.find(doorpath);
			if (iter == m_door_path.end())
				continue;
			//获取最近的走线
			PathInformation infor = nullptr;
			if (iter->second.room == room)
				infor =  iter->second.path_node;
			else if(iter->second.another_room == room)
				infor = iter->second.another_path;
			if (!infor)
				continue;
			//计算最短的距离
			for (auto &ref : *infor) {
				if (base_leaf_node[ref.x][ref.y].flag&E_Entity)
					continue;
				auto point =GridCoordToWorldCoord(ref.x, ref.y);
				auto dist=ComMath::getTwoPointDistance(point, Point);
				if (dist < maxf) {
					maxf = dist;
					out = infor;
				}
			}
		}
	}
	return out;
}
 
PathNode	MoveLinesEstimator::SearchNearestNode(PathInformation infor, Point3f Point) {
	PathNode node;
	float maxf = FLT_MAX;
	for (auto &ref : *infor) {
		if (base_leaf_node[ref.x][ref.y].flag&E_Entity)
			continue;
		auto point = GridCoordToWorldCoord(ref.x, ref.y);
		auto dist = ComMath::getTwoPointDistance(point, Point);
		if (dist < maxf) {
			maxf = dist;
			node = ref;
		}
	}
	return node;
}

Model* MoveLinesEstimator::SearchFarestModelWithDoor(Door*door, Room*room, AICase*ai_case) {
	Model*model = nullptr;
	auto iter = room_conrresponding_model.find(room->GetNo());
	if (iter == room_conrresponding_model.end())
		return model;
	float Max = FLT_MIN;
	for (auto model_ : iter->second) {
		if (pass_model.find(model_.model) != pass_model.end())
			continue;
		auto p_model = ai_case->design.GetModelByNo(model_.model);
		auto dost=ComMath::getTwoPointDistance(door->pos, p_model->location);
		if (dost > Max) {
			Max = dost;
			model = p_model;
		}
	}
	return model;
}

void MoveLinesEstimator::AddNodePathToDoorPath(VectorCoordinate&Ref, Door_NO door, Room_NO room) {
	auto iter = m_door_path.find(door);
	auto node = std::make_shared<std::vector<PathNode>>();
	*node = Ref;
	if (iter == m_door_path.end())
	{
		PathOfDoor pathOfDoor;
		pathOfDoor.room = room;
		pathOfDoor.path_node = node;
		m_door_path.insert(std::make_pair(door,pathOfDoor));
	}
	else {
		if (!iter->second.another_room.size()) {
			iter->second.another_room = room;
			iter->second.another_path = node;
		}
		else
		{
			iter->second.room = room;
			iter->second.path_node = node;
		}
	}
}

void MoveLinesEstimator::AddNodePathToDoorPath(PathInformation infor, Door_NO door, Room_NO room) {

	auto iter = m_door_path.find(door);
	if (iter == m_door_path.end())
	{
		PathOfDoor pathOfDoor;
		pathOfDoor.room = room;
		pathOfDoor.path_node = infor;
		m_door_path.insert(std::make_pair(door, pathOfDoor));
	}
	else {
		if (!iter->second.another_room.size()) {
			iter->second.another_room = room;
			iter->second.another_path = infor;
		}
		else
		{
			iter->second.room = room;
			iter->second.path_node = infor;
		}
	}
}

void MoveLinesEstimator::AddNodePathToMinorPath(VectorCoordinate&Ref) {
	auto new_path = Ref;
	minor_path.push_back(std::move(new_path));
}

void MoveLinesEstimator::ClearCloseFlag() {
	for(auto &ref:base_leaf_node)
		for (auto &node : ref) {
			node.flag &= ~E_Close;
		}
}

bool MoveLinesEstimator::MinimunRoute(Door*door, Model*Model) {
	ClearCloseFlag();
	bool b_path = false;   
	Point3f model_pos = Model->location;
	Point3f door_pos = door->pos;
	std::set<PathNode>			OpenMsg;
	auto father =CalculateNodeIndexOnCoordinate(door_pos);
	auto destination = CalculateNodeIndexOnCoordinate(model_pos);
	auto destAddr = GetNodeAddr(destination);
 
	SetZeroNode(father, destination);
	OpenMsg.insert(father);
	//关闭 door_close
	VectorCoordinate vector;
	while (OpenMsg.size())
	{
		//获取最小的权重 (i,j) father node
		PathNode*Node = nullptr;
		float min = FLT_MAX;
		for (auto &open : OpenMsg){
			if (open.score < min)
			{
				Node = (PathNode*)&open;
				min = open.score;
			 }
		}
		if (GetNodeAddr(*Node) == destAddr) {
			b_path = true;
			break;
		}
		vector.clear();
		CalculateSubNode((PathNode)*Node, vector, base_leaf_node);
		for (auto ref : vector) {
			std::set<PathNode>::iterator &iter = OpenMsg.find(ref);
			if (iter!= OpenMsg.end()) {
				auto addr = GetNodeAddr(*iter);
				float dist=CalculateCostOpen(*iter, *Node);
				if (dist < iter->score) {
					PathNode &ref = (PathNode)*iter;
					ResetFatherNodeOpen(ref, *Node);
					 ref.score = dist;
				}
			}
			else {
				auto new_node = CalculateNodeToOpenTable(ref, *Node, destination);
				OpenMsg.insert(new_node);
			}
		}
		SetNodeToClose(Node->x, Node->y);
		OpenMsg.erase(*Node);
	}
	return b_path;
}

bool MoveLinesEstimator::MinimumRouteModelWithModel(Model*beg, Model*end) {
	ClearCloseFlag();
	bool b_path = false;
	std::set<PathNode>			OpenMsg;
	auto father = CalculateNodeIndexOnCoordinate(beg->location);
	auto destination = CalculateNodeIndexOnCoordinate(end->location);
	auto destAddr = GetNodeAddr(destination);

	SetZeroNode(father, destination);
	OpenMsg.insert(father);
	//关闭 door_close
	VectorCoordinate vector;
	while (OpenMsg.size())
	{
		//获取最小的权重 (i,j) father node
		PathNode*Node = nullptr;
		float min = FLT_MAX;
		for (auto &open : OpenMsg) {
			if (open.score < min)
			{
				Node = (PathNode*)&open;
				min = open.score;
			}
		}
		if (GetNodeAddr(*Node) == destAddr) {
			b_path = true;
			break;
		}
		vector.clear();
		CalculateSubNode((PathNode)*Node, vector, base_leaf_node);
		for (auto ref : vector) {
			std::set<PathNode>::iterator &iter = OpenMsg.find(ref);
			if (iter != OpenMsg.end()) {
				auto addr = GetNodeAddr(*iter);
				float dist = CalculateCostOpen(*iter, *Node);
				if (dist < iter->score) {
					PathNode &ref = (PathNode)*iter;
					ResetFatherNodeOpen(ref, *Node);
					ref.score = dist;
				}
			}
			else {
				auto new_node = CalculateNodeToOpenTable(ref, *Node, destination);
				OpenMsg.insert(new_node);
			}
		}
		SetNodeToClose(Node->x, Node->y);
		OpenMsg.erase(*Node);
	}
	return b_path;
}

bool MoveLinesEstimator::MinimumRouteModelWithModel(Point3f beg, Point3f end) {
	ClearCloseFlag();
	bool b_path = false;
	std::set<PathNode>			OpenMsg;
	auto father = CalculateNodeIndexOnCoordinate(beg);
	auto destination = CalculateNodeIndexOnCoordinate(end);
	auto destAddr = GetNodeAddr(destination);

	SetZeroNode(father, destination);
	OpenMsg.insert(father);
	//关闭 door_close
	VectorCoordinate vector;
	while (OpenMsg.size())
	{
		//获取最小的权重 (i,j) father node
		PathNode*Node = nullptr;
		float min = FLT_MAX;
		for (auto &open : OpenMsg) {
			if (open.score < min)
			{
				Node = (PathNode*)&open;
				min = open.score;
			}
		}
		if (GetNodeAddr(*Node) == destAddr) {
			b_path = true;
			break;
		}
		vector.clear();
		CalculateSubNode((PathNode)*Node, vector, base_leaf_node);
		for (auto ref : vector) {
			std::set<PathNode>::iterator &iter = OpenMsg.find(ref);
			if (iter != OpenMsg.end()) {
				auto addr = GetNodeAddr(*iter);
				float dist = CalculateCostOpen(*iter, *Node);
				if (dist < iter->score) {
					PathNode &ref = (PathNode)*iter;
					ResetFatherNodeOpen(ref, *Node);
					ref.score = dist;
				}
			}
			else {
				auto new_node = CalculateNodeToOpenTable(ref, *Node, destination);
				OpenMsg.insert(new_node);
			}
		}
		SetNodeToClose(Node->x, Node->y);
		OpenMsg.erase(*Node);
	}
	return b_path;
}

bool MoveLinesEstimator::MinimumRouteModelWithModel(Point3f beg, PathNode node) {
	ClearCloseFlag();
	bool b_path = false;
	std::set<PathNode>			OpenMsg;
	auto father = CalculateNodeIndexOnCoordinate(beg);
	auto destination = node;
	auto destAddr = GetNodeAddr(destination);
	SetZeroNode(father, destination);
	OpenMsg.insert(father);
	//关闭 door_close
	VectorCoordinate vector;
	while (OpenMsg.size())
	{
		//获取最小的权重 (i,j) father node
		PathNode*Node = nullptr;
		float min = FLT_MAX;
		for (auto &open : OpenMsg) {
			if (open.score < min)
			{
				Node = (PathNode*)&open;
				min = open.score;
			}
		}
		if (GetNodeAddr(*Node) == destAddr) {
			b_path = true;
			break;
		}
		vector.clear();
		CalculateSubNode((PathNode)*Node, vector, base_leaf_node);
		for (auto ref : vector) {
			std::set<PathNode>::iterator &iter = OpenMsg.find(ref);
			if (iter != OpenMsg.end()) {
				auto addr = GetNodeAddr(*iter);
				float dist = CalculateCostOpen(*iter, *Node);
				if (dist < iter->score) {
					PathNode &ref = (PathNode)*iter;
					ResetFatherNodeOpen(ref, *Node);
					ref.score = dist;
				}
			}
			else {
				auto new_node = CalculateNodeToOpenTable(ref, *Node, destination);
				OpenMsg.insert(new_node);
			}
		}
		SetNodeToClose(Node->x, Node->y);
		OpenMsg.erase(*Node);
	}
	return b_path;



}

bool MoveLinesEstimator::IsCompatibleNode(PathNode &destination) {
	if (destination.x < 0 || destination.x >= (int)max_row)
		return false;
	if (destination.y < 0 || destination.y >= (int)max_column)
		return false;
	//非实体
	if (base_leaf_node[destination.x][destination.y].flag&E_Entity)
		return false;
	//非外面
	if (base_leaf_node[destination.x][destination.y].flag&E_Outside)
		return false;
	//打开的
	if (base_leaf_node[destination.x][destination.y].flag&E_Close)
		return false;
	return true;
}

void MoveLinesEstimator::CalculateSubNode(PathNode &destination,VectorCoordinate&set, CoverNode&procedural) {
	auto up = PathNode(destination.x, destination.y +1);
	auto down = PathNode(destination.x, destination.y - 1);
	auto left	=	 PathNode(destination.x-1, destination.y);
	auto right = PathNode(destination.x+1, destination.y);
	//四个方向子节点，不包括斜着方向
	if (IsCompatibleNode(up))
		set.push_back(up);
	if (IsCompatibleNode(down))
		set.push_back(down);
	if (IsCompatibleNode(left))
		set.push_back(left);
	if (IsCompatibleNode(right))
		set.push_back(right);
}

void MoveLinesEstimator::SetZeroNode(PathNode &node,const PathNode &destNode) {
	auto &ref = base_leaf_node[node.x][node.y];
	auto &dest_ref= base_leaf_node[destNode.x][destNode.y];
	ref.travel_cost = 0;
	ref.disination_cost = (float)((node.x - destNode.x)*(node.x - destNode.x) + (node.y - destNode.y)* (node.y - destNode.y));
	ref.father_x = node.x;
	ref.father_y = node.y;
	node.score = ref.travel_cost + ref.disination_cost;
}

float MoveLinesEstimator::CalculateCostOpen(const PathNode &node, const PathNode &Father) {
	auto &ref = base_leaf_node[node.x][node.y];
	auto &father = base_leaf_node[Father.x][Father.y];
	float out = father.travel_cost + 1;
	out += ref.disination_cost;
	return out;
}

void MoveLinesEstimator::ResetFatherNodeOpen(PathNode &node, const PathNode&Father) {
	auto &ref = base_leaf_node[node.x][node.y];
	auto &father = base_leaf_node[Father.x][Father.y];
	ref.father_x = Father.x;
	ref.father_y = Father.y;
	ref.travel_cost = father.travel_cost + 1.f;
}

PathNode  MoveLinesEstimator::CalculateNodeToOpenTable(const PathNode&child, const PathNode&Father, const PathNode&Dist) {
	auto &ref = base_leaf_node[child.x][child.y];
	auto &father = base_leaf_node[Father.x][Father.y];
	ref.father_x = Father.x;
	ref.father_y = Father.y;
	ref.travel_cost = father.travel_cost + 1.f;
	ref.disination_cost =(float)((child.x - Dist.x)*(child.x - Dist.x) + (child.y - Dist.y)* (child.y - Dist.y));
	PathNode node = child;
	node.score = ref.travel_cost + ref.disination_cost;
	return node;
}

void MoveLinesEstimator::SearchCandidateNode(VectorCoordinate&vec, CoverNode&procedural, PathNode &destination) {
	auto &set = *vec.begin();
	auto up = PathNode(set.x, set.y+1);
	auto down = PathNode(set.x, set.y - 1);
	auto left = PathNode(set.x-1, set.y);
	auto right = PathNode(set.x+1, set.y);
	if (IsCompatibleNode(up)) {
		procedural[up.x][up.y].father_x = set.x;
		procedural[up.x][up.y].father_y = set.y;
	}
	if (IsCompatibleNode(down)) {
		procedural[down.x][down.y].father_x = set.x;
		procedural[down.x][down.y].father_y = set.y;
	}
	if (IsCompatibleNode(left)) {
		procedural[left.x][left.y].father_x = set.x;
		procedural[left.x][left.y].father_y = set.y;
	}
	if (IsCompatibleNode(right)) {
		procedural[right.x][right.y].father_x = set.x;
		procedural[right.x][right.y].father_y = set.y;
	}
}

void MoveLinesEstimator::CollectPathNodeToVector(PathNode end) {
	m_temp_path.clear();
	m_path.clear();
	auto &node = end;
	//每个节点都回储存父节点的地址,最后一个
	while (true)
	{
		m_temp_path.push_back(node);
		auto new_node = PathNode(base_leaf_node[node.x][node.y].father_x, base_leaf_node[node.x][node.y].father_y);
		if (node.x == new_node.x&&node.y == new_node.y)
			break;
		node = new_node;
	}
	for (int i = int(m_temp_path.size() - 1); i >= 0; --i)
	{
		m_path.push_back(m_temp_path[i]);
	}
}
 
void MoveLinesEstimator::CollectPathNodeToVector(Point3f &end) {
	m_temp_path.clear();
	m_path.clear();
	auto node = CalculateNodeIndexOnCoordinate(end);
	//每个节点都回储存父节点的地址,最后一个
	while (true)
	{
		m_temp_path.push_back(node);
		auto new_node = PathNode(base_leaf_node[node.x][node.y].father_x, base_leaf_node[node.x][node.y].father_y);
		if (node.x == new_node.x&&node.y == new_node.y)
			break;
		node = new_node;
	}
	for (int i = int(m_temp_path.size() - 1); i >= 0; --i) 
	{
		m_path.push_back(m_temp_path[i]);
	}
}

void MoveLinesEstimator::CompressNode() {

	if (!m_path.size())
		return;
	std::vector<std::pair<OrientationNode,unsigned>>	 set;
	for (unsigned i = 0; i < m_path.size();++ i)
	{
		if (i == 0) {
			continue;
		}
		auto &last = m_path[i - 1];
		auto orientation = MakeSureOrientationOfTurningPoint(m_path[i - 1], m_path[i]);
		//1.set 不为空,最后一个节点和新节点同方向,替换
		if (set.size() == 0) {
			set.push_back(std::make_pair(orientation, i - 1));
			continue;
		}
		if (set.size() == 1) {
			if(set[set.size() - 1].first == orientation)
				set[set.size() - 1].second = i - 1;
			else
				set.push_back(std::make_pair(orientation, i - 1));
			continue;
		}
		if (set.size()>1 && set[set.size() - 1].first != orientation) {
			set.push_back(std::make_pair(orientation, i - 1));
		}
 
		//A*特殊性,会出现ABA 和ABC的情况
		if (set.size() == 4)
		{
			//ABA,Shrink 节点
			if (set[1].first == set[3].first&&set[0].first==set[2].first) 
			{
				unsigned index = 0;
				bool canbeshrink = MakeSureCanBeCopressed(set[0],set[1], set[3], &index);
				if (canbeshrink)
				{
					i = index-1;
					set.clear();
					continue;
				}
				else
				{
					auto node1 = set[1];
					auto node2 = set[2];
					auto node3 = set[3];
					set.clear();
					set.push_back(node1);
					set.push_back(node2);
					set.push_back(node3);
				}
			}
			//ABC,移除第一个节点
			else {
				auto node1 = set[1];
				auto node2 = set[2];
				auto node3 = set[3];
				set.clear();
				set.push_back(node1);
				set.push_back(node2);
				set.push_back(node3);
			}
		}
	}
}

OrientationNode MoveLinesEstimator::MakeSureOrientationOfTurningPoint(PathNode&Node1, PathNode&Node2) {

	if (Node1.x == Node2.x){
		if (Node2.y > Node1.y)
			return OrientationNode::Right;
		else if (Node2.y < Node1.y)  
			return OrientationNode::Left;
	}
	if(Node2.y==Node1.y)
		if(Node2.x>Node1.x)
			return OrientationNode::Up;
		else if(Node2.x<Node1.x)
			return  OrientationNode::Down;

	return  OrientationNode::None;
}

bool	MoveLinesEstimator::MakeSureCanBeCopressed(OrientationNodePair&First,OrientationNodePair&Left, OrientationNodePair&Right,unsigned *out) {
 
	if (Left.first == OrientationNode::Right&&First.first == Up)
	{
		/*
		1.	/\	---------@
			 |                *
			 @**********
		*/
		//检查列的遮挡
		for (auto x = m_path[Left.second].x + 1; x < m_path[Right.second].x; ++x)
		{
			auto y = m_path[Left.second].y;
			PathNode node(x, y);
			if(!IsCompatibleNode(node))
				return false;
		}
		//检查行的遮挡
		for (auto y = m_path[Left.second].y + 1; y < m_path[Right.second].y; ++y)
		{
			auto x = m_path[Right.second].x;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
		//检查角落遮挡
		PathNode node(m_path[Right.second].x, m_path[Left.second].y);
		if (!IsCompatibleNode(node))
			return false;
		//无遮挡直接修改节点属性,从下个节点开始
		unsigned index = Left.second + 1;
		for (auto x = m_path[Left.second].x + 1; x < m_path[Right.second].x; ++x)
		{
			auto y = m_path[Left.second].y;
			m_path[index].x = x;
			m_path[index].y = y;
			++index;
		}
		m_path[index].x = m_path[Right.second].x;
		m_path[index].y = m_path[Left.second].y;
		*out = index;
		++index;
		for (auto y = m_path[Left.second].y + 1; y < m_path[Right.second].y; ++y)
		{
			auto x = m_path[Right.second].x;
			m_path[index].x = x;
			m_path[index].y = y;
			index++;
		}
		return true;
	}
	/*
	2. @	---------- /\
		*					|
		***********  @
	*/
	else  if (Left.first == OrientationNode::Left&&First.first == OrientationNode::Up) {
 
		for (auto x = m_path[Left.second].x + 1; x < m_path[Right.second].x; ++x)
		{ 
			auto y = m_path[Left.second].y;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
		for (auto y = m_path[Left.second].y - 1; y > m_path[Right.second].y; --y)
		{
			auto x = m_path[Right.second].x;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
		PathNode node(m_path[Right.second].x, m_path[Left.second].y);
		if (!IsCompatibleNode(node))
			return false;

		unsigned index = Left.second + 1;
		for (auto x = m_path[Left.second].x + 1; x < m_path[Right.second].x; ++x)
		{
			auto y = m_path[Left.second].y;
			m_path[index].x = x;
			m_path[index].y = y;
			++index;
		}
		m_path[index].x = m_path[Right.second].x;
		m_path[index].y = m_path[Left.second].y;
		*out = index;
		++index;
		for (auto y = m_path[Left.second].y - 1; y > m_path[Right.second].y; --y)
		{
			auto x = m_path[Right.second].x;
			m_path[index].x = x;
			m_path[index].y = y;
			index++;
		}
		return true;
	}
	else if (Left.first == OrientationNode::Right&&First.first == OrientationNode::Down) {
		/*
			3.	 @**********
				  |                *
				 \/	----------@
		*/
 
			for (auto x = m_path[Left.second].x - 1; x > m_path[Right.second].x; --x)
			{
				auto y = m_path[Left.second].y;
				PathNode node(x, y);
				if (!IsCompatibleNode(node))
					return false;
			}
			for (auto y = m_path[Left.second].y + 1; y < m_path[Right.second].y; ++y)
			{
				auto x = m_path[Right.second].x;
				PathNode node(x, y);
				if (!IsCompatibleNode(node))
					return false;
			}
	 
			PathNode node(m_path[Right.second].x, m_path[Left.second].y);
			if (!IsCompatibleNode(node))
				return false;

			unsigned index = Left.second + 1;
			for (auto x = m_path[Left.second].x - 1; x > m_path[Right.second].x; --x)
			{
				auto y = m_path[Left.second].y;
				m_path[index].x = x;
				m_path[index].y = y;
				++index;
			}
			m_path[index].x = m_path[Right.second].x;
			m_path[index].y = m_path[Left.second].y;
			*out = index;
			++index;
			for (auto y = m_path[Left.second].y + 1; y < m_path[Right.second].y; ++y)
			{
				auto x = m_path[Right.second].x;
				m_path[index].x = x;
				m_path[index].y = y;
				++index;
			}
			return true;
	}
	else if (Left.first == OrientationNode::Left&&First.first == OrientationNode::Down) {
	/*
			4.						@
									 |
				@	-----------\/
		*/


		for (auto x = m_path[Left.second].x - 1; x > m_path[Right.second].x; --x)
		{
			auto y = m_path[Left.second].y;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
		for (auto y = m_path[Left.second].y - 1; y > m_path[Right.second].y; --y)
		{
			auto x = m_path[Right.second].x;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
 
		PathNode node(m_path[Right.second].x, m_path[Left.second].y);
		if (!IsCompatibleNode(node))
			return false;
		unsigned index = Left.second + 1;
		for (auto x = m_path[Left.second].x - 1; x > m_path[Left.second].x; --x)
		{
			auto y = m_path[Left.second].y;
			m_path[index].x = x;
			m_path[index].y = y;
			++index;
		}
		*out = index;
		m_path[index].x = m_path[Right.second].x;
		m_path[index].y = m_path[Left.second].y;
		++index;
		for (auto y = m_path[Left.second].y - 1; y > m_path[Right.second].y; --y)
		{
			auto x = m_path[Right.second].x;
			m_path[index].x = x;
			m_path[index].y = y;
			++index;
		}
		return true;
}
	else if (Left.first == OrientationNode::Down&&First.first == OrientationNode::Right)
	{
		/*      
			5.	@----------->
				 *					|
				*	********* @
		*/
		 
			for (auto y = m_path[Left.second].y + 1; y < m_path[Right.second].y; ++y)
			{
				auto x = m_path[Left.second].x;
				PathNode node(x, y);
				if (!IsCompatibleNode(node))
					return false;
			}
			for (auto x = m_path[Left.second].x - 1; x > m_path[Right.second].x; --x)
			{
				auto y = m_path[Right.second].y;
				PathNode node(x, y);
				if (!IsCompatibleNode(node))
					return false;
			}

			PathNode node(m_path[Left.second].x, m_path[Right.second].y);
			if (!IsCompatibleNode(node))
				return false;

			unsigned index = Left.second + 1;
			for (auto y = m_path[Left.second].y + 1; y < m_path[Right.second].y; ++y)
			{
				auto x = m_path[Left.second].x;
				m_path[index].x = x;
				m_path[index].y = y;
				++index;
			}
			m_path[index].x = m_path[Left.second].x;
			m_path[index].y = m_path[Right.second].y;
			*out = index;
			++index;
			for (auto x = m_path[Left.second].x - 1; x > m_path[Right.second].x; --x)
			{
				auto y = m_path[Right.second].y;
				m_path[index].x = x;
				m_path[index].y = y;
				++index;
			}
		return true;
	}
	else if (Left.first == OrientationNode::Up&&First.first == OrientationNode::Right) {
		/*
		6					@
							 |
		@---------->
	*/

		for (auto y = m_path[Left.second].y + 1; y < m_path[Right.second].y; ++y)
		{
			auto x = m_path[Left.second].x;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
		for (auto x = m_path[Left.second].x + 1; x < m_path[Right.second].x; ++x)
		{
			auto y = m_path[Right.second].y;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
		PathNode node(m_path[Left.second].x, m_path[Right.second].y);
		if (!IsCompatibleNode(node))
			return false;
		unsigned index = Left.second + 1;
		for (auto y = m_path[Left.second].y + 1; y < m_path[Right.second].y; ++y)
		{
			auto x = m_path[Left.second].x;
			m_path[index].x = x;
			m_path[index].y = y;
			++index;
		}
		m_path[index].x = m_path[Left.second].x;
		m_path[index].y = m_path[Right.second].y;
		*out = index;
		++index;
		for (auto x = m_path[Left.second].x + 1; x < m_path[Right.second].x; ++x)
		{
			auto y = m_path[Right.second].y;
			m_path[index].x = x;
			m_path[index].y = y;
			++index;

		}
		return true;
	}
	else if (Left.first == OrientationNode::Down&&First.first == OrientationNode::Left)
	{
		/*
			7.	<-------------@
				|
				@************
		*/
		for (auto y = m_path[Left.second].y - 1; y > m_path[Right.second].y; --y)
		{
			auto x = m_path[Left.second].x;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
		for (auto x = m_path[Left.second].x - 1; x > m_path[Right.second].x; --x)
		{
			auto y = m_path[Right.second].y;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}

		PathNode node(m_path[Left.second].x, m_path[Right.second].y);
		if (!IsCompatibleNode(node))
			return false;

		unsigned index = Left.second + 1;
		for (auto y = m_path[Left.second].y - 1; y > m_path[Right.second].y; --y)
		{
			auto x = m_path[Left.second].x;
			m_path[index].x = x;
			m_path[index].y = y;
			++index;
		}
		m_path[index].x = m_path[Left.second].x;
		m_path[index].y = m_path[Right.second].y;
		*out = index;
		++index;
		for (auto x = m_path[Left.second].x - 1; x > m_path[Right.second].x; --x)
		{
			auto y = m_path[Right.second].y;
			m_path[index].x = x;
			m_path[index].y = y;
			++index;
		}
		return true;
	}
	else if (Left.first == OrientationNode::Up&&First.first == OrientationNode::Left) {
		/*
				8.	 @
					  |
					 <-----------@
			*/
		for (auto y = m_path[Left.second].y - 1; y > m_path[Right.second].y; --y)
		{
			auto x = m_path[Left.second].x;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}

		for (auto x = m_path[Left.second].x + 1; x < m_path[Right.second].x; ++x)
		{
			auto y = m_path[Right.second].y;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}

		PathNode node(m_path[Left.second].x, m_path[Right.second].y);
		if (!IsCompatibleNode(node))
			return false;

		unsigned index = Left.second + 1;
		for (auto y = m_path[Left.second].y - 1; y > m_path[Right.second].y; --y)
		{
			auto x = m_path[Left.second].x;
			m_path[index].x = y;
			m_path[index].y = y;

			++index;
		}
		m_path[index].x = m_path[Left.second].x;
		m_path[index].y = m_path[Right.second].y;
		*out = index;
		++index;
		for (auto x = m_path[Left.second].x + 1; x < m_path[Right.second].x; ++x)
		{
			auto y = m_path[Right.second].y;
			m_path[index].x = x;
			m_path[index].y = y;
			++index;
		}
		return true;
	}
	return false;
}

bool MoveLinesEstimator::MakeSureCanBeCopressedEnd(OrientationNodePair&First, OrientationNodePair&Left, OrientationNodePair&Right, unsigned *out) {
	if (Left.first == OrientationNode::Right&&First.first == Up)
	{
		/*
		1.	/\	---------@
			 |                *
			 @**********
		*/
		//检查列的遮挡
		for (auto x = m_temp_path[Left.second].x + 1; x < m_temp_path[Right.second].x; ++x)
		{
			auto y = m_temp_path[Left.second].y;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
		//检查行的遮挡
		for (auto y = m_temp_path[Left.second].y + 1; y < m_temp_path[Right.second].y; ++y)
		{
			auto x = m_temp_path[Right.second].x;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
		//检查角落遮挡
		PathNode node(m_temp_path[Right.second].x, m_temp_path[Left.second].y);
		if (!IsCompatibleNode(node))
			return false;
		//无遮挡直接修改节点属性,从下个节点开始
		unsigned index = Left.second + 1;
		for (auto x = m_temp_path[Left.second].x + 1; x < m_temp_path[Right.second].x; ++x)
		{
			auto y = m_temp_path[Left.second].y;
			m_temp_path[index].x = x;
			m_temp_path[index].y = y;
			++index;
		}
		m_temp_path[index].x = m_temp_path[Right.second].x;
		m_temp_path[index].y = m_temp_path[Left.second].y;
		*out = index;
		++index;
		for (auto y = m_temp_path[Left.second].y + 1; y < m_temp_path[Right.second].y; ++y)
		{
			auto x = m_temp_path[Right.second].x;
			m_temp_path[index].x = x;
			m_temp_path[index].y = y;
			index++;
		}
		return true;
	}
	/*
	2. @	---------- /\
		*					|
		***********  @
	*/
	else  if (Left.first == OrientationNode::Left&&First.first == OrientationNode::Up) {

		for (auto x = m_temp_path[Left.second].x + 1; x < m_temp_path[Right.second].x; ++x)
		{
			auto y = m_temp_path[Left.second].y;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
		for (auto y = m_temp_path[Left.second].y - 1; y > m_temp_path[Right.second].y; --y)
		{
			auto x = m_temp_path[Right.second].x;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
		PathNode node(m_temp_path[Right.second].x, m_temp_path[Left.second].y);
		if (!IsCompatibleNode(node))
			return false;

		unsigned index = Left.second + 1;
		for (auto x = m_temp_path[Left.second].x + 1; x < m_temp_path[Right.second].x; ++x)
		{
			auto y = m_temp_path[Left.second].y;
			m_temp_path[index].x = x;
			m_temp_path[index].y = y;
			++index;
		}
		m_temp_path[index].x = m_temp_path[Right.second].x;
		m_temp_path[index].y = m_temp_path[Left.second].y;
		*out = index;
		++index;
		for (auto y = m_temp_path[Left.second].y - 1; y > m_temp_path[Right.second].y; --y)
		{
			auto x = m_temp_path[Right.second].x;
			m_temp_path[index].x = x;
			m_temp_path[index].y = y;
			index++;
		}
		return true;
	}
	else if (Left.first == OrientationNode::Right&&First.first == OrientationNode::Down) {
		/*
			3.	 @**********
				  |                *
				 \/	----------@
		*/

		for (auto x = m_temp_path[Left.second].x - 1; x > m_temp_path[Right.second].x; --x)
		{
			auto y = m_temp_path[Left.second].y;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
		for (auto y = m_temp_path[Left.second].y + 1; y < m_temp_path[Right.second].y; ++y)
		{
			auto x = m_temp_path[Right.second].x;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}

		PathNode node(m_temp_path[Right.second].x, m_temp_path[Left.second].y);
		if (!IsCompatibleNode(node))
			return false;

		unsigned index = Left.second + 1;
		for (auto x = m_temp_path[Left.second].x - 1; x > m_temp_path[Right.second].x; --x)
		{
			auto y = m_temp_path[Left.second].y;
			m_temp_path[index].x = x;
			m_temp_path[index].y = y;
			++index;
		}
		m_temp_path[index].x = m_temp_path[Right.second].x;
		m_temp_path[index].y = m_temp_path[Left.second].y;
		*out = index;
		++index;
		for (auto y = m_temp_path[Left.second].y + 1; y < m_temp_path[Right.second].y; ++y)
		{
			auto x = m_temp_path[Right.second].x;
			m_temp_path[index].x = x;
			m_temp_path[index].y = y;
			++index;
		}
		return true;
	}
	else if (Left.first == OrientationNode::Left&&First.first == OrientationNode::Down) {
		/*
				4.						@
										 |
					@	-----------\/
			*/


		for (auto x = m_temp_path[Left.second].x - 1; x > m_temp_path[Right.second].x; --x)
		{
			auto y = m_temp_path[Left.second].y;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
		for (auto y = m_temp_path[Left.second].y - 1; y > m_temp_path[Right.second].y; --y)
		{
			auto x = m_temp_path[Right.second].x;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}

		PathNode node(m_temp_path[Right.second].x, m_temp_path[Left.second].y);
		if (!IsCompatibleNode(node))
			return false;
		unsigned index = Left.second + 1;
		for (auto x = m_temp_path[Left.second].x - 1; x > m_temp_path[Left.second].x; --x)
		{
			auto y = m_temp_path[Left.second].y;
			m_temp_path[index].x = x;
			m_temp_path[index].y = y;
			++index;
		}
		*out = index;
		m_temp_path[index].x = m_temp_path[Right.second].x;
		m_temp_path[index].y = m_temp_path[Left.second].y;
		++index;
		for (auto y = m_temp_path[Left.second].y - 1; y > m_temp_path[Right.second].y; --y)
		{
			auto x = m_temp_path[Right.second].x;
			m_temp_path[index].x = x;
			m_temp_path[index].y = y;
			++index;
		}
		return true;
	}
	else if (Left.first == OrientationNode::Down&&First.first == OrientationNode::Right)
	{
		/*
			5.	@----------->
				 *					|
				*	********* @
		*/

		for (auto y = m_temp_path[Left.second].y + 1; y < m_temp_path[Right.second].y; ++y)
		{
			auto x = m_temp_path[Left.second].x;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
		for (auto x = m_temp_path[Left.second].x - 1; x > m_temp_path[Right.second].x; --x)
		{
			auto y = m_temp_path[Right.second].y;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}

		PathNode node(m_temp_path[Left.second].x, m_temp_path[Right.second].y);
		if (!IsCompatibleNode(node))
			return false;

		unsigned index = Left.second + 1;
		for (auto y = m_temp_path[Left.second].y + 1; y < m_temp_path[Right.second].y; ++y)
		{
			auto x = m_temp_path[Left.second].x;
			m_temp_path[index].x = x;
			m_temp_path[index].y = y;
			++index;
		}
		m_temp_path[index].x = m_temp_path[Left.second].x;
		m_temp_path[index].y = m_temp_path[Right.second].y;
		*out = index;
		++index;
		for (auto x = m_temp_path[Left.second].x - 1; x > m_temp_path[Right.second].x; --x)
		{
			auto y = m_temp_path[Right.second].y;
			m_temp_path[index].x = x;
			m_temp_path[index].y = y;
			++index;
		}
		return true;
	}
	else if (Left.first == OrientationNode::Up&&First.first == OrientationNode::Right) {
		/*
		6					@
							 |
		@---------->
	*/

		for (auto y = m_temp_path[Left.second].y + 1; y < m_temp_path[Right.second].y; ++y)
		{
			auto x = m_temp_path[Left.second].x;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
		for (auto x = m_temp_path[Left.second].x + 1; x < m_temp_path[Right.second].x; ++x)
		{
			auto y = m_temp_path[Right.second].y;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
		PathNode node(m_temp_path[Left.second].x, m_temp_path[Right.second].y);
		if (!IsCompatibleNode(node))
			return false;
		unsigned index = Left.second + 1;
		for (auto y = m_temp_path[Left.second].y + 1; y < m_temp_path[Right.second].y; ++y)
		{
			auto x = m_temp_path[Left.second].x;
			m_temp_path[index].x = x;
			m_temp_path[index].y = y;
			++index;
		}
		m_temp_path[index].x = m_temp_path[Left.second].x;
		m_temp_path[index].y = m_temp_path[Right.second].y;
		*out = index;
		++index;
		for (auto x = m_temp_path[Left.second].x + 1; x < m_temp_path[Right.second].x; ++x)
		{
			auto y = m_temp_path[Right.second].y;
			m_temp_path[index].x = x;
			m_temp_path[index].y = y;
			++index;

		}
		return true;
	}
	else if (Left.first == OrientationNode::Down&&First.first == OrientationNode::Left)
	{
		/*
			7.	<-------------@
				|
				@************
		*/
		for (auto y = m_temp_path[Left.second].y - 1; y > m_temp_path[Right.second].y; --y)
		{
			auto x = m_temp_path[Left.second].x;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}
		for (auto x = m_temp_path[Left.second].x - 1; x > m_temp_path[Right.second].x; --x)
		{
			auto y = m_temp_path[Right.second].y;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}

		PathNode node(m_temp_path[Left.second].x, m_temp_path[Right.second].y);
		if (!IsCompatibleNode(node))
			return false;

		unsigned index = Left.second + 1;
		for (auto y = m_temp_path[Left.second].y - 1; y > m_temp_path[Right.second].y; --y)
		{
			auto x = m_temp_path[Left.second].x;
			m_temp_path[index].x = x;
			m_temp_path[index].y = y;
			++index;
		}
		m_temp_path[index].x = m_temp_path[Left.second].x;
		m_temp_path[index].y = m_temp_path[Right.second].y;
		*out = index;
		++index;
		for (auto x = m_temp_path[Left.second].x - 1; x > m_temp_path[Right.second].x; --x)
		{
			auto y = m_temp_path[Right.second].y;
			m_temp_path[index].x = x;
			m_temp_path[index].y = y;
			++index;
		}
		return true;
	}
	else if (Left.first == OrientationNode::Up&&First.first == OrientationNode::Left) {
		/*
				8.	 @
					  |
					 <-----------@
			*/
		for (auto y = m_temp_path[Left.second].y - 1; y > m_temp_path[Right.second].y; --y)
		{
			auto x = m_temp_path[Left.second].x;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}

		for (auto x = m_temp_path[Left.second].x + 1; x < m_temp_path[Right.second].x; ++x)
		{
			auto y = m_temp_path[Right.second].y;
			PathNode node(x, y);
			if (!IsCompatibleNode(node))
				return false;
		}

		PathNode node(m_temp_path[Left.second].x, m_temp_path[Right.second].y);
		if (!IsCompatibleNode(node))
			return false;

		unsigned index = Left.second + 1;
		for (auto y = m_temp_path[Left.second].y - 1; y > m_temp_path[Right.second].y; --y)
		{
			auto x = m_temp_path[Left.second].x;
			m_temp_path[index].x = y;
			m_temp_path[index].y = y;

			++index;
		}
		m_temp_path[index].x = m_temp_path[Left.second].x;
		m_temp_path[index].y = m_temp_path[Right.second].y;
		*out = index;
		++index;
		for (auto x = m_temp_path[Left.second].x + 1; x < m_temp_path[Right.second].x; ++x)
		{
			auto y = m_temp_path[Right.second].y;
			m_temp_path[index].x = x;
			m_temp_path[index].y = y;
			++index;
		}
		return true;
	}
	return false;
}

void	MoveLinesEstimator::AddNodeConstraitedByWall(Room*room) {
	for (size_t i = 0; i < room->point_list.size(); ++i)
	{
		Point3f beg = room->point_list[i];
		Point3f end = room->point_list[(i + 1) % room->point_list.size()];
		auto center = (beg + end)*0.5f;
		auto normal = ComMath::Normalize((beg - center));
		auto angle = 90.f / 360.f*(3.1415926f * 2);
		auto x = cosf(angle)*normal.x - sinf(angle)*normal.y;
		normal.y = sinf(angle)*normal.x + cosf(angle)*normal.y;
		normal.x = x;
		auto new_normal = normal + center;
		if (!ComMath::PointInPolygon(new_normal, room->point_list))
			normal = -normal;
		//计算墙体范围
		center = beg + normal * wallinfluence;
		normal= end + normal * wallinfluence;
		//计算aabb
		auto maxf = ComMath::Point3fMax(beg, end);
		maxf = ComMath::Point3fMax(maxf, center);
		maxf = ComMath::Point3fMax(maxf, normal);

		auto minf = ComMath::Point3fMin(beg, end);
		minf = ComMath::Point3fMin(minf, center);
		minf = ComMath::Point3fMin(minf, normal);;

		AddNodeConstraitedByAABB(maxf, minf);
	}
}

void MoveLinesEstimator::AddNodeConstraitedByAABB( Point3f&max,  Point3f&min) {
	auto maxf = max - Min;
	auto minf = min -  Min;
	auto maxf_row = (unsigned)ceilf(maxf.y / granularity);
	auto maxf_column = (unsigned)ceilf(maxf.x / granularity);
	auto minf_row = (unsigned)ceilf(minf.y / granularity);
	auto minf_column = (unsigned)ceilf(minf.x / granularity);

	for (auto row = minf_row; row < maxf_row; ++row)
		for (auto column = minf_column; column < maxf_column; ++column)
		{
			if (column < max_column&&row < max_row)
				base_leaf_node[row][column].flag |= E_Entity;
		}
}

void MoveLinesEstimator::RemoveNodeConstraitedByDoor(Room*room, AICase*Case) {
	auto door_list=Case->house.GetDoorsByRoomNo(room->GetNo());
	for (auto door : door_list) {
		auto &beg =  door.start_point;
		auto &end =  door.end_point;
		auto center = (beg + end)*0.5f;
		auto normal = ComMath::Normalize((beg - center));
		auto angle = 90.f / 360.f*(3.1415926f * 2);
		auto x = cosf(angle)*normal.x - sinf(angle)*normal.y;
		normal.y = sinf(angle)*normal.x + cosf(angle)*normal.y;
		normal.x = x;
		auto new_normal = normal + center;
		if (!ComMath::PointInPolygon(new_normal, room->point_list))
			normal = -normal;
		//计算墙体范围
		center = beg + normal * wallinfluence;
		normal = end + normal * wallinfluence;
		//计算aabb
		auto maxf = ComMath::Point3fMax(beg, end);
		maxf = ComMath::Point3fMax(maxf, center);
		maxf = ComMath::Point3fMax(maxf, normal);

		auto minf = ComMath::Point3fMin(beg, end);
		minf = ComMath::Point3fMin(minf, center);
		minf = ComMath::Point3fMin(minf, normal);;
		RemoveNodeConstraitedByAABB(maxf, minf);
	}
}

void MoveLinesEstimator::RemoveNodeConstraitedByModel(Model*model) {
	 auto box = CalculateModelWorldCoordinateBoundingBox(model);
	 RemoveNodeConstraitedByAABB(box[0], box[1]);
}

void MoveLinesEstimator::RemoveNodeConstraitedByAABB(Point3f&max, Point3f&min) {
	auto maxf = max - Min;
	auto minf = min - Min;
	auto maxf_row = (unsigned)ceilf(maxf.y / granularity);
	auto maxf_column = (unsigned)ceilf(maxf.x / granularity);
	auto minf_row = (unsigned)ceilf(minf.y / granularity);
	auto minf_column = (unsigned)ceilf(minf.x / granularity);

	for (auto row = minf_row; row < maxf_row; ++row)
		for (auto column = minf_column; column < maxf_column; ++column)
		{
			if (column < max_column&&row < max_row)
				base_leaf_node[row][column].flag &= ~E_Entity;
		}
}

void MoveLinesEstimator::AddNodeToInsideByAABB(Point3f&max, Point3f&min) {
	auto maxf = max - Min;
	auto minf = min - Min;
	auto maxf_row = (unsigned)ceilf(maxf.y / granularity);
	auto maxf_column = (unsigned)ceilf(maxf.x / granularity);
	auto minf_row = (unsigned)ceilf(minf.y / granularity);
	auto minf_column = (unsigned)ceilf(minf.x / granularity);

	for (auto row = minf_row; row < maxf_row; ++row)
		for (auto column = minf_column; column < maxf_column; ++column)
		{
			if (column < max_column&&row < max_row) {
				base_leaf_node[row][column].flag &= ~E_Outside;
				base_leaf_node[row][column].flag &= ~E_Entity;
			}
		}
}

Point3f MoveLinesEstimator::GridCoordToWorldCoord(int x, int y) {
	Point3f out = Point3f(0.f, 0.f, 0.f);
	out.y += x * granularity+Min.y;
	out.x += y * granularity + Min.x;
	return out;
}

void MoveLinesEstimator::RemoveModel(Model*model) {
	 pass_model.insert(model->GetNo());
}

void MoveLinesEstimator::FillLinesArray(std::vector<Line>&Ref) {
	for (auto &path : m_door_path)
	{
		auto &pathOfNode = path.second;
		if (pathOfNode.room.size()) {
			//线段
			auto &ref = pathOfNode.path_node;
		}
		if (pathOfNode.another_room.size()) {
			//线段
			auto &ref = pathOfNode.another_path;
		}
	}
}
 
// 获得家务动线的模型列表
vector<Model> MoveLinesEstimator::GetHouseworkModels(AICase*ai_case)
{
	vector<Model> model_list;
	// 家务动线
	// 厨房工作三角-厨房门-餐厅餐桌-有洗衣机或晾衣杆功能的阳台门-阳台洗衣机
	// 水槽（135）-灶具（136）-餐桌椅（116）-洗衣机（125）
	vector<int> inner_list = { 135,136,116,125 };
	for (int i = 0; i < ai_case->design.model_list.size(); i++)
	{
		Model tmp = ai_case->design.model_list[i];
		vector<int>::iterator ret;
		ret = find(inner_list.begin(), inner_list.end(), tmp.inner_type);
		if (ret == inner_list.end())
		{
			continue;
		}
		else
		{
			model_list.push_back(tmp);
		}
	}
	return model_list;
}

// 获得访客动线的模型列表
vector<Model> MoveLinesEstimator::GetVisitorModels(AICase*ai_case)
{
	vector<Model> model_list;
	// 访客动线
	// A.入户门 - 客厅茶几 - 有休闲椅的阳台门 - 休闲椅
	// B.入户门 - 餐桌
	// C.入户门 - 客卫门 - 客卫盥洗池 + 马桶
	// 茶几（115）-左休闲椅（109）-右休闲椅（137）-餐桌椅（116）-马桶（127）-浴室柜（128）
	for (int i = 0; i < ai_case->design.model_list.size(); i++)
	{
		Model tmp = ai_case->design.model_list[i];

		// 处理客厅茶几
		if (tmp.inner_type == 115)
		{
			Room * room = ai_case->house.GetRoomByRoomNo(tmp.room_no);
			if (room != NULL)
			{
				if (room->getSpaceId() == KETING || room->getSpaceId() == KECANTING)
				{
					model_list.push_back(tmp);
					continue;
				}
			}
		}
		// 处理左右休闲椅
		else if (tmp.inner_type == 109 || tmp.inner_type == 137)
		{
			Room * room = ai_case->house.GetRoomByRoomNo(tmp.room_no);
			if (room != NULL)
			{
				if (room->getSpaceId() == YANGTAI)
				{
					model_list.push_back(tmp);
					continue;
				}
			}
		}
		// 处理餐桌椅
		else if (tmp.inner_type == 116)
		{
			model_list.push_back(tmp);
			continue;
		}
		// 处理马桶和浴室柜
		else if (tmp.inner_type == 127 || tmp.inner_type == 128)
		{
			// 客卫生间
			Room * room = GetEffectiveBashRoom(ai_case,0);
			if (room != NULL)
			{
				if (tmp.room_no == room->GetNo())
				{
					model_list.push_back(tmp);
					continue;;
				}
			}
		}
	}

	// 针对访客动线加入入户门，通过模拟一个入户门的模型实现
	if (model_list.size() > 0)
	{
		// 入户门对应的墙体
		Wall *entrance_door_wall = ai_case->house.GetWallByEntranceDoor();
		if(entrance_door_wall == NULL)
		{
			return model_list;
		}
		for (int i = 0; i < entrance_door_wall->door_list.size(); i++)
		{
			Door tmp_door = entrance_door_wall->door_list[i];
			// 入户门
			if (tmp_door.door_type == D_SECURITY)
			{
				Point3f normal = ai_case->house.CalculateWallNormal(entrance_door_wall);
				// 模拟一个模型
				Model virtual_door;
				virtual_door.id = 0;
				virtual_door.SetNo(tmp_door.GetNo());
				virtual_door.length = tmp_door.length;
				virtual_door.width = 5;
				virtual_door.room_no = entrance_door_wall->room_no;
				Point3f tmp_point = tmp_door.start_point + normal * virtual_door.width;
				virtual_door.location = (tmp_point + tmp_door.end_point)*0.5;
				model_list.push_back(virtual_door);
				ai_case->design.model_list.push_back(virtual_door);
				break;
			}
		}
	}
	return model_list;
}
// 获得家人动线的模型列表
vector<Model> MoveLinesEstimator::GetFamilyModels(AICase*ai_case)
{
	vector<Model> model_list;
	// 家人动线
	//A.公共走廊-主卧门+子女房门+老人房门+书房门 
	// B.卧室门 - 床
	// C.书房门 - 书桌
	// D.主卧门 - 主卫生间门 - 马桶 + 盥洗池 + 淋浴
	// 床（100）-书桌（106）-马桶（127）-浴室柜（128）
	for (int i = 0; i < ai_case->design.model_list.size(); i++)
	{
		Model tmp = ai_case->design.model_list[i];

		// 处理主卧和次卧的床
		if (tmp.inner_type == 100)
		{
			Room * room = ai_case->house.GetRoomByRoomNo(tmp.room_no);
			if (room != NULL)
			{
				if (room->getSpaceId() == ZHUWO || room->getSpaceId() == CIWO)
				{
					model_list.push_back(tmp);
					continue;
				}
			}
		}
		// 处理书房书桌
		else if (tmp.inner_type == 106)
		{
			Room * room = ai_case->house.GetRoomByRoomNo(tmp.room_no);
			if (room != NULL)
			{
				if (room->getSpaceId() == SHUFANG)
				{
					model_list.push_back(tmp);
					continue;
				}
			}
		}
		// 处理马桶和浴室柜
		else if (tmp.inner_type == 127 || tmp.inner_type == 128)
		{
			// 客卫生间
			Room * room = GetEffectiveBashRoom(ai_case,1);
			if (room != NULL)
			{
				if (tmp.room_no == room->GetNo())
				{
					model_list.push_back(tmp);
					continue;
				}
			}
		}

	}
	return model_list;
}

// 获得有效的客卫生间
// // effecttive_type -0 取客卫生间   1 取主卫生间
Room *MoveLinesEstimator::GetEffectiveBashRoom(AICase*ai_case , int effecttive_type)
{
	if (effecttive_type == 0)
	{
		// 存在客卫生间
		vector<Room*> bash_room_list = ai_case->house.GetRoomsBySpaceId(KEWEISHENGJIAN);
		if (bash_room_list.size() > 0)
		{
			return bash_room_list[0];
		}
		// 存在卫生间
		vector<Room*> bash_room_list2 = ai_case->house.GetRoomsBySpaceId(WEISHENGJIAN);
		if (bash_room_list2.size() > 0)
		{
			return bash_room_list2[0];
		}
		// 存在主卫生间
		vector<Room*> bash_room_list3 = ai_case->house.GetRoomsBySpaceId(ZHUWEISHENGJIAN);
		if (bash_room_list3.size() > 0)
		{
			return bash_room_list3[0];
		}
	}
	else
	{
		// 存在主卫生间
		vector<Room*> bash_room_list3 = ai_case->house.GetRoomsBySpaceId(ZHUWEISHENGJIAN);
		if (bash_room_list3.size() > 0)
		{
			return bash_room_list3[0];
		}
		// 存在卫生间
		vector<Room*> bash_room_list2 = ai_case->house.GetRoomsBySpaceId(WEISHENGJIAN);
		if (bash_room_list2.size() > 0)
		{
			return bash_room_list2[0];
		}

		// 存在客卫生间
		vector<Room*> bash_room_list = ai_case->house.GetRoomsBySpaceId(KEWEISHENGJIAN);
		if (bash_room_list.size() > 0)
		{
			return bash_room_list[0];
		}
	}

	return NULL;
}

// 获得家务动线
vector<Line> MoveLinesEstimator::GetHouseworkLines(AICase*ai_case)
{
	vector<Line> housework_lines;
	// 家务动线涉及的家具编号
	vector<Model> housework_model = GetHouseworkModels(ai_case);
	SetHouseworkModelId(housework_model);
	if (EstimateModelMoveLines(ai_case))
	{
		housework_lines = GetMoveLines();
	}

	return housework_lines;
}
// 获得访客动线
vector<Line> MoveLinesEstimator::GetVisitorLines(AICase*ai_case)
{
	vector<Line> visitor_lines;
	// 访客动线涉及的家具编号
	vector<Model> visitor_model = GetVisitorModels(ai_case);
	SetHouseworkModelId(visitor_model);
	if (EstimateModelMoveLines(ai_case))
	{
		visitor_lines = GetMoveLines();
	}

	return visitor_lines;
}

// 获得家人动线
vector<Line> MoveLinesEstimator::GetFamilyLines(AICase*ai_case)
{
	vector<Line> family_lines;
	// 访客动线涉及的家具编号
	vector<Model> family_model = GetFamilyModels(ai_case);
	SetHouseworkModelId(family_model);
	if (EstimateModelMoveLines(ai_case))
	{
		family_lines = GetMoveLines();
	}

	return family_lines;
}

// 获得家务动线
Json::Value MoveLinesEstimator::GetHouseworkJson(vector<Line> lines)
{
	Json::Value housework;
	Json::Value housework_lines;
	for (int i = 0; i < lines.size(); i++)
	{
		Json::Value json_line;
		json_line["start"] = lines[i].start.ToString();
		json_line["end"] = lines[i].end.ToString();
		housework_lines.append(json_line);
	}

	if (housework_lines.size() > 0)
	{
		housework["lineList"] = housework_lines;
	}
	else
	{
		housework["lineList"].resize(0);
	}

	return housework;
}
// 获得访客动线
Json::Value MoveLinesEstimator::GetVisitorJosn(vector<Line> lines)
{
	Json::Value visitor;
	Json::Value visitor_lines;
	for (int i = 0; i < lines.size(); i++)
	{
		Json::Value json_line;
		json_line["start"] = lines[i].start.ToString();
		json_line["end"] = lines[i].end.ToString();
		visitor_lines.append(json_line);
	}

	if (visitor_lines.size() > 0)
	{
		visitor["lineList"] = visitor_lines;
	}
	else
	{
		visitor["lineList"].resize(0);
	}

	return visitor;
}

// 获得家人动线
Json::Value MoveLinesEstimator::GetFamilyJson(vector<Line> lines)
{
	Json::Value family;
	Json::Value family_lines;
	for (int i = 0; i < lines.size(); i++)
	{
		Json::Value json_line;
		json_line["start"] = lines[i].start.ToString();
		json_line["end"] = lines[i].end.ToString();
		family_lines.append(json_line);
	}

	if (family_lines.size() > 0)
	{
		family["lineList"] = family_lines;
	}
	else
	{
		family["lineList"].resize(0);
	}

	return family;
}
// 计算动线交叉扣分
int MoveLinesEstimator::GetCrossScore(MoveLine struct_moveline)
{
	// 计算动线相关的分数
	// 家务动线与访客动线判定
	int housework_visitor = 0;
	for (int i = 0; i < struct_moveline.housework_lines.size(); i++)
	{
		Line src_line = struct_moveline.housework_lines[i];
		for (int j = 0; j < struct_moveline.visitor_lines.size(); j++)
		{
			Line des_line = struct_moveline.visitor_lines[j];
			if (ComMath::IsIntersectTwoLine(src_line, des_line))
			{
				// 如果两条线的距离都小于80cm则不认为交叉
				if (ComMath::getTwoPointDistance(src_line.start, src_line.end) > EFFECTIVE_LENGTH || ComMath::getTwoPointDistance(des_line.start, des_line.end) > EFFECTIVE_LENGTH)
				{
					housework_visitor++;
				}
			}
		}
	}

	// 家务动线与家人动线判定
	int housework_family = 0;
	for (int i = 0; i < struct_moveline.housework_lines.size(); i++)
	{
		Line src_line = struct_moveline.housework_lines[i];
		for (int j = 0; j < struct_moveline.family_lines.size(); j++)
		{
			Line des_line = struct_moveline.family_lines[j];
			if (ComMath::IsIntersectTwoLine(src_line, des_line))
			{
				// 如果两条线的距离都小于80cm则不认为交叉
				if (ComMath::getTwoPointDistance(src_line.start, src_line.end) > EFFECTIVE_LENGTH || ComMath::getTwoPointDistance(des_line.start, des_line.end) > EFFECTIVE_LENGTH)
				{
					housework_family++;
				}
			}
		}
	}

	// 访客动线与家人动线判定
	int visitor_family = 0;
	for (int i = 0; i < struct_moveline.visitor_lines.size(); i++)
	{
		Line src_line = struct_moveline.visitor_lines[i];
		for (int j = 0; j < struct_moveline.family_lines.size(); j++)
		{
			Line des_line = struct_moveline.family_lines[j];
			if (ComMath::IsIntersectTwoLine(src_line, des_line))
			{
				// 如果两条线的距离都小于80cm则不认为交叉
				if (ComMath::getTwoPointDistance(src_line.start, src_line.end) > EFFECTIVE_LENGTH || ComMath::getTwoPointDistance(des_line.start, des_line.end) > EFFECTIVE_LENGTH)
				{
					visitor_family++;
				}
			}
		}
	}

	return housework_visitor + housework_family + visitor_family;
}

// 计算动线分数
Json::Value MoveLinesEstimator::GetScore(MoveLine struct_moveline, AICase*ai_case)
{
	Json::Value scoreList;
	Json::Value score;

	// 交叉扣分
	Json::Value cross_score;
	int cross_line = GetCrossScore(struct_moveline);
	cross_score["id"] = CORSS_A;
	cross_score["count"] = cross_line;
	cross_score["score"] = 3;
	if (cross_line > 0)
	{
		score.append(cross_score);
	}


	if (score.size() > 0)
	{
		scoreList["scoreList"] = score;
	}
	else
	{
		scoreList["scoreList"].resize(0);
	}
	
	return scoreList;

}

void  MoveLinesEstimator::CombineCoincidiceLines()
{
	std::vector<Line> newLines = line_result;
	while (true)
	{
		int m = -1;
		int n = -1;
		Line combineLine;
		for (int i = 0; i < newLines.size(); ++i)
		{
			if (m != -1)
				break;
			for (int j = 0; j < newLines.size(); ++j)
			{
				if (m != -1)
					break;
				if (i == j)
					continue;
				bool isParalines = ComMath::isParallelTwoLine(newLines[i].start, newLines[i].end, newLines[j].start, newLines[j].end);
				if (!isParalines)
					continue;
				bool onLines = false;
				if (ComMath::isPointOnLine(newLines[i].start, newLines[j].start, newLines[j].end))
					onLines = true;
				if(ComMath::isPointOnLine(newLines[i].end, newLines[j].start, newLines[j].end))
					onLines = true;
				if (ComMath::isPointOnLine(newLines[j].start, newLines[i].start, newLines[i].end))
					onLines = true;
				if (ComMath::isPointOnLine(newLines[j].end, newLines[i].start, newLines[i].end))
					onLines = true;
				if (!onLines)
					continue;
				Point3f maxs = newLines[i].start;
				Point3f mins = maxs;
				maxs = ComMath::Point3fMax(maxs, newLines[i].end);
				maxs = ComMath::Point3fMax(maxs, newLines[j].start);
				maxs = ComMath::Point3fMax(maxs, newLines[j].end);
				mins = ComMath::Point3fMin(mins, newLines[i].end);
				mins = ComMath::Point3fMin(mins, newLines[j].start);
				mins = ComMath::Point3fMin(mins, newLines[j].end);
				combineLine.start = mins;
				combineLine.end = maxs;			 
				m = i;
				n = j;
			}
		}
		if (m == -1)
			break;
		std::vector<Line> tempLines;
		tempLines.push_back(combineLine);
		for (int i = 0; i < newLines.size(); ++i)
		{
			if (i == m || i == n)
				continue;
			tempLines.push_back(newLines[i]);
		}
		newLines = std::move(tempLines);
	}
	line_result = newLines;
}