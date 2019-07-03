

#include "MetisJob.h"
#include"../Log/easylogging++.h"

MetisJob::MetisJob()
{
}


MetisJob::~MetisJob()
{
}

void MetisJob::LunchMetisjob(MetisHttpRelpy*relpy)
{
	relpy_handler = relpy;
	LPTHREAD_START_ROUTINE start_func = (LPTHREAD_START_ROUTINE)(*(size_t*)*(size_t*)this);
	m_thread_handler = CreateThread(NULL, 0, start_func, this, 0, &m_thread_id);
}

unsigned long MetisJob::Run(void *parameter)
{
	while (true)
	{
		MetisJob::Job();
	}
}

void MetisJob::Job()
{
	HttpReply *task = nullptr;
	{
		//锁住工作
		std::unique_lock<std::mutex>	Lock(mutex);
		//cout << m_thread_id <<":the task number:" << ai_task.size() << endl;;
		if (!ai_task.size())
			conditional.wait(Lock);
		task = ai_task.front();
		ai_task.pop_front();
		//释放锁
	}
	if (task)
	{
		if (task->apiType == 1)
		{
			AutomicalDesignBim(task);
		}

		if (task->apiType == 2)
		{
			EvaluateDesign(task);
		}
	}

	//添加回复句柄
	relpy_handler->ProductReplyTask(task);
}

void MetisJob::AutomicalDesign(HttpReply *task)
{
	LOG(INFO) << "##########Receive  a auto design task##########";
	if (task->code != suc_code)
	{
		return;
	}

	// 开始进行自动设计
	Json::Reader reader;
	Json::Value root;
	shared_ptr< AICase>ptr_src_case = make_shared< AICase>();

	// 初始化户型数据
	if (reader.parse(task->client_house_layout, root))
	{
		ptr_src_case->house.LoadAppSingleData(root);
	}
	else
	{
		// 户型数据错误
		task->code = house_err_code;
		LOG(ERROR) << "houselayout error:" << task->code;
		return;
	}

	// 初始化设计数据
	if (reader.parse(task->client_design, root))
	{
		ptr_src_case->design.LoadAppSingleData(root);
	}
	else
	{
		// 设计数据错误
		task->code = design_err_code;
		LOG(ERROR) << "design error:" << task->code;
		return;
	}

	ptr_src_case->Init();
	// 初始化单房间数据
	ptr_src_case->InitSingleCase();
	if (ptr_src_case->single_case_list.size() != 1)
	{
		// 户型数据错误
		task->code = house_err_code;
		LOG(ERROR) << "houselayout error:" << task->code;
		return;
	}

	// 获得待布局的房间数据
	SingleCase single_case = ptr_src_case->single_case_list[0];
	bool region_flag = false;
	if (single_case.IsRegionLayout())
	{
		RoomRegion region = single_case.region_list[0];
		single_case = (single_case.GetNewSingleCaseByRegion(region));
		region_flag = true;
	}
	SingleCase* p_single_case = &single_case;
	p_single_case->entrance_door = ptr_src_case->house.entrance_door;


	// 判定户型的合理性
	if (!region_flag)
	{
		if (p_single_case->door_list.size() < 1)
		{
			// 户型数据错误
			task->code = not_door_err_code;
			LOG(ERROR) << "houselayout is not exist door:" << task->code;
			return;
		}
	}

	// 房间变换
	int change_room_id = ComUtil::ChangeRoomId(p_single_case->single_room.getSpaceId());
	if (change_room_id == NOT_NAME)
	{
		// 户型数据错误
		task->code = not_name_err_code;
		LOG(ERROR) << "the room is not name:" << task->code;
		return;
	}
	p_single_case->single_room.setSpaceId(change_room_id);
	// 根据规则初始化模型的布局信息
	if (change_room_id == WEISHENGJIAN || change_room_id == CHUFANG)
	{
		;
	}
	else
	{
		// 初始化模型信息,过滤没有规则的模型
		ptr_src_case->InitModelLayout(p_single_case);
	}

	if (change_room_id == CHUFANG)
	{
		LOG(INFO) << "##########Begin kitchen layout" << p_single_case->single_room.GetNo() << ")##########";
		// 厨房布局
		KitchenDesign Design;
		if (!Design.AutoDesign(p_single_case))
		{
			// 不存在合适的方案
			task->code = not_exist_code;
			LOG(ERROR) << "not exist layout:" << task->code;
			return;
		}
	}
	else if (change_room_id == WEISHENGJIAN)
	{
		LOG(INFO) << "##########Begin toilet  layout" << p_single_case->single_room.GetNo() << ")##########";
		// 模拟生成马桶位置
		p_single_case->CreateFloorboxOfStool();
		ToiletAutomaticlDesign toilet_design;
		toilet_design.AutoDesign(p_single_case);
	}
	else
	{
		if (change_room_id == KETING)
		{
			// 创建组合模型
			p_single_case->CreateCombinedModel();
		}

		// 构造区域解析器
		LOG(INFO) << "##########Begin   layout" << p_single_case->single_room.GetNo() << ")##########";
		shared_ptr<MetisRegionResolver> region_resolver = make_shared<MetisRegionResolver>(p_single_case);
		MetisFormulaContainer formula_container;
		MetisRegionResolver* p_region_resolver = region_resolver.get();
		// 解析独立性性规则
		bool Independent = formula_container.ConstructWeightMapWithIndependent(p_region_resolver, p_single_case);
		if (Independent == false)
		{
			LOG(ERROR) << "Failure of analyze independent rules(ConstructWeightMapWithIndependent).";
			// 解析依赖性规则错误
			task->code = dependent_err_code;
			return;
		}
		// 动态布局独立性规则物品
		MetisDynamicLayout::CalculateIndependentExcellentRegion(region_resolver);

		// 解析依赖性规则
		bool dependent = formula_container.ConstructWeightMapWithAllType(p_region_resolver, p_single_case);
		if (dependent == false)
		{
			LOG(ERROR) << "Failure of analyze dependent rules(ConstructWeightMapWithAllType).";
			// 解析依赖性规则错误
			task->code = undependent_err_code;
			return;
		}
		// 动态布局依赖性物品
		MetisDynamicLayout::CalculateAttachExcellentRegion(region_resolver);
		//LOG(INFO) << "Layout sucess!" << p_single_case->single_room.GetNo();
		MetisDynamicLayout::ScalableFurniture(region_resolver);
		// 处理家具吸附性
		MetisDynamicLayout::OptimizedFurnitureLayout(region_resolver);
		// 提取方案信息
		MetisDynamicLayout::ReviseFurnitureInformaiton(region_resolver, p_single_case);

		if (p_single_case->layout_list.size() < 1)
		{
			// 不存在有效的设计数据
			task->code = not_exist_code;
			return;
		}
		// 解析包
		p_single_case->AnalyzeCombinedModel();
	}

	// 同步模型数据
	ptr_src_case->SyncSingleToDesign(p_single_case);
	int layout_count = static_cast<int>(ptr_src_case->design.layout_list.size());
	// 去除重复的布局
	ptr_src_case->design.RomoveSameLayout();
	int new_count = static_cast<int>(ptr_src_case->design.layout_list.size());
	if (layout_count != new_count)
	{
		LOG(INFO) << "the number of layout count:" << layout_count;
		LOG(INFO) << "the number of  rest layout count:" << new_count;
	}

	// 返回布局模型中对应的规则
	task->design_entity = ptr_src_case->design;
	task->design_entity.space_id = change_room_id;
	LOG(INFO) << "##########End   layout success" << p_single_case->single_room.GetNo() << ")##########";
}


void MetisJob::AutomicalDesignBim(HttpReply *task)
{
	LOG(INFO) << "##########Receive  a auto design task##########";
	if (task->code != suc_code)
	{
		return;
	}

	// 开始进行自动设计
	Json::Reader reader;
	Json::Value root;
	shared_ptr< AICase>ptr_src_case = make_shared< AICase>();

	// 初始化户型数据
	if (reader.parse(task->client_house_layout, root))
	{
		ptr_src_case->house.LoadBimSingleData(root);
	}
	else
	{
		// 户型数据错误
		task->code = house_err_code;
		LOG(ERROR) << "houselayout error:" << task->code;
		return;
	}

	// 初始化设计数据
	if (reader.parse(task->client_design, root))
	{
		ptr_src_case->design.LoadBimSingleData(root);
	}
	else
	{
		// 设计数据错误
		task->code = design_err_code;
		LOG(ERROR) << "design error:" << task->code;
		return;
	}

	ptr_src_case->Init();
	// 初始化单房间数据
	ptr_src_case->InitSingleCase();
	if (ptr_src_case->single_case_list.size() != 1)
	{
		// 户型数据错误
		task->code = house_err_code;
		LOG(ERROR) << "houselayout error:" << task->code;
		return;
	}

	// 获得待布局的房间数据
	SingleCase single_case = ptr_src_case->single_case_list[0];
	bool region_flag = false;
	if (single_case.IsRegionLayout())
	{
		RoomRegion region = single_case.GetLayoutRegion();
		if (!region.getIsVirtual())
		{
			single_case = (single_case.GetNewSingleCaseByRegion(region));
			region_flag = true;
		}
	}
	SingleCase* p_single_case = &single_case;
	p_single_case->entrance_door = ptr_src_case->house.entrance_door;


	// 判定户型的合理性
	if (!region_flag)
	{
		if (p_single_case->door_list.size() < 1)
		{
			// 户型数据错误
			task->code = not_door_err_code;
			LOG(ERROR) << "houselayout is not exist door:" << task->code;
			return;
		}
	}

	// 房间变换
	int change_room_id = ComUtil::ChangeRoomId(p_single_case->single_room.getSpaceId());
	if (change_room_id == NOT_NAME)
	{
		// 户型数据错误
		task->code = not_name_err_code;
		LOG(ERROR) << "the room is not name:" << task->code;
		return;
	}
	p_single_case->single_room.setSpaceId(change_room_id);
	// 根据规则初始化模型的布局信息
	if (change_room_id == WEISHENGJIAN || change_room_id == CHUFANG)
	{
		;
	}
	else
	{
		// 初始化模型信息,过滤没有规则的模型
		ptr_src_case->InitModelLayout(p_single_case);
	}

	if (change_room_id == CHUFANG)
	{
		LOG(INFO) << "##########Begin kitchen layout" << p_single_case->single_room.GetNo() << ")##########";
		// 厨房布局
		KitchenDesign Design;
		if (!Design.AutoDesign(p_single_case))
		{
			// 不存在合适的方案
			task->code = not_exist_code;
			LOG(ERROR) << "not exist layout:" << task->code;
			return;
		}
	}
	else if (change_room_id == WEISHENGJIAN)
	{
		LOG(INFO) << "##########Begin toilet  layout" << p_single_case->single_room.GetNo() << ")##########";
		// 模拟生成马桶位置
		p_single_case->CreateFloorboxOfStool();
		ToiletAutomaticlDesign toilet_design;
		toilet_design.AutoDesign(p_single_case);
	}
	else
	{
 		shared_ptr<CombinedGenerationJ> generationJ = make_shared<CombinedGenerationJ>();
		if (change_room_id == KETING)
		{
			// 创建组合模型
			/** 进行新虚拟包测试 @纪仁泽 2019-02-25 */
			//p_single_case->CreateCombinedModel();
			if (generationJ->InitModelPackage(p_single_case->model_list))
			{
				p_single_case->model_list.push_back(generationJ->GetVirtualModel());
			}
		}

		// 构造区域解析器
		LOG(INFO) << "##########Begin   layout" << p_single_case->single_room.GetNo() << ")##########";
		shared_ptr<MetisRegionResolver> region_resolver = make_shared<MetisRegionResolver>(p_single_case);
		MetisFormulaContainer formula_container;
		MetisRegionResolver* p_region_resolver = region_resolver.get();
		// 解析独立性规则
		bool Independent = formula_container.ConstructWeightMapWithIndependent(p_region_resolver, p_single_case);
		if (Independent == false)
		{
			LOG(ERROR) << "Failure of analyze independent rules(ConstructWeightMapWithIndependent).";
			// 解析依赖性规则错误
			task->code = dependent_err_code;
			return;
		}
		// 动态布局独立性规则物品
		MetisDynamicLayout::CalculateIndependentExcellentRegion(region_resolver);

		// 解析依赖性规则
		bool dependent = formula_container.ConstructWeightMapWithAllType(p_region_resolver, p_single_case);
		if (dependent == false)
		{
			LOG(ERROR) << "Failure of analyze dependent rules(ConstructWeightMapWithAllType).";
			// 解析依赖性规则错误
			task->code = undependent_err_code;
			return;
		}
		// 动态布局依赖性物品
		MetisDynamicLayout::CalculateAttachExcellentRegion(region_resolver);
		//LOG(INFO) << "Layout sucess!" << p_single_case->single_room.GetNo();
		MetisDynamicLayout::ScalableFurniture(region_resolver);
		// 处理家具吸附性
		MetisDynamicLayout::OptimizedFurnitureLayout(region_resolver);
		// 提取方案信息
		MetisDynamicLayout::ReviseFurnitureInformaiton(region_resolver, p_single_case);

		if (p_single_case->layout_list.size() < 1)
		{
			// 不存在有效的设计数据
			task->code = not_exist_code;
			return;
		}
		/** 进行新虚拟包测试 @纪仁泽 2019-02-25 */
		// 解析包
		//p_single_case->AnalyzeCombinedModel();
		bool Ishaveplan = generationJ->AnalyzePackage(p_single_case->layout_list);

		/** 客厅解析后，是否返回正确布局 */
		if (Ishaveplan == false)
		{
			task->code = not_exist_code;
		}
	}

	// 同步模型数据
	ptr_src_case->SyncSingleToDesign(p_single_case);
	int layout_count = static_cast<int>(ptr_src_case->design.layout_list.size());
	// 去除重复的布局
	ptr_src_case->design.RomoveSameLayout();
	int new_count = static_cast<int>(ptr_src_case->design.layout_list.size());
	if (layout_count != new_count)
	{
		LOG(INFO) << "the number of layout count:" << layout_count;
		LOG(INFO) << "the number of  rest layout count:" << new_count;
	}

	// 返回布局模型中对应的规则
	task->design_entity = ptr_src_case->design;
	task->design_entity.space_id = change_room_id;
	LOG(INFO) << "##########End   layout success" << p_single_case->single_room.GetNo() << ")##########";
}

void MetisJob::EvaluateDesign(HttpReply *task)
{
	LOG(INFO) << "##########Receive  a design evaluate task##########";
	if (task->code != suc_code)
	{
		return;
	}

	// 开始进行自动设计
	Json::Reader reader;
	Json::Value root;
	AICase* src_case = new AICase();

	// 初始化户型数据
	if (reader.parse(task->client_house_layout, root))
	{
		src_case->house.LoadAppData(root);
	}
	else
	{
		// 户型数据错误
		task->code = house_err_code;
		LOG(ERROR) << "houselayout error:" << task->code;
		return;
	}

	// 初始化设计数据
	if (reader.parse(task->client_design, root))
	{
		src_case->design.LoadAppData(root);
	}
	else
	{
		// 设计数据错误
		task->code = design_err_code;
		LOG(ERROR) << "design error:" << task->code;
		return;
	}

	src_case->Init();
	// 初始化单房间数据
	src_case->InitSingleCase();
	if (src_case->single_case_list.size() < 1)
	{
		// 户型数据错误
		task->code = house_err_code;
		LOG(ERROR) << "houselayout error:" << task->code;
		return;
	}

	// 动线评估
	std::shared_ptr<MoveLinesEstimator> move_lines_estimator = std::make_shared<MoveLinesEstimator>();
	Json::Value movingline = move_lines_estimator->GetMovingLinesJson(src_case);

	// 全能齐全
	shared_ptr<AICase> p_ai_case(src_case);
	shared_ptr<MetisCompleteFeature> feature = make_shared<MetisCompleteFeature>(p_ai_case);
	Json::Value design_fucntion = feature->TransferData();

	// 风水评估
	shared_ptr<MetisGeomancy> geomancycheck = make_shared<MetisGeomancy>(p_ai_case);
	Json::Value geomantic = geomancycheck->GetGeomancyJson();

	// 整体数据组装
	Json::Value data;
	data["movingLines"] = movingline;
	data["funtion"] = design_fucntion;
	data["geomantic"] = geomantic;
	task->evaluate_design = data;
}

size_t MetisJob::GetJobSize() {
	return ai_task.size();
}
void MetisJob::ProductAiTask(HttpReply*reply) {
	//添加回复任务
	{
		std::unique_lock<std::mutex>	Lock(mutex);
		ai_task.push_back(reply);
	}
	conditional.notify_one();
}
