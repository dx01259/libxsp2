/*
 * SocketEpollTask.cpp
 *
 *  Created on: 2015年8月13日
 *      Author: dengxu
 */

#include"SocketEpollTask.h"
#include"../../base/src/logdef.h"
#include<unistd.h>
#include<fcntl.h>
#include<netinet/in.h>

CSocketEpollTask::CSocketEpollTask()
{
	m_taskFinish = true;
	m_bEpollWait	= false;
	m_timeout 	= 0;
	m_socket  	= 0;
	m_bCreateEpollObject = false;
}

CSocketEpollTask::~CSocketEpollTask()
{
	// TODO Auto-generated destructor stub
}

void CSocketEpollTask::SetEpollWaitTimeout(int timeout, bool epollWait)
{
	this->m_timeout 	= timeout;
	this->m_bEpollWait	= epollWait;
}

int CSocketEpollTask::SetAsyncSocket(int sockfd)
{
	if(sockfd > 0)
	{
		int flags = fcntl(sockfd, F_GETFL, 0);
		if(-1 == flags)
		{
			TRACE_ERR(LOG_ERRORS, errno, 100, "fcntl change the socket object is error");
			return -1;
		}
		flags |= SOCK_NONBLOCK;
		flags = fcntl(sockfd, F_SETFL, flags);
		if(-1 == flags)
		{
			TRACE_ERR(LOG_ERRORS, errno, 100, "fcntl change the socket object is error");
			return -1;
		}
		return sockfd;
	}
	return -2;
}

int CSocketEpollTask::IoEpollWait(int timeout)
{
	TRACE_MSG(LOG_DEBUGS, 100, "epoll listen socket begin !");
	while(m_bEpollWait)
	{
		m_taskFinish = false;
		SafeChangeEvents();//此地方必须添加，否着程序会有段错误
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
			//TRACE_MSG(LOG_DEBUGS, 100, "套接字: %d,本地套接字: %d", event.data.fd, m_socket);
			if(m_socket == event.data.fd)
			{
				while(0<(clientfd = Accept(m_socket, (struct sockaddr *)&addr, &addrlen))){
					TRACE_MSG(LOG_DEBUGS, 100, "套接字: %d请求连接!", clientfd);
					SetAsyncSocket(clientfd);
					EpollAdd(clientfd, EPOLLET|EPOLLIN);
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
				szRecvData.resize(1025);
				int reLen = 0, offset=0;
				while((reLen=Read(fd, &szRecvData[offset], 1024)) > 0)
				{
					offset += reLen;
					szRecvData.resize(offset+1024);
				}
				if((reLen < 0 && errno==ECONNRESET)
						|| (0 == reLen) )
				{
					TRACE_MSG(LOG_DEBUGS, 100, "套接字: %d主动关闭连接!", fd);
					EpollDel(fd,  EPOLLET | EPOLLIN );
					close(fd);
					//判断线程监听的套接字是否全部关闭，如果是，则退出线程。
					if(m_object.maxNumbers <=0){
						m_bEpollWait     = false;
						break;
					}
					continue;
				}
				else if(reLen < 0 && reLen != -2) {
					continue;
				}
				szRecvData.resize(offset);
				//this->m_deque.PushBack(szRecvData);
				//TRACE_MSG(LOG_DEBUGS, 100,"recv %s from %d", szRecvData.c_str(), fd);
			}
			else if(EPOLLOUT & event.events)//write data
			{
				int fd = event.data.fd;
				string szSendData = (char *)event.data.ptr;
				int datalen = szSendData.size(), sendlen=0;
				while(0 < datalen)
				{
					sendlen=Write(fd, szSendData.c_str(), datalen);
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

				TRACE_MSG(LOG_DEBUGS, 20+szSendData.size(), "Send %s is success", szSendData.c_str());
			}
		}
	}

	m_taskFinish = true;
	TRACE_MSG(LOG_DEBUGS, 100, "epoll listen socket end !");

	return 0;
}

void CSocketEpollTask::CreateEpollObject()
{
	if(!this->m_bCreateEpollObject)
	{
		m_bCreateEpollObject = EpollCreate()>0;
	}
}

bool CSocketEpollTask::AddTask(const int sockfd, const int events)
{
	if(!this->m_bCreateEpollObject)
	{
		CreateEpollObject();
	}
	EpollAdd(sockfd, events);
}

void CSocketEpollTask::RunTask(void *task)
{
	CSocketEpollTask *pThis = (CSocketEpollTask *)task;
	if(pThis){
		pThis->IoEpollWait(pThis->m_timeout);
	}
}

void CSocketEpollTask::CloseTask()
{
	this->m_timeout = 0;
	this->m_bEpollWait = false;
}
