#pragma once
#pragma comment(lib, "Ws2_32.lib")	
#include "HttpCommon.h"
#include "MetisHttpRelpy.h"
#include "MetisJob.h"
#include <vector>
#include <string>
#include "../CombinedGeneration.h"

struct evhttp_request;
void AcceptHttp(evhttp_request *req, void *arg);
class MetisHttpServer
{
public:
	MetisHttpServer();
	~MetisHttpServer();
	//初始化服务
	bool InitializeMetisHttpServer(size_t aiJobSize);
	MetisJob* GetMinJobHandler();
private:
	MetisHttpRelpy				reply_handler;
	std::vector<MetisJob*>	ai_Job;
};
 