
#include "MetisThread.h"


MetisThread::MetisThread()
{
}


MetisThread::~MetisThread()
{
}

void MetisThread::LaunchThread() 
{
	LPTHREAD_START_ROUTINE start_func = (LPTHREAD_START_ROUTINE)(*(size_t*)*(size_t*)this);
	m_thread_handler=	CreateThread(NULL, 0, start_func,this, 0, &m_thread_id);
}

void MetisThread::Stop()
{
	
}

size_t MetisThread::JobNum()
{
	return 0;
}

unsigned long MetisThread::Run(void *parameter)
{
	m_bexit = true;
	while (m_bexit.load())
	{
		ConsumeJob();
	}
	return 0;
}

void MetisThread::ProduceJob(MetisJob* job)
{
	{
		//��ס
		std::unique_lock<std::mutex>	Lock(m_work_mutex);
		//�ͷ�
	}
	//�����߳� 
	m_conditon.notify_one();
}

void MetisThread::ConsumeJob()
{
	{
		//��ס����
		std::unique_lock<std::mutex>	Lock(m_work_mutex);

		//�ͷ���
	}
	//ִ���Զ����ֹ���
}
 
void MetisThread::SetIOCP(HANDLE iocp)
{
	m_compelte_port = iocp;
}