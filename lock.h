//example:
//void MyFunc()
//{
//	//进入函数，_lock被创建，调用构造函数，加锁
//	CLockRegion _lock(&lock);
//
//	//do something
//
//	//函数退出，_lock被析构，解锁
//}

#pragma once

#include <windows.h>

class CLock
{
public:
	CLock(void)	{InitializeCriticalSection(&cs);}

	~CLock(void){DeleteCriticalSection(&cs);}

	inline void Lock()	{EnterCriticalSection(&cs);	}

	inline void UnLock(){LeaveCriticalSection(&cs);	}

private:
	CRITICAL_SECTION cs;
};

class CLockRegion
{
public:
	CLockRegion(CLock* lock) : m_lock(lock)	{ m_lock->Lock();}
	~CLockRegion()	{m_lock->UnLock();	}

private:
	CLock* m_lock;
};

extern CLock lock;
