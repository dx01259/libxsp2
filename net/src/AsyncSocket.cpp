/*
 * AsyncSocket.cpp
 *
 *  Created on: 2015年8月10日
 *      Author: dengxu
 */

#include"AsyncSocket.h"
#include"../../base/src/logdef.h"
#include<fcntl.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>

CAsyncSocket::CAsyncSocket()
{
	m_socket = 0;
}

CAsyncSocket::~CAsyncSocket()
{

}

int CAsyncSocket::CreateSocket(const char *ip, const short port, int protocol)
{
	int type = (protocol!=PROTO_UDP ? SOCK_STREAM:SOCK_DGRAM);
	int sockfd = Socket(AF_INET, type, 0);
	if(sockfd > 0)
	{
		m_socket = sockfd;
		struct sockaddr_in servaddr;
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(port);
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		int ret = Bind(m_socket, (struct sockaddr *)&servaddr, sizeof(servaddr));
		SetAsyncSocket(m_socket);
		if(ret == 0 && type == SOCK_STREAM)
		{
			ret = Listen(m_socket, SOMAXCONN);
			if(0 != ret)
			{
				TRACE_ERR(LOG_ERRORS, errno, 100, "Listen the socket object is error");
				return ret;
			}
		}
		return sockfd;
	}
	return 0;
}

void CAsyncSocket::CloseSocket()
{
	if(m_socket){
		close(m_socket);
		m_socket = 0;
	}
}

int CAsyncSocket::AsyncAccept(struct sockaddr *__restrict addrp, socklen_t *__restrict addr_len)
{
	if(m_socket)
	{
		return Accept(m_socket, addrp, addr_len);
	}
	return 0;
}
ssize_t CAsyncSocket::Recv(void *buf, size_t len)
{
	if(m_socket)
	{
		return recv(m_socket, buf, len, MSG_DONTWAIT);
	}
	return 0;
}

ssize_t CAsyncSocket::Send(const char *buf, size_t len)
{
	if(m_socket)
	{
		return send(m_socket, buf, len, MSG_DONTWAIT);
	}
	return 0;
}


int CAsyncSocket::SetAsyncSocket(int sockfd)
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
		TRACE_MSG(LOG_DEBUGS, 100, "Socket %d set SOCK_NONBLOCK is success", sockfd);
		return sockfd;
	}
	return -2;
}
