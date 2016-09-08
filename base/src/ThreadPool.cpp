/*
 * CThreadPool.cpp
 *
 *  Created on: 2015年8月7日
 *      Author: dengxu
 */

#include"ThreadPool.h"
#include"commfunc.h"
#include"logdef.h"

CThreadPool::CThreadPool()
{
	check_zero(sem_init(&m_ReadyRunning, 0, 0));
	check_zero(pthread_attr_init(&m_attr));
	check_zero(pthread_attr_setscope(&m_attr, PTHREAD_SCOPE_SYSTEM));
	check_zero(pthread_attr_setdetachstate(&m_attr, PTHREAD_CREATE_DETACHED));
	check_zero(pthread_create(&m_threadID, &m_attr, thread_proc, this));
}

CThreadPool::~CThreadPool()
{
	sem_destroy(&m_ReadyRunning);
	pthread_attr_destroy(&m_attr);
	Clear();
}

thread_struct *CThreadPool::AddTask(struct thread_struct *pthread, pthread_func func, void *arg, bool singleTask)
{
	printf("Hello 0\n");
	if(pthread)
	{
		printf("Hello 1\n");
		pthread->busying = singleTask ? -1:0;//如果时单一任务，就将忙碌级别设置最大值
		pthread->status  = IS_WAITING;
		if(!pthread->pTasks)
		{
			printf("Hello 2\n");
			pthread->pTasks = new CMsgDeque<thread_task *>;
		}
		thread_task *ptask = new thread_task();
		if(ptask)
		{
			ptask->function  = func;
			ptask->parameter = arg;
		}
		printf("Hello 3\n");
		pthread->pTasks->PushBack(ptask);
	}
	printf("Hello 4\n");
	return pthread;
}

int CThreadPool::Push(struct thread_struct *pthread)
{
	if(pthread)
	{
		AUTO_GUARD(g, THREAD_MUTEX, m_mutex);

		pthread_t pid;
		int ret = pthread_create(&pid, &m_attr, thread_proc, this);
		if(0 != ret)
		{
			TRACE_ERR(LOG_ERRORS, errno, 200, "线程池创建子线程失败");
			return ret;
		}
		pthread->thread = pid;
		pthread->status = IS_WAITING;	//创建新的子线程时必须确线程状态是等待
		m_ThreadPool.set(pid, pthread);	//保存线程对象
		m_ThreadIDS.push_back(pid);		//保存所有有效线程ID
		m_ThreadReady.push_back(pid);	//发送保存处于等待状态线程ID的信号
		return 0;
	}
	return -1;
}

void CThreadPool::Pop()
{
	AUTO_GUARD(g, THREAD_MUTEX, m_mutex);

	thread_struct *pthread = NULL;
	pthread_t pid = m_ThreadIDS.front();
	if(m_ThreadPool.find(pid, pthread) == 0)
	{
		pthread_cancel(pthread->thread);
	}

}

struct thread_struct *CThreadPool::Front(thread_struct *pthread)
{
	AUTO_GUARD(g, THREAD_MUTEX, m_mutex);

	pthread_t pid = m_ThreadIDS.front();
	if(m_ThreadPool.find(pid, pthread) == 0)
	{
		return pthread;
	}

	return 0;
}

struct thread_struct *CThreadPool::GetBest(thread_struct *pthread, int scheduling)
{
	switch(scheduling)
	{
	case FCFS_ALGORITHM:pthread=schedulingFCFS();	break;
	case HRP_ALGORITHM :pthread=schedulingHRP();		break;
	default:											return NULL;
	}
	sem_post(&m_ReadyRunning);
	return pthread;
}

bool CThreadPool::Reset(pthread_t id, struct thread_struct *pthread)
{
	AUTO_GUARD(g, THREAD_MUTEX, m_mutex);

	if(pthread)
	{
		thread_struct *pRetPthread = NULL;
		if(m_ThreadPool.find(id, pRetPthread) == 0)
		{
			memcpy(pRetPthread, pthread, sizeof(pRetPthread));
			return m_ThreadPool.set(id, pRetPthread)==0 ? true:false;
		}
	}
	return false;
}

void CThreadPool::Remove(const pthread_t thread)
{
	AUTO_GUARD(g, THREAD_MUTEX, m_mutex);

	deque<pthread_t>::iterator iter = m_ThreadIDS.begin();
	for(; iter != m_ThreadIDS.end(); ++iter)
	{
		pthread_t &pthid = *iter;
		if(pthid == thread)
		{
			pthread_cancel(pthid);
			break;
		}
	}
}

void CThreadPool::RemoveThreadReady(const pthread_t thread)
{
	deque<pthread_t>::iterator iter = m_ThreadReady.begin();
	for(; iter != m_ThreadReady.end(); ++iter)
	{
		if(*iter == thread)
		{
			m_ThreadReady.erase(iter);
			break;
		}
	}
}

void CThreadPool::RemoveThreadRunning(const pthread_t thread)
{
	deque<pthread_t>::iterator iter = m_ThreadRunning.begin();
	for(; iter != m_ThreadRunning.end(); ++iter)
	{
		if(*iter == thread)
		{
			m_ThreadRunning.erase(iter);
			break;
		}
	}
}

void CThreadPool::Clear()
{
	AUTO_GUARD(g, THREAD_MUTEX, m_mutex);

	vector<thread_struct *> vThreadPool;
	m_ThreadPool.get_all_values(vThreadPool);
	for(size_t i=0; i<vThreadPool.size(); ++i)
	{
		thread_struct *pthread = vThreadPool[i];
		pthread_cancel(pthread->thread);
	}
}

void *CThreadPool::thread_proc(void *arg)
{
	void *ret = NULL;
	CThreadPool *pool =  (CThreadPool *)arg;
	cleanup_push_thread(clean_func, pool);
	while(pool)
	{
		thread_struct *pthread = NULL;
		if(0 == pool->m_ThreadPool.find(pthread_self(), pthread))
		{
			//AUTO_GUARD(g, THREAD_MUTEX, pool->m_mutex);不能放在这里的原因是如果某个线程没有任务，就会一直等待下去，从而把其他线程都阻塞在此地
			thread_task *ptask = NULL;
			if(pthread->pTasks)
			{
				TRACE_MSG(LOG_DEBUGS, 100, "线程池中线程:%x开始获取任务！", pthread_self());
				while(pthread->pTasks->PopFront(ptask, -1))
				{
					if(ptask->function){
						{
							AUTO_GUARD(g, THREAD_MUTEX, pool->m_mutex);//放在此处，能解决上诉问题
							pthread->status = IS_RUNNING;
							pool->m_ThreadPool.set(pthread_self(), pthread);
							sem_post(&pool->m_ReadyRunning);
						}//在此加括号的目的是为了限制加锁的释放时间和区域，避免执行任务函数时，函数内部有无限循环，从而阻塞其他线程
						ptask->function(ptask->parameter);
						delete_memory(ptask);//任务运行完之后需要释放申请到的内存
					}

					AUTO_GUARD(g, THREAD_MUTEX, pool->m_mutex);//放在此处，能解决上诉问题
					pthread->status = IS_WAITING;
					if(pthread->busying == -1){
						pthread->busying = 0;
					}
					else{
						--pthread->busying;
					}
					pool->m_ThreadPool.set(pthread_self(), pthread);
					sem_post(&pool->m_ReadyRunning);
				}
			}
			TRACE_MSG(LOG_DEBUGS, 100, "线程池中线程:%x处理任务结束！", pthread_self());
		}
		else{
			//线程池内部逻辑处理线程
			int ret = sem_wait(&pool->m_ReadyRunning);
			if(-1==ret && errno!=EINTR && errno!=ETIMEDOUT)
			{
				TRACE_ERR(LOG_ERRORS, errno, 100,"线程池内部线程在等待信号时发生错误");
			}
			else if(0 == ret)
			{
				AUTO_GUARD(g, THREAD_MUTEX, pool->m_mutex);
				vector<thread_struct *> vThreadPools;
				pool->m_ThreadReady.clear();
				pool->m_ThreadRunning.clear();
				pool->m_ThreadPool.get_all_values(vThreadPools);
				for(int i=0; i<vThreadPools.size(); ++i)
				{
					if(vThreadPools[i]->status==IS_WAITING)
					{
						pool->m_ThreadReady.push_back(vThreadPools[i]->thread);
					}
					else if(vThreadPools[i]->status==IS_RUNNING)
					{
						pool->m_ThreadRunning.push_back(vThreadPools[i]->thread);
					}
				}
				TRACE_MSG(LOG_DEBUGS, 100, "更新线程池里面的就绪队列和运行队列容器");
			}
		}
	}
	cleanup_pop_thread(1);
	return NULL;
}

void CThreadPool::clean_func(void *arg)
{
	CThreadPool *pool =  (CThreadPool *)arg;
	if(pool)
	{
		AUTO_GUARD(g, THREAD_MUTEX, pool->m_mutex);

		thread_struct *pthread = NULL;
		if(0 == pool->m_ThreadPool.find(pthread_self(), pthread))
		{
			for(size_t i=0; i<pthread->pTasks->Size(); ++i)
			{
				thread_task *ptask=NULL;
				if(pthread->pTasks->PopFront(ptask)){
					delete_memory(ptask);
				}
			}

			if(pthread->status == IS_WAITING){
				pool->RemoveThreadReady(pthread->thread);
			}
			else if(pthread->status == IS_RUNNING){
				pool->RemoveThreadRunning(pthread->thread);
			}

			delete_memory(pthread->pTasks);
			delete_memory(pthread);
			pool->m_ThreadPool.remove(pthread_self());

			TRACE_MSG(LOG_DEBUGS, 100, "子线程%x退出！", pthread_self());
		}
		else{
			//线程池内部线程清理操作
			TRACE_MSG(LOG_DEBUGS, 100, "主线%x退出，则同时清除所有子线程！", pthread_self());
			pool->Clear();//主线程退出，其余的子线程也必须退出
		}
	}
}

thread_struct *CThreadPool::schedulingFCFS()
{
	AUTO_GUARD(g, THREAD_MUTEX, m_mutex);

	thread_struct *pthread = NULL;
	if(!m_ThreadReady.empty())
	{
		pthread_t thread = m_ThreadReady.front();
		if(m_ThreadPool.find(thread, pthread)==0)
		{
			if(pthread->busying != -1)
			{
				m_ThreadReady.pop_front();
				m_ThreadRunning.push_back(thread);
				pthread->status = IS_RUNNING;
				m_ThreadPool.set(thread, pthread);
			}
			else{
				pthread = schedulingHRP();//先到先服务优先调度算法失败时，直接调用高响应比优先调度算法
			}
		}
	}
	return pthread;
}

thread_struct *CThreadPool::schedulingHRP()
{
	AUTO_GUARD(g, THREAD_MUTEX, m_mutex);

	size_t i = 0;
	thread_struct *pthread = NULL;
	vector<thread_struct *> vThreadPools;
	m_ThreadPool.get_all_values(vThreadPools);
	for(; i<vThreadPools.size(); ++i)
	{
		if(vThreadPools[i]->busying == 0)
		{
			pthread = vThreadPools[i];
			pthread_t thrID = vThreadPools[i]->thread;
			m_ThreadReady.pop_front();
			m_ThreadRunning.push_back(thrID);
			pthread->status = IS_RUNNING;
			m_ThreadPool.set(thrID, pthread);
		}
		else if(vThreadPools[i]->busying == -1){
			continue;
		}
		else{
			i == 0 ? pthread = vThreadPools[0]:0;
			if(pthread->busying > vThreadPools[i]->busying)
			{
				pthread = vThreadPools[i];
			}
		}
	}
	if( i == vThreadPools.size() &&
			pthread->busying != -1 )
	{
		pthread_t thrID = pthread->thread;
		m_ThreadReady.pop_front();
		m_ThreadRunning.push_back(thrID);
		pthread->status = IS_RUNNING;
		m_ThreadPool.set(thrID, pthread);
	}
	else{
		pthread = NULL;
	}

	return pthread;
}



