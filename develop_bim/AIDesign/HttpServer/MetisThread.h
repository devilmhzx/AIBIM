/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:�߳���
*  ��Ҫ����:
*
*  ��ǰ�汾:
*  ����:��С��
*  ��������:2018-09-11
*  ˵��: Metis Job
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
	//�����߳�
	void LaunchThread();
    virtual unsigned long Run(void *parameter);
	void Stop();
	size_t JobNum();
	//�����������Ʒ���
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

 