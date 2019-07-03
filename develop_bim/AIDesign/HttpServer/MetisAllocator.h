/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:内存池模板类
*  简要描述:
*
*  当前版本:
*  作者:冯小翼
*  创建日期:2018-09-12
*  说明: MetisAllocator
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

