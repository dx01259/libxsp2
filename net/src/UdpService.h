/*
 * UdpService.h
 *
 *  Created on: 2015年8月12日
 *      Author: dengxu
 */

#ifndef NET_SRC_UDPSERVICE_H_
#define NET_SRC_UDPSERVICE_H_

#include"AsyncSocket.h"
#include"../../base/src/EpollWatch.h"
#include"../../base/src/ThreadPool.h"
#include"../../base/src/IOBufferPtr.h"
#include"../../base/src/ThreadTask.h"
#include"SocketEpollTask.h"
#include<semaphore.h>

class CUdpService:
		public CAsyncSocket,
		public CEpollWatch
{
public:
	CUdpService();
	virtual ~CUdpService();
public:
	int Start(const short port, const int threadNumber, size_t buffsize=1024*1024/*1KB*/);
	void Stop();
	int IoEpollWait(int timeout);
	ssize_t Parse(const byte *data, size_t len);
	ssize_t Recv(void *buf, size_t len);
	ssize_t Send(const char *buf, size_t len);
protected:
	bool PushEpollWait(int sockfd, int events, int timeout, bool epollWait=false);
protected:
	vector<CThreadTask<CSocketEpollTask> *> m_Task;
private:
	sem_t m_semt;						//发送信号
	CThreadPool m_ThreadPool;			//线程池，用来并发处理IO事件
	CIOBufferPtr m_IOBuffRecv;		//TCP服务接收的数据缓存
};

#endif /* NET_SRC_UDPSERVICE_H_ */
