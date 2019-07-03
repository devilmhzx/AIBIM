/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:线程类
*  简要描述:
*
*  当前版本:
*  作者:冯小翼
*  创建日期:2018-09-11
*  说明: Metis Job
**************************************************/
#pragma once
#include <list>
#include <mutex>
#include <condition_variable>
#include <Windows.h>
#include <atomic>
#include"../Log/easylogging++.h"

class MetisJob;
class MetisThread
{
public:
	MetisThread();
	~MetisThread();
public:
	//启动线程
	void LaunchThread();
    virtual unsigned long Run(void *parameter);
	void Stop();
	size_t JobNum();
	//即将处理的设计方案
	void ProduceJob(MetisJob* job);
	void ConsumeJob();
	void SetIOCP(HANDLE iocp);
private:
	std::mutex						m_work_mutex;
	std::mutex						m_cache_mutex;
	std::condition_variable	m_conditon;
private:
	HANDLE							m_thread_handler;
	unsigned long					m_thread_id;
	std::atomic_bool				m_bexit;
	HANDLE							m_compelte_port;
};

 