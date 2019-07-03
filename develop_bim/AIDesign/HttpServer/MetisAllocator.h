/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:�ڴ��ģ����
*  ��Ҫ����:
*
*  ��ǰ�汾:
*  ����:��С��
*  ��������:2018-09-12
*  ˵��: MetisAllocator
**************************************************/
#pragma once
#include <list>
#include <mutex>
template<typename Type>
class MetisAllocator
{
public:
	MetisAllocator() = default;
	~MetisAllocator() = default;
	void Destory()
	{
		for (auto p : mem_pool)
		{
			delete p;
		}
	}
	Type* Allocate() 
	{
		Type* out;
		mutex.lock();
		if (!mem_pool.size())
		{
			mutex.unlock();
			out = new Type();
			return out;
		}
		else
		{
			out = mem_pool.front();
			mem_pool.pop_front();
			mutex.unlock();
			return out;
		}
		return out;
	}
	void   Free(Type* dest)
	{
		mutex.lock();
		mem_pool.push_back(dest);
		mutex.unlock();
	}
private:
	std::list<Type*>	mem_pool;
	std::mutex			mutex;
};

