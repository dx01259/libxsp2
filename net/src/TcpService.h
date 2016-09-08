/*
 * TcpService.h
 *
 *  Created on: 2015年8月12日
 *      Author: dengxu
 */

#ifndef NET_SRC_TCPSERVICE_H_
#define NET_SRC_TCPSERVICE_H_

#include"AsyncSocket.h"
#include"../../base/src/EpollWatch.h"
#include"../../base/src/ThreadPool.h"
#include"../../base/src/IOBufferPtr.h"
#include"../../base/src/ThreadTask.h"
#include"../../base/src/Mutex.h"

#include"SocketEpollTask.h"
#include<semaphore.h>

class CTcpService:
		public CAsyncSocket,
		public CEpollWatch
{
public:
	CTcpService();
	virtual ~CTcpService();
public:
	int Start(const short port, const int epollNumber=2, size_t buffsize=1024*1024/*1KB*/);
	void Stop();
	int IoEpollWait(int timeout);
	ssize_t Recv(void *buf, size_t len);
	ssize_t Send(const char *buf, size_t len);
protected:
	bool PushSubEpollWait(int sockfd, int events, int timeout, bool permanent=false);
	CThreadTask<CSocketEpollTask> *GetBestWatchEpollObject();
	CThreadTask<CSocketEpollTask> *GetMinWatchEpollObject();
	void RemoveSocketEpollTask();
	void ClearSocketEpollTask();
protected:
	static void *thread_proc(void *arg);
	static void  clean_func(void *arg);
protected:
	vector<CThreadTask<CSocketEpollTask> *> m_Task;
private:
	int		m_timeout;					//epoll_pwait函数等待时间的参数
	CMutex	m_mutex;						//进程内部线程同步句柄
	pthread_t m_threadID;				//TCP服务的内部线程句柄
	pthread_attr_t m_attr;				//线程的属性
	CThreadPool	m_ThreadPool;		//线程池，用来并发处理IO事件
	CIOBufferPtr	m_IOBuffRecv;		//TCP服务接收的数据缓存
};

#endif /* NET_SRC_TCPSERVICE_H_ */
