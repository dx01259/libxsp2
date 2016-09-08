/*
 * CMutex.h
 *
 *  Created on: 2015年8月7日
 *      Author: dengxu
 */
#include<pthread.h>

#ifndef BASE_SRC_MUTEX_H_
#define BASE_SRC_MUTEX_H_

class CMutex
{
public:
	CMutex();
	virtual ~CMutex();
public:
	int lock();
	int unlock();
	int trylock();
	bool islocked();
private:
	bool volatile   m_blocked;
	pthread_mutex_t m_mutex;
};

template<class T>
class CAutoGuard
{
public:
	CAutoGuard(T &mutex):
		m_mutex(mutex)
	{
		m_mutex.lock();
	}
	~CAutoGuard(void)
	{
		m_mutex.unlock();
	}
protected:
	T &m_mutex;
};

#ifndef THREAD_MUTEX
#define THREAD_MUTEX CMutex
#endif

//自动锁定义
//guard：CAutoGuard的局部变量名称
//mutex: 锁的类型
//mtx：   锁的对象
#ifndef AUTO_GUARD
#define AUTO_GUARD(guard, mutex, mtx)\
	CAutoGuard<mutex> guard(mtx)
#endif

#endif /* BASE_SRC_MUTEX_H_ */
