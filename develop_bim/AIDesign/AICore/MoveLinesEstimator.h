#pragma once
#include "../Three/json/json.h"
#include "../AICase.h"
#include <set>
#include <unordered_map>

#define TOTAL_SCORE 40
#define EFFECTIVE_LENGTH 80

enum NodeProperty
{
	E_Outside=0x1,
	E_Entity =0x2,
	E_Close	=0x4,
	E_Debug =0x8
};

enum OrientationNode
{
		None,
		Left,
		Right,
		Up,
		Down
};
using OrientationNodePair = std::pair<OrientationNode, unsigned>;
__declspec(align(32))
struct EstimatorNode
{
	EstimatorNode():father_x(0), father_y(0), flag(0){}
	/*中心点*/
	Point3f centre_point;
	/*目标损失*/
	float		disination_cost;
	/*路径损失*/
	float		travel_cost;
	/*父X坐标*/
	int		father_x;
	/*父Y坐标*/
	int		father_y;
	unsigned flag;
};
struct PathNode
{
	PathNode(int i,int j):x(i),y(j){}
	PathNode(){}
	/*节点坐标X*/
	int		x;
	/*节点坐标y*/
	int		y;
	/*Loss 损失*/
	float	    score;
};
 
struct MoveLine
{
	/*家务线*/
	vector<Line> housework_lines;
	/*访客线*/
	vector<Line> visitor_lines;
	/*家人线*/
	vector<Line> family_lines;
	/*评估分数*/
	float	    score;
};
bool operator< (const PathNode &Left, const PathNode &Right);
using NodeCoordinate = std::pair<int, int>;
using NodeCoordinateArray = std::vector<NodeCoordinate>;
class MoveLinesEstimator
{
public:
	/*节点路径信息*/
	using PathInformation = std::shared_ptr<std::vector<PathNode>>;
	/*次要节点结构*/
	using MinorPath = std::vector<PathNode>;
	/*模型编号*/
	using Model_NO = std::string;
	/*门编号*/
	using Door_NO = Model_NO;
	/*房间编号*/
	using Room_NO = Door_NO;
	struct PathOfDoor
	{
		/**路径*/
		PathInformation	path_node;
		/**房间编号*/
		Room_NO				room;
		/**另一个路径*/
		PathInformation	another_path;
		/**另一个路径所处的房间编号*/
		Room_NO				another_room;
	};
	struct RoomRelation
	{
		/**开始房间*/
		Room_NO					beg_room;
		/**路径房间*/
		std::vector<Door_NO>	path_room;
		/**路径门*/
		std::vector<Door_NO>	path_door;
		/**结束房间*/
		Room_NO					end_room;
	};
	struct ModelNode
	{
		/*模型编号*/
		Model_NO					model;
		/*模型覆盖的节点数组*/
		NodeCoordinateArray			cover_node;
	};
public:
	MoveLinesEstimator();
	~MoveLinesEstimator();
	/*路径节点集合*/
	using VectorCoordinate = std::vector<PathNode>;
	using CoverNode = std::vector<std::vector<EstimatorNode>>;
public:
	/*-----------------------------------------------------------------------------
	* SetHouseworkModelId : 
	*    1.输入需要连接模型ID的数组
	*    2.无返回,调用EstimateModelMoveLines 之后可以得到相关的走线
	*	 3.GetMoveLines() 得到此类走线的线端描述(begPoint,endPoint)
	------------------------------------------------------------------------------*/
	void SetHouseworkModelId(std::vector<Model>&Model);
	/*----------------------------------------------------------------------------
	*CorrectMoveLine:
	*    1.用来矫正重叠的线端(即A和B线段结合在某些子划分上重叠,包含完全重叠)
	*    2.输入参数其他的线端集合
	------------------------------------------------------------------------------*/
	void CorrectMoveLine(std::vector<Line>&dest,const std::vector<Line> &other_lines);

	/*------------------------------------------------------------------------------
	*   GetMovingLinesJson 
	*	1.	获取动线json描述
	------------------------------------------------------------------------------*/
	Json::Value GetMovingLinesJson(AICase*ai_case);
	/*------------------------------------------------------------------------------
	*   EstimateModelMoveLines
	*	1.	动线生成
	*   2.  失败返回错误
	------------------------------------------------------------------------------*/
	bool EstimateModelMoveLines(AICase*ai_case);
	/*------------------------------------------------------------------------------
	*   GetMoveLines
	*	1.	获取动线的线端数组,首尾相连
	*   2.  无参数
	------------------------------------------------------------------------------*/
	const std::vector<Line>&GetMoveLines();
	/*------------------------------------------------------------------------------
	*   GetHouseworkModels
	*	1.	获得家务动线的模型列表
	*   2.  输入:AiCase的指针
	------------------------------------------------------------------------------*/
	vector<Model> GetHouseworkModels(AICase*ai_case);
	/*------------------------------------------------------------------------------
	*   GetHouseworkModels
	*	1.	获得访客动线的模型列表
	*   2.  输入:AiCase的指针
	------------------------------------------------------------------------------*/
	vector<Model> GetVisitorModels(AICase*ai_case);
	/*------------------------------------------------------------------------------
	*   GetHouseworkModels
	*	1.	获得家人动线的模型列表
	*   2.  输入:AiCase的指针
	------------------------------------------------------------------------------*/
	vector<Model> GetFamilyModels(AICase*ai_case);
	/*------------------------------------------------------------------------------
	*   GetHouseworkModels
	*	1.	获得家人动线的模型列表
	*   2.  输入:AiCase的指针
	------------------------------------------------------------------------------*/
	
	/**!!!! 内部测试,测试动线,直接返回节点可视化!!!!*/
	std::vector<std::vector<EstimatorNode>>& EstimateMoveLines(AICase*ai_case);
private:
	/*检测是不是能生成动线*/
	bool isVaildSignleCase(AICase*ai_case);

	/*建立区域网格节点*/
	bool BuildRegionGrid(AICase*ai_case);
	/*网格划分属性,在户型内,在户型外*/
	void SplitGridToInsideRegion(AICase*ai_case);
	/*清理所有缓存*/
	void Clear();
	/*粗略检测模型是不是溢出户型区域*/
	bool DetectModelOnOuter(AICase*ai_case);
	/*转换为节点路径线段*/
	void NodePathToLineContainer();
	/*单区域布局*/
	void NodePathToLineContainerWithSignleRegion();
	/*无功能,测试用*/
	bool BuildHouseworkLines(AICase*ai_case);
	/*模型分区域*/
	void DivideModelToRoom(AICase*ai_case);
	/*保存模型覆盖的节点*/
	NodeCoordinateArray SearchNodeCoveredByModel(Model* model);
	/*清理模型节点*/
	void	DisableModelRegion(Model* model);
	/*设置所有门的节点*/
	void OpenDoorNode(AICase*ai_case);
	/*建立关系容器*/
	void BuildRelationContainer();
	/*添加区域的路径区域*/
	bool AddRouteRegionToRelationContainer(AICase*ai_case);
	/*添加门路径区域*/
	bool AddRouteDoorToRelationContaier(AICase*ai_case);
	/*建立门与区域的关系*/
	bool BuildDoorWithRegionRelation(AICase*ai_case);
	/*处理所有区域路径*/
	bool FigureOutRelationPathWithAStart(AICase*ai_case);
	/*处理剩余的Model*/	 
	bool FindRouteWithRestModel(AICase*ai_case);
	/*处理只有一个区域的*/
	bool FindMoveLineWithOneRegion(AICase*ai_case);
	/*检查门是不是被处理过*/
	bool IsDoorHasBeenPassed(Door_NO door, Room_NO room);
	/*检查一个区域是否还有走线*/
	PathInformation SearchOtherPathByRoom(Point3f Point, Room_NO room);
	/*获取一个一条路径最近的点*/
	PathNode SearchNearestNode(PathInformation infor, Point3f Point);
	/*获取最远的model距离门*/
	Model* SearchFarestModelWithDoor(Door*door, Room*room,AICase*ai_case);
	/*添加路径*/
	void AddNodePathToDoorPath(VectorCoordinate&Ref,Door_NO door,Room_NO room);
	/*添加路径*/
	void AddNodePathToDoorPath(PathInformation infor, Door_NO door, Room_NO room);
	/*添加路径*/
	void AddNodePathToMinorPath(VectorCoordinate&Ref);
	/*清理FLAG*/
	void ClearCloseFlag();
	/*--A*--*/
	bool MinimunRoute(Door*beg,Model*end);
	/*--A*--*/
	bool MinimumRouteModelWithModel(Model*beg, Model*end);
	/*--A*--*/
	bool MinimumRouteModelWithModel(Point3f beg, Point3f end);
	/*--A*--*/
	bool MinimumRouteModelWithModel(Point3f beg, PathNode node);
	/*A Start Search Candidate Node */
	void SearchCandidateNode(VectorCoordinate &Node, CoverNode&procedural, PathNode &destination);
	/*Collect node to temp container*/
	void CollectPathNodeToVector(Point3f &end);
	/*Collect node to temp container*/
	void CollectPathNodeToVector(PathNode end);
	/*压缩路径*/
	void CompressNode();
	/*确定拐点方向*/
	OrientationNode MakeSureOrientationOfTurningPoint(PathNode&Node1, PathNode&Node2);
	/*确定是否可以压缩(for beg)*/
	bool	MakeSureCanBeCopressed(OrientationNodePair&First,OrientationNodePair&Left, OrientationNodePair&Right,unsigned *out);
	/*确定是否可以压缩(for end)*/
	bool MakeSureCanBeCopressedEnd(OrientationNodePair&First, OrientationNodePair&Left, OrientationNodePair&Right, unsigned *out);
	/*清理一个节点FLAG*/
	void SetZeroNode(PathNode &node,const PathNode &destNode);
	/*判断合适节点*/
	bool IsCompatibleNode(PathNode &destination);
	/*获取最小Loss节点*/
	void CalculateSubNode(PathNode &destination,VectorCoordinate&set, CoverNode&procedural);
	/*计算节点cost*/
	float CalculateCostOpen(const PathNode &node, const PathNode&Father);
	/*重置父节点*/
	void ResetFatherNodeOpen(PathNode &node, const PathNode&Father);
	/*设置open 节点*/
	PathNode  CalculateNodeToOpenTable(const PathNode&child, const PathNode&Father,const PathNode&Dist);
	/*添加墙体限制*/
	void	AddNodeConstraitedByWall(Room*room);
	/*添加AABB限制*/
	void AddNodeConstraitedByAABB( Point3f&max,  Point3f&min);
	/*去除门的限制*/
	void RemoveNodeConstraitedByDoor(Room*room,AICase*Case);
	/*去除AABB限制*/
	void RemoveNodeConstraitedByAABB(Point3f&max, Point3f&min);
	/*去除Model限制*/
	void RemoveNodeConstraitedByModel(Model*model);
	/*添加值区域内*/
	void AddNodeToInsideByAABB(Point3f&max, Point3f&min);
	/*网格坐标转世界坐标*/
	Point3f GridCoordToWorldCoord(int x, int y);
	/*去除model*/
	void RemoveModel(Model*model);
	/*填充直线*/
	void FillLinesArray(std::vector<Line>&Ref);
	/*获得家务动线*/
	vector<Line> GetHouseworkLines(AICase*ai_case);
	/* 获得访客动线*/
	vector<Line> GetVisitorLines(AICase*ai_case);
	/* 获得家人动线*/
	vector<Line> GetFamilyLines(AICase*ai_case);
	/* 获得家务动线*/
    Json::Value GetHouseworkJson(vector<Line> lines);
	/* 获得访客动线*/
	Json::Value GetVisitorJosn(vector<Line> lines);
	/* 获得家人动线*/
	Json::Value GetFamilyJson(vector<Line> lines);
	/* 计算动线交叉扣分*/
	int GetCrossScore(MoveLine struct_moveline);
	/* 计算动线分数*/
	Json::Value GetScore(MoveLine struct_moveline, AICase*ai_case);
	/*合并直线*/
	void  CombineCoincidiceLines();
private:
	/*  获得有效的卫生间*/
	Room *GetEffectiveBashRoom(AICase*ai_case, int effecttive_type);
	/* 世界坐标到节点坐标转换*/
	PathNode CalculateNodeIndexOnCoordinate(Point3f&Ref);
	/* 清理关闭的节点*/
	void ModifyNodeToZeroState();
	/* 关闭的节点*/
	void SetNodeToClose(int i, int j);
	/* 获取节点地址*/
	EstimatorNode* GetNodeAddr(PathNode Node);
	/* 计算模型AABB*/
	std::vector<Point3f> CalculateModelWorldCoordinateBoundingBox(Model*model);
	using RoomModels = std::unordered_map<Room_NO, std::vector<ModelNode>>;
public:
	CoverNode								base_leaf_node;
	RoomModels								room_conrresponding_model;
	unsigned								max_row;
	unsigned								max_column;
	Point3f									Max;
	Point3f									Min;
	/* 家务走线*/
	std::set<Model_NO>						model_no;
	/* 缓存一次走线节点*/
	std::vector<PathNode>					m_path;
	/* 装换节点起点终点buffer*/
	std::vector<PathNode>					m_temp_path;
	/* 区域关系容器*/
	std::vector<RoomRelation>				m_room_container;
	/* 门域关系容器*/
	std::unordered_map<Door_NO,PathOfDoor>	m_door_path;
	/* 区域和门的关系*/
	std::unordered_map <Room_NO, std::set<Door_NO>>		relation_room_door;
	/* 已经处理的Model*/
	std::set<Model_NO>									pass_model;
	/* 次路径*/
	std::vector<MinorPath>								minor_path;
	/* 结果*/
	std::vector<Line>									line_result;
};

