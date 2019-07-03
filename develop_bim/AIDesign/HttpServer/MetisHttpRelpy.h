#pragma once
#include <condition_variable>
#include <list>
#include <fstream>
#include "../json/json.h"
#include <vector>
#include "HttpCommon.h"
#include <string>


class MetisHttpRelpy
{
public:
	MetisHttpRelpy();
	~MetisHttpRelpy();
public:
	void LunchReply();
	virtual unsigned long Run(void *parameter);
	void ProductReplyTask(HttpReply*reply);
private:
	 void OnReply();
	void SendMsgForClient(HttpReply*reply);
	// 生成头部信息
	Json::Value GenerateHeader(HttpReply*reply);
	// 生成设计数据信息
	Json::Value GenerateDesign(HttpReply*reply);

private:
	std::list<HttpReply*>		relpy_task;
	std::mutex						mutex;
	std::condition_variable	condition;
	unsigned long					m_thread_id;
	void*								m_thread_handler;
};

