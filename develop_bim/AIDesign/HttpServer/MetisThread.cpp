
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
		//锁住
		std::unique_lock<std::mutex>	Lock(m_work_mutex);
		//释放
	}
	//唤醒线程 
	m_conditon.notify_one();
}

void MetisThread::ConsumeJob()
{
	{
		//锁住工作
		std::unique_lock<std::mutex>	Lock(m_work_mutex);

		//释放锁
	}
	//执行自动布局工作
}
 
void MetisThread::SetIOCP(HANDLE iocp)
{
	m_compelte_port = iocp;
}