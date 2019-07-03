#pragma once
#include "HttpCommon.h"
#include"MetisHttpRelpy.h"
#define WIN32_LEAN_AND_MEAN
#include <json.h>
#include"../AICase.h"
#include "../AICore/MetisDynamicLayout.h"
#include "../AICore/MetisRegionResolver.h"
#include "../AICore/MetisFormulaContainer.h"
#include "../AICore/KitchenAutomaticalDesign.h"
#include "../AICore/ToiletAutomaticlDesign.h"
#include "../AICore/MoveLinesEstimator.h"
#include "../AICore/MetisCompleteFeature.h"
#include "../AICore/MetisGeomancy.h"
#include "../CombinedGeneration.h"
#include "../CombinedGenerationJ.h"
class MetisJob
{
public:
	MetisJob();
	~MetisJob();
public:
	 void LunchMetisjob(MetisHttpRelpy*relpy);
	 virtual unsigned long Run(void *parameter);
	 void ProductAiTask(HttpReply*reply);
	 size_t GetJobSize();
private:
	void Job();
	void EvaluateDesign(HttpReply *task);
	void AutomicalDesign(HttpReply *task);
	void AutomicalDesignBim(HttpReply *task);
private:
	MetisHttpRelpy*				relpy_handler;
	std::list<HttpReply*>		ai_task;
	std::mutex						mutex;
	std::condition_variable	conditional;
	unsigned long					m_thread_id;
	void*							m_thread_handler;
};

