/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:构件数据
*  简要描述:

*  当前版本:V1.0.1
*  作者:jirenze
*  创建日期:2019-03-29
*  说明:为了解析BIM中点位相关的数据
**************************************************/ 
#pragma once
#include "BaseBox.h"
#include "WallBox.h"
class ComponentObject : public BaseBox
{
public:
	ComponentObject();
	virtual ~ComponentObject();

	// 加载BIM数据
	void LoadBimComponentObject(Json::Value data);
public:
	/** 组件类型 */
	ComponentJType componentJ_type;

	/** 点位 */
	string room_no;

	WallBox GetNewWallBox(ComponentObject & in_component_object);
private:

};

