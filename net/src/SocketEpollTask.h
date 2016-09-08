/*
 * SocketEpollTask.h
 *
 *  Created on: 2015年8月13日
 *      Author: dengxu
 */

#ifndef NET_SRC_SOCKETEPOLLTASK_H_
#define NET_SRC_SOCKETEPOLLTASK_H_

#include"../../base/src/EpollWatch.h"
#include"../../base/src/Socket.h"

class CSocketEpollTask:
		public CSocket,
		public CEpollWatch
{
public:
	CSocketEpollTask();
	virtual ~CSocketEpollTask();
public:
	inline void SetSocketObject(int sockfd)
	{
		if( m_socket == 0 ){
			this->m_socket = sockfd;
		}
	}
	inline size_t GetEpollEventsNumbers()
	{
		return m_object.maxNumbers;
	}
	inline bool TaskIsFinished()
	{
		return m_taskFinish;
	}
public:
	void	SetEpollWaitTimeout(int timeout, bool epollWait);
	void	CreateEpollObject();
	int		IoEpollWait(int timeout);
	bool	AddTask(const int sockfd, const int events);
	void	CloseTask();
public:
	static void	RunTask(void *task);
	static int	SetAsyncSocket(int sockfd);
protected:
	bool	m_taskFinish;				//任务已经结束标志
	int 	m_timeout;					//IoEpollWait函数的参数值
	int		m_socket;						//监听套接字主服务端套接字描述符
	bool	m_bCreateEpollObject;		//是否已经创建监听句柄，如果已经创建，则不再重复创建
};

#endif /* NET_SRC_SOCKETEPOLLTASK_H_ */
