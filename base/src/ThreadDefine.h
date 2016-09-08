/*
 * ThreadDefine.h
 *
 *  Created on: 2015年8月8日
 *      Author: dengxu
 */

#ifndef BASE_SRC_THREADDEFINE_H_
#define BASE_SRC_THREADDEFINE_H_

#include "MsgDeque.h"

typedef void *(*pthread_func)(void *);//线程回调函数指针

struct thread_struct;
struct thread_task;

//线程的运行状态
enum thread_status
{
	IS_SLEEP=0,				//线程休眠中
	IS_RUNNING,				//线程运行中
	IS_WAITING,				//线程等待中
	IS_EXITED,				//线程已退出
};

//线程的调度算法
enum SCHEDULING_ALGORITHM
{
	FCFS_ALGORITHM=0,		//先到先服务优先调度算法
	HRP_ALGORITHM=1,		//高响应比优先调度算法
};


//线程池线程单元结构体
struct thread_struct
{
	pthread_t			thread;				//线程ID
	size_t				busying;				//线程忙碌的程度
	thread_status	status;				//线程的运行状态
	CMsgDeque<thread_task *>	*pTasks;	//线程对应的任务处理
};

//线程对应的任务处理结构体ls
struct thread_task
{
	pthread_func	function;		//线程调用的函数
	void			*parameter;	//线程传递的参数
};

#ifdef pthread_cleanup_push_defer_np
#define cleanup_push_thread(clean_fun, arg) \
	pthread_cleanup_push_defer_np(clean_fun, arg)
#define cleanup_pop_thread pthread_cleanup_pop_restore_np
#else
#define cleanup_push_thread(clean_fun, arg)\
do{\
	if(thrp)\
	{\
		int oldtype, ret;\
		ret = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);\
		if(0 == ret)\
		{\
			pthread_cleanup_push(clean_fun, arg)

#define cleanup_pop_thread(execute)\
			pthread_cleanup_pop(execute);\
			pthread_setcanceltype(oldtype, NULL);\
		}\
	}\
}while(0)
#endif



#endif /* BASE_SRC_THREADDEFINE_H_ */
