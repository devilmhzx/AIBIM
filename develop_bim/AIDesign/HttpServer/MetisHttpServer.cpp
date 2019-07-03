#include "MetisHttpServer.h"
#include <evhttp.h>
#include <event2/thread.h>
#include"../Log/easylogging++.h"

MetisHttpServer::MetisHttpServer()
{
}


MetisHttpServer::~MetisHttpServer()
{
}
 
void  AcceptHttp(struct evhttp_request *req, void *arg)
{
	auto command = evhttp_request_get_command(req);
	//适配新的内存
	auto task = HttpCommon::reply_pool.Allocate();
	if (!task->buffer)
		task->buffer = evbuffer_new();

	//获取最小工作JOB
	auto server = (MetisHttpServer*)arg;
	auto job_handler = server->GetMinJobHandler();

	if (command == EVHTTP_REQ_POST)
	{
		char * post_data = (char *)EVBUFFER_DATA(req->input_buffer);
		size_t  post_size = EVBUFFER_LENGTH(req->input_buffer);
		std::string string;
		string.resize(post_size + 3);
		memcpy(const_cast<char*>(string.c_str() + 2), post_data, post_size);
		string[0] = '/';
		string[1] = '?';
		string[post_size + 2] = 0;
		auto decoded_uri = evhttp_decode_uri(string.c_str());
	
		map < std::string, std::string> params = HttpCommon::AnalyseAutoDesignParams(decoded_uri);
		free(decoded_uri);
		if (params.count(HttpCommon::HouseLayout) > 0 && params.count(HttpCommon::Design) > 0)
		{
			//填充结构
			task->client_house_layout = params[HttpCommon::HouseLayout];
			task->client_design = params[HttpCommon::Design];
			std::string test = HttpCommon::UrlDecode(params[HttpCommon::Design]);
			task->code = suc_code;
		}
		else
		{
			task->code = param_err_code;
			goto discard;
		}
	}
	else
	{
		task->code = err_code;
	}
discard:
	//HTTP header
	evhttp_add_header(req->output_headers, "Server", "myhttpd v 0.0.1");
	evhttp_add_header(req->output_headers, "Content-Type", "application/json; charset=GBK");
	evhttp_add_header(req->output_headers, "Connection", "close");
	//开始工作
	task->req = req;
	task->apiType = 1;
	job_handler->ProductAiTask(task);
}

// 户型评价
void  DesignEvaluate(struct evhttp_request *req, void *arg)
{
	auto command = evhttp_request_get_command(req);
	//适配新的内存
	auto task = HttpCommon::reply_pool.Allocate();
	if (!task->buffer)
		task->buffer = evbuffer_new();

	//获取最小工作JOB
	auto server = (MetisHttpServer*)arg;
	auto job_handler = server->GetMinJobHandler();

	if (command == EVHTTP_REQ_POST)
	{
		struct evkeyvalq args;
		char * post_data = (char *)EVBUFFER_DATA(req->input_buffer);
		size_t  post_size = EVBUFFER_LENGTH(req->input_buffer);
		std::string string;
		string.resize(post_size + 3);
		memcpy(const_cast<char*>(string.c_str() + 2), post_data, post_size);
		string[0] = '/';
		string[1] = '?';
		string[post_size + 2] = 0;
		auto decoded_uri = evhttp_decode_uri(string.c_str());
		evhttp_parse_query(decoded_uri, &args);

		map < std::string, std::string> params = HttpCommon::AnalyseDesignEvaluateParams(decoded_uri);

		if (params.count(HttpCommon::AllHouse) > 0 && params.count(HttpCommon::AllDesign) > 0)
		{
			//填充结构
			task->client_house_layout = params[HttpCommon::AllHouse];
			task->client_design = params[HttpCommon::AllDesign];
			task->code = suc_code;
		}
		else
		{
			task->code = param_err_code;
			goto discard;
		}
	}
	else
	{
		task->code = err_code;
	}

discard:
	//HTTP header
	evhttp_add_header(req->output_headers, "Server", "myhttpd v 0.0.1");
	evhttp_add_header(req->output_headers, "Content-Type", "application/json; charset=utf-8");
	evhttp_add_header(req->output_headers, "Connection", "close");
	//开始工作
	task->req = req;
	task->apiType = 2;
	job_handler->ProductAiTask(task);
}


MetisJob* MetisHttpServer::GetMinJobHandler()
{
	size_t size = SIZE_MAX;
	MetisJob*job = nullptr;
	for (auto i : ai_Job) {
		auto job_size = i->GetJobSize();
		if (job_size < size)
		{
			size = job_size;
			job = i;
		}
	}
	return job;
}

bool MetisHttpServer::InitializeMetisHttpServer(size_t aiJobSize)
{
	CombinedGeneration::InitializeModelPackage();
	evthread_use_windows_threads();
	reply_handler.LunchReply();
	//run thread
	for (auto i = 0; i < aiJobSize; ++i)
	{
		auto job = new MetisJob();
		job->LunchMetisjob(&reply_handler);
		this->ai_Job.push_back(job);
	}


	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
		LOG(ERROR) << "WSAStartup Failed";
		return false;
	}
	struct event_base * base = event_base_new();
	struct evhttp * http_server = evhttp_new(base);
	if (!http_server){
		LOG(ERROR) << "evhttp_new Failed";
		return false;
	}
	
	int ret = evhttp_bind_socket(http_server, "192.168.1.85", 8012);
	if (ret != 0){
		LOG(ERROR) << "bind scoket error(39.96.35.40:8015)";
		return false;
	}

	evhttp_set_cb(http_server, "/ai_design_evaluate/", DesignEvaluate,this);
	evhttp_set_cb(http_server, "/ai_auto_design/", AcceptHttp, this);

	//evhttp_set_gencb(http_server,"/ai_auto_design/", AcceptHttp, this);
	
	LOG(INFO) << "start receive task......";
	event_base_dispatch(base);
	evhttp_free(http_server);
	WSACleanup();
	return true;
}
