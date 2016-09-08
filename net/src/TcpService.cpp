/*
 * TcpService.cpp
 *
 *  Created on: 2015年8月12日
 *      Author: dengxu
 */

#include"TcpService.h"
#include"../../base/src/logdef.h"
#include"../../base/src/commfunc.h"
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>

#include<vector>
using namespace std;

CTcpService::CTcpService()
{
	m_bEpollWait	= true;
	m_timeout  	= 0;
	m_threadID	= 0;
	check_zero(pthread_attr_init(&m_attr));
	check_zero(pthread_attr_setscope(&m_attr, PTHREAD_SCOPE_SYSTEM));
	check_zero(pthread_attr_setdetachstate(&m_attr, PTHREAD_CREATE_DETACHED));
}

CTcpService::~CTcpService()
{
	pthread_attr_destroy(&m_attr);
}

int CTcpService::Start(const short port, const int epollNumber, size_t buffsize)
{
	int sockfd = CreateSocket("127.0.0.1", port);
	if(sockfd > 0)
	{
		check_zero(pthread_create(&m_threadID, &m_attr, thread_proc, this));
		m_IOBuffRecv.Resize(buffsize);
		for(int i=0; i<epollNumber; ++i)
		{
			//创建IO监听复用句柄
			CThreadTask<CSocketEpollTask> *ptask = new CThreadTask<CSocketEpollTask>;
			m_Task.push_back(ptask);

			//创建线程数
			thread_struct *pthread = new thread_struct();
			memset(pthread, 0, sizeof(thread_struct));
			pthread->pTasks = new CMsgDeque<thread_task *>;//在此地先分配空间的原因是，为了让线程池里面的线程在无任务时处于阻塞状态，而不是循环运行
			m_ThreadPool.Push(pthread);
		}
	}
	return 0;
}

void CTcpService::Stop()
{
	m_bEpollWait = false;
	CloseSocket();
	m_ThreadPool.Clear();
}

bool CTcpService::PushSubEpollWait(int sockfd, int events, int timeout, bool permanent)
{
	SetAsyncSocket(sockfd);
	CThreadTask<CSocketEpollTask> *ptask = NULL;
	thread_struct *pthread = NULL;
	if((pthread = m_ThreadPool.GetBest(pthread, FCFS_ALGORITHM)))
	{
		ptask = GetBestWatchEpollObject();
		if(ptask)
		{
			ptask->Instance().SetSocketObject(GetSocket());
			ptask->Instance().SetEpollWaitTimeout(timeout, permanent);
			ptask->Instance().AddTask(sockfd, events);
			m_Task.push_back(ptask);
			m_ThreadPool.AddTask(pthread, ptask->task_proc, ptask, permanent);
			TRACE_MSG(LOG_DEBUGS, 100, "向线程池子线程: %x添加任务成功！", pthread->thread);
			return true;
		}
		else{
			TRACE_MSG(LOG_DEBUGS, 100, "有多余未分配任务的线程，却没有多余的IO复用监听句柄");
		}
	}
	else{
		//当分配线程时是否失败，如果失败，则将需要监听的套接字，发送到其他已经分配的线程中。
		ptask = GetMinWatchEpollObject();
		if(ptask)
		{
			ptask->Instance().AddTask(sockfd, events);
		}
		return true;
	}
	TRACE_MSG(LOG_DEBUGS, 100, "向线程池中获取最优调度子线程失败！");
	return false;
}

void CTcpService::RemoveSocketEpollTask()
{
	AUTO_GUARD(g, THREAD_MUTEX, m_mutex);

	vector<CThreadTask<CSocketEpollTask> *>::iterator iter = m_Task.begin();
	for(; iter != m_Task.end(); ++iter)
	{
		CThreadTask<CSocketEpollTask> *ptask = *iter;
		if(ptask->Instance().TaskIsFinished())
		{
			m_Task.erase(iter);
			delete_memory(ptask);
		}
	}
}

void CTcpService::ClearSocketEpollTask()
{
	AUTO_GUARD(g, THREAD_MUTEX, m_mutex);

	vector<CThreadTask<CSocketEpollTask> *>::iterator iter = m_Task.begin();
	for(; iter != m_Task.end(); ++iter)
	{
		CThreadTask<CSocketEpollTask> *ptask = *iter;
		delete_memory(ptask);
	}
	m_Task.clear();
}

CThreadTask<CSocketEpollTask> *CTcpService::GetBestWatchEpollObject()
{
	CThreadTask<CSocketEpollTask> *ptask = NULL;
	//优先选择没有被分配任务的IO复用对象
	for(int i=0; i<m_Task.size(); ++i)
	{
		if(m_Task[i]->Instance().TaskIsFinished())
		{
			ptask = m_Task[i];
			break;
		}
	}
	return ptask;
}
CThreadTask<CSocketEpollTask> *CTcpService::GetMinWatchEpollObject()
{
	CThreadTask<CSocketEpollTask> *ptask = NULL;
	//如果IO复用对象都已经分配，则寻找最少监听IO Socket对象的IO复用对象
	vector<CThreadTask<CSocketEpollTask> *>::iterator iter = m_Task.begin();
	for(ptask=(*iter); iter != m_Task.end(); ++iter)
	{
		if(ptask->Instance().GetEpollEventsNumbers() > (*iter)->Instance().GetEpollEventsNumbers()
				&& !ptask->Instance().TaskIsFinished())
		{
			ptask = *iter;
		}
	}
	return ptask;
}

int CTcpService::IoEpollWait(int timeout)
{
	TRACE_MSG(LOG_DEBUGS, 100, "epoll listen socket begin !");
	while(m_bEpollWait)
	{
		SafeChangeEvents();//此函数必须添加，否则会发生段错误。
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
			socklen_t addrlen = sizeof(struct sockaddr);
			epoll_event &event = m_object.vEvents[i];
			if(GetSocket() == event.data.fd)
			{
				while(0<(clientfd = AsyncAccept((struct sockaddr *)&addr, &addrlen))){
					TRACE_MSG(LOG_DEBUGS, 100, "套接字: %d请求连接!", clientfd);
					PushSubEpollWait(clientfd, EPOLLET | EPOLLIN, timeout, true);
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
				while((reLen=Read(fd, &szRecvData[offset], 4)) > 0 )
				{
					offset += reLen;
					szRecvData.resize(offset+1024);
				}
				if((reLen < 0 && errno==ECONNRESET)
						|| (0 == reLen) )
				{
					TRACE_MSG(LOG_DEBUGS, 100, "套接字: %d主动关闭连接!", fd);
					EpollDel(fd,  EPOLLET | EPOLLIN);
					close(fd);
					continue;
				}
				else if(reLen <0 && reLen != -2) continue;
				szRecvData.resize(offset);
				//this->m_deque.PushBack(szRecvData);
				TRACE_MSG(LOG_DEBUGS, 100,"recv %s from %d", szRecvData.c_str(), fd);
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

	TRACE_MSG(LOG_DEBUGS, 100, "epoll listen socket end !");

	return 0;
}

void *CTcpService::thread_proc(void *arg)
{
	CTcpService *pThis = (CTcpService *)arg;
	cleanup_push_thread(clean_func, pThis);
	if(pThis)
	{
		pThis->EpollCreate();
		pThis->EpollAdd(pThis->GetSocket(),  EPOLLET | EPOLLIN );
		pThis->IoEpollWait(-1);
	}
	cleanup_pop_thread(1);
	return (void *)1;
}

void CTcpService::clean_func(void *arg)
{
	CTcpService *pThis = (CTcpService *)arg;
	if(pThis)
	{
		pThis->ClearSocketEpollTask();
	}
}

ssize_t CTcpService::Recv(void *buf, size_t len)
{
	return 0;
}

ssize_t CTcpService::Send(const char *buf, size_t len)
{
	return 0;
}
