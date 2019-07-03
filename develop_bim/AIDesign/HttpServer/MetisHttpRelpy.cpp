#include "MetisHttpRelpy.h"
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include"../Log/easylogging++.h"

MetisHttpRelpy::MetisHttpRelpy()
{
}


MetisHttpRelpy::~MetisHttpRelpy()
{
}

void MetisHttpRelpy::LunchReply()
{
	LPTHREAD_START_ROUTINE start_func = (LPTHREAD_START_ROUTINE)(*(size_t*)*(size_t*)this);
	m_thread_handler = CreateThread(NULL, 0, start_func, this, 0, &m_thread_id);
}
unsigned long MetisHttpRelpy::Run(void *parameter)
{
	while (true)
	{
		OnReply();
	}
}

void MetisHttpRelpy::OnReply()
{
	HttpReply *send_task = nullptr;
	{
		//锁住工作
		std::unique_lock<std::mutex>	Lock(mutex);
		if (!relpy_task.size())
			condition.wait(Lock);
		 send_task = relpy_task.front();
		relpy_task.pop_front();
		//释放锁
	}
	 //send msg
	SendMsgForClient(send_task);
}

void MetisHttpRelpy::SendMsgForClient(HttpReply*reply)
{
	Json::FastWriter writer;
	std::string json_file = "";
	Json::Value data;
	//生成头信息(包括设计的数量)
	Json::Value rsp_data =GenerateHeader(reply);
	if (reply->code == err_code)
	{
		goto free_buffer;
	}

	if (reply->apiType == 1)
	{
		//每个方案的Json(具体设计)
		data = GenerateDesign(reply);
		rsp_data["data"] = data;
	}
	else if(reply->apiType == 2)
	{
		rsp_data["data"] = reply->evaluate_design;
	}
	

free_buffer:
	json_file = writer.write(rsp_data);
	//json_file = HttpCommon::G2U(json_file.c_str());
	evbuffer_add_printf(reply->buffer, "%s",json_file.c_str());
	evhttp_send_reply(reply->req, HTTP_OK, "OK", reply->buffer);
	//清理IO buffer为下次做准备
	evbuffer_drain(reply->buffer, evbuffer_get_length(reply->buffer));
	//释放包
	reply->req = nullptr;
	reply->client_design.clear();
	reply->client_house_layout.clear();
	reply->design.clear();
	HttpCommon::reply_pool.Free(reply);
}

void MetisHttpRelpy::ProductReplyTask(HttpReply*reply){
	//添加回复任务

	std::unique_lock<std::mutex>	Lock(mutex);
	relpy_task.push_back(reply);
	condition.notify_one();
}

// 生成头部信息
Json::Value MetisHttpRelpy::GenerateHeader(HttpReply*reply)
{
	Json::Value rsp_data;
	rsp_data["code"] = reply->code;
	//rsp_data["msg"] = HttpCommon::GetMsgByCode(reply->code);
	rsp_data["msg"] = "success";
	//rsp_data["msg"] = "成功！";

	return rsp_data;
}

// 生成数据信息
Json::Value MetisHttpRelpy::GenerateDesign(HttpReply*reply)
{
	Json::Value data;
	int records_total = static_cast<int>(reply->design_entity.layout_list.size());
	data["records_total"] = records_total;

	Json::Value design_list;


	for (int i = 0; i < records_total; i++)
	{
		Json::Value model_list;
		Json::Value model_obj;
		vector<Model> tmp_list = reply->design_entity.layout_list[i].model_list;
		for (vector<Model>::iterator model = tmp_list.begin(); model != tmp_list.end(); model++)
		{
			shared_ptr<DatabaseHelper> p_db = std::make_shared<DatabaseHelper>();
			model->rule_description = p_db->searchRegualrtionsDescriptionByModel(reply->design_entity.space_id,&(*model));
			Json::Value model_value = model->DumpBimData();
			model_obj.append(model_value);
		}
		if (model_obj.size() > 0)
		{
			model_list["moveableMeshList"] = model_obj;
		}
		else
		{
			model_list["moveableMeshList"].resize(0);
		}
		
		design_list.append(model_list);
		break;
	}

	if (design_list.size()>0)
	{
		data["list"] = design_list;
	}
	else
	{
		data["list"].resize(0);
	}
	
	return data;
}
