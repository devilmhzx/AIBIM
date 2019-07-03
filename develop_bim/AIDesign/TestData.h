/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:测试数据
*  简要描述:
*
*  当前版本:
*  作者:常远
*  创建日期:2018-09-20
*  说明:
**************************************************/

#include <map>
#include "ComMath.h"
#include "ComUtil.h"
#include "Point3f.h"
#include "AICase.h"
#include "Room.h"
#include "Houselayout.h"
#include "AICore/MetisNode.h"
#include "AICore/MetisRegionResolver.h"
#include "AICore/MetisDynamicLayout.h"
#include "AICore/MetisFormulaContainer.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include "Three\sqlite3\sqlite3.h"  
#include "DatabaseHelper.h"
#include <memory>

#include "Three/json/json.h"

#pragma once
class TestData
{
public:
	TestData();
	~TestData();
	static TestData* initance();

public:
	AICase *getAICaseById(int index,vector<int>model_list);
	bool initMapCase();
	string getOutPath(int index);

private:
	static TestData * p;
	map<int, vector<string>> src_maps;
	map<int, AICase*> map_case;
	
};

