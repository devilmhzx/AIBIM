/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:�Ĳ���
*  ��Ҫ����:����ָ�Ϊ�Ĳ������ݽṹ���ڵ�Ϊ�洢���Ĺ���
*
*  ��ǰ�汾: V1.0.0
*  ����:jirenze
*  ��������:2018-09-04
*  ˵��:
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
 * �ڵ�.
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

	/** ��������� */
	Point2f top_left_point;
	Point2f top_right_point;
	Point2f bottom_left_point;
	Point2f bottom_right_point;
	Point2f center_point;

	/**Ȩ��*/
	float Weight;

	/**����Ƿ��ڷ���������*/
	bool bInside;

	/*********************
	*   ����Ƿ����� ��Χ��
	*   ��Χ @see region_resolver ->door_region_depth
	*********************/
	bool bIndoor;

	/** ����Ƿ��ڵر귶Χ�� */
	bool bInfloorbox;
};
