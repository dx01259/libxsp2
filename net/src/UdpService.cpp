/*
 * UdpService.cpp
 *
 *  Created on: 2015年8月12日
 *      Author: dengxu
 */

#include "UdpService.h"
#include"../../base/src/logdef.h"
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>

CUdpService::CUdpService()
{
	m_bEpollWait = true;
}

CUdpService::~CUdpService()
{
	// TODO Auto-generated destructor stub
}

int CUdpService::Start(const short port, const int threadNumber, size_t buffsize)
{
	int sockfd = CreateSocket("127.0.0.1", port, PROTO_UDP);
	if(sockfd != -1)
	{
		EpollCreate();
		EpollAdd(sockfd,  EPOLLET | EPOLLIN | EPOLLOUT);
		m_IOBuffRecv.Resize(buffsize);
		for(int i=0; i<threadNumber; ++i)
		{
			thread_struct *pthread = new thread_struct();
			memset(pthread, 0, sizeof(thread_struct));
			pthread->pTasks = new CMsgDeque<thread_task *>;//在此地先分配空间的原因是，为了让线程池里面的线程在无任务时处于阻塞状态，而不是循环运行
			m_ThreadPool.Push(pthread);
		}
	}
	return 0;
}

void CUdpService::Stop()
{
	m_bEpollWait = false;
	CloseSocket();
	m_ThreadPool.Clear();
}

bool CUdpService::PushEpollWait(int sockfd, int events, int timeout, bool epollWait)
{
	CThreadTask<CSocketEpollTask> *ptask = new CThreadTask<CSocketEpollTask>;
	if(ptask){
		SetAsyncSocket(sockfd);
		ptask->Instance().SetEpollWaitTimeout(timeout, epollWait);
		ptask->Instance().AddTask(sockfd, events);
		thread_struct *pthread = NULL;
		if(m_ThreadPool.GetBest(pthread, FCFS_ALGORITHM))
		{
			m_ThreadPool.AddTask(pthread, ptask->task_proc, ptask, epollWait);
			return true;
		}
	}
	return false;
}

int CUdpService::IoEpollWait(int timeout)
{
	while(m_bEpollWait)
	{
		int ret = epoll_pwait(m_object.epollfd, &m_object.vEvents[0], m_object.maxNumbers, timeout, &m_object.siget);
		if(0 ==ret ) continue;//timeout
		else if(0 > ret)
		{
			if(EINTR==errno || EAGAIN==errno)
				continue;
			TRACE_ERR(LOG_ERRORS, errno, 100, "epoll_pwait happy error");
			return -1;
		}
		for(int i=0; i<ret && i<m_object.maxNumbers; ++i)
		{
			int clientfd = 0;
			struct sockaddr_in addr;
			socklen_t addrlen = sizeof(struct sockaddr_in);
			epoll_event &event = m_object.vEvents[i];
			if(GetSocket() == event.data.fd)
			{
				while(0<(clientfd = AsyncAccept((struct sockaddr *)&addr, &addrlen))){
					TRACE_MSG(LOG_DEBUGS, 100, "套接字: %d请求连接!", clientfd);
					PushEpollWait(clientfd, EPOLLET | EPOLLIN | EPOLLOUT, timeout, true);
				}
				if(-1==clientfd && errno!=EAGAIN &&
						ECONNABORTED!=errno && EPROTO!=errno && EINTR!=errno)
				{
					TRACE_ERR(LOG_ERRORS, errno, 100, "Accept socket is error");
				}
				continue;
			}
			else if(EPOLLIN & event.events)//read data
			{
				int fd = event.data.fd;
				string szRecvData;
				szRecvData.resize(4);
				int reLen = 0, offset=0;
				while((reLen=read(fd, &szRecvData[offset], 4)) > 0)
				{
					offset += reLen;
					szRecvData.resize(offset+1024);
				}
				if((reLen < 0 && errno==ECONNRESET)
						|| (0 == reLen) )
				{
					close(fd);
					TRACE_MSG(LOG_DEBUGS, 100, "套接字: %d主动关闭连接!", fd);
					EpollDel(fd,  EPOLLET | EPOLLIN );
					continue;
				}
				else if(reLen < 0) continue;
				szRecvData.resize(offset);
				//this->m_deque.PushBack(szRecvData);
				TRACE_MSG(LOG_DEBUGS, 100,"recv %d from %d", szRecvData.c_str(), fd);
			}
			else if(EPOLLOUT & event.events)//write data
			{
				int fd = event.data.fd;
				string szSendData = (char *)event.data.ptr;
				int datalen = szSendData.size(), sendlen=0;
				while(0 < datalen)
				{
					sendlen=write(fd, szSendData.c_str(), datalen);
					if(sendlen < 0)
					{
						if(-1 == sendlen && errno != EAGAIN)
						{
							TRACE_ERR(LOG_ERRORS, errno, 100, "Send %s is error", szSendData.c_str());
							continue;
						}
						break;
					}
					datalen -= sendlen;
				}
				TRACE_MSG(LOG_DEBUGS, 20+szSendData.size(), "Send %s is success");
			}
		}
	}

	return 0;
}

ssize_t CUdpService::Parse(const byte *data, size_t len)
{
	return 0;
}

ssize_t CUdpService::Recv(void *buf, size_t len)
{
	return 0;
}

ssize_t CUdpService::Send(const char *buf, size_t len)
{
	return 0;
}
