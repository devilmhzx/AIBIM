/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:四叉树
*  简要描述:房间分割为四叉树数据结构，节点为存储器的功能
*
*  当前版本: V1.0.0
*  作者:jirenze
*  创建日期:2018-09-04
*  说明:
**************************************************/
#pragma once
#include "../SingleCase.h"
#include "../ComMath.h"
#include "../Box2D.h"
#include "../Point2f.h"
#include<memory>
#include <vector>

/*************************
 *
 * 节点.
 *
 ***********************/
struct MetisNode
{
public:
	MetisNode() {};

	MetisNode(const MetisNode *Ref)
	{
		memcpy(this, Ref, sizeof(MetisNode));
	}

	MetisNode(const Box2D &node_box);

	~MetisNode() {};

	/** 五个点坐标 */
	Point2f top_left_point;
	Point2f top_right_point;
	Point2f bottom_left_point;
	Point2f bottom_right_point;
	Point2f center_point;

	/**权重*/
	float Weight;

	/**标记是否在房间区域内*/
	bool bInside;

	/*********************
	*   标记是否在门 范围内
	*   范围 @see region_resolver ->door_region_depth
	*********************/
	bool bIndoor;

	/** 标记是否在地标范围内 */
	bool bInfloorbox;
};
