/*
 * ThreadPool.h
 *
 *  Created on: 2015年8月7日
 *      Author: dengxu
 */

#ifndef BASE_SRC_THREADPOOL_H_
#define BASE_SRC_THREADPOOL_H_

#include<pthread.h>
#include<semaphore.h>
#include"ThreadDefine.h"
#include"Mutex.h"
#include"TMap.h"

#include<deque>
using namespace std;


class CThreadPool
{
public:
	CThreadPool();
	virtual ~CThreadPool();
public:
	thread_struct *AddTask(struct thread_struct *pthread, pthread_func func, void *arg, bool singleTask=false);
	int Push(struct thread_struct *pthread);
	void Pop();
	thread_struct *Front(thread_struct *pthread);
	thread_struct *GetBest(thread_struct *pthread, int scheduling/*获取最优线程的调度算法*/);
	bool Reset(pthread_t id, struct thread_struct *pthread);
	void Remove(const pthread_t thread);
	void RemoveThreadReady(const pthread_t thread);
	void RemoveThreadRunning(const pthread_t thread);
	void Clear();
protected:
	thread_struct *schedulingFCFS();	//先到先服务优先调度算法
	thread_struct *schedulingHRP();		//高响应比优先调度算法
protected:
	static void *thread_proc(void *arg);
	static void clean_func(void *arg);
protected:
	sem_t		m_ReadyRunning;			//线程就绪队列和运行队列的缓存更新事件
	CMutex		m_mutex;					//线程之间的同步
	pthread_t m_threadID;				//线程池的内部线程句柄
	pthread_attr_t m_attr;				//线程的属性
private:
	CTMap<pthread_t, struct thread_struct *> m_ThreadPool;
	deque<pthread_t>	m_ThreadIDS;		//所有线程队列
	deque<pthread_t>	m_ThreadReady;	//线程的就绪队列
	deque<pthread_t>	m_ThreadRunning;//线程正在运行队列
};

#endif /* BASE_SRC_THREADPOOL_H_ */
