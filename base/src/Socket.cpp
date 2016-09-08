/*
 * Socket.cpp
 *
 *  Created on: 2015年8月10日
 *      Author: dengxu
 */

#include"Socket.h"
#include"logdef.h"
#include<unistd.h>


CSocket::CSocket()
{
	// TODO Auto-generated constructor stub

}

CSocket::~CSocket()
{
	// TODO Auto-generated destructor stub
}

int CSocket::Socket(int domain, int type, int protocol)
{
	int sockfd = socket(domain, type, protocol);
	if(-1 == sockfd)
	{
		TRACE_ERR(LOG_ERRORS, errno, 100, "Create socket object is error");
	}
	return sockfd;
}

int CSocket::Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	int ret = setsockopt(fd, level, optname, optval, optlen);
	if(-1 == ret)
	{
		TRACE_ERR(LOG_ERRORS, errno, 100, "Setsockopt socket of %d is error", fd);
	}
	return ret;
}

int CSocket::Bind(int fd, const struct sockaddr * addrp, socklen_t len)
{
	int ret = bind(fd, addrp, len);
	if(-1 == ret)
	{
		TRACE_ERR(LOG_ERRORS, errno, 100, "Bind socket of %d is error", fd);
	}
	return ret;
}

int CSocket::Listen(int fd, int n)
{
	int ret = listen(fd, n);
	if(-1 == ret)
	{
		TRACE_ERR(LOG_ERRORS, errno, 100, "Listen socket of %d is error", fd);
	}
	return ret;
}

int CSocket::Accept(int fd, struct sockaddr *__restrict addrp, socklen_t *__restrict addr_len)
{
	int ret = accept(fd, addrp, addr_len);
	if(-1 == ret)
	{
		if(errno!=EAGAIN &&
			errno!=EWOULDBLOCK &&
			errno!=EINTR &&
			errno!=ECONNABORTED)
		{
			TRACE_ERR(LOG_ERRORS, errno, 100, "Accept socket of %d is error", fd);
			return ret;
		}
		return -2;
	}
	return ret;
}

int CSocket::Connect(int fd, const struct sockaddr * addrp, socklen_t len)
{
	int ret = connect(fd, addrp, len);
	if(-1 == ret)
	{
		if(errno!=EINTR &&
			errno!=EAGAIN)
		{
			TRACE_ERR(LOG_ERRORS, errno, 100, "Connect socket of %d is error", fd);
			return ret;
		}
		return -2;
	}
	return ret;
}

int CSocket::Close(int fd)
{
	int ret = close(fd);
	if(-1 == ret)
	{
		if(errno!=EINTR)
		{
			TRACE_ERR(LOG_ERRORS, errno, 100, "Close socket of %d is error", fd);
			return ret;
		}
		return -2;
	}
	return ret;
}

ssize_t CSocket::Recvfrom(int fd, void *__restrict buf, size_t n, int flags,
			struct sockaddr *__restrict addrp, socklen_t *__restrict addr_len)
{
	ssize_t ret = recvfrom(fd, buf, n, flags, addrp, addr_len);
	if(-1 == ret)
	{
		if(errno!=EAGAIN &&
			errno!=EWOULDBLOCK &&
			errno!=EINTR &&
			errno!=ECONNABORTED)
		{
			TRACE_ERR(LOG_ERRORS, errno, 100, "Recvfrom datas from socket %d happen error", fd);
			return ret;
		}
		return -2;
	}
	return ret;
}

ssize_t CSocket::Recv(int fd, void *buf, size_t n, int flags)
{
	ssize_t ret = recv(fd, buf, n, flags);
	if(-1 == ret)
	{
		if(errno!=EAGAIN &&
			errno!=EWOULDBLOCK &&
			errno!=EINTR &&
			errno!=ECONNABORTED)
		{
			TRACE_ERR(LOG_ERRORS, errno, 100, "Recv datas from socket %d happen error", fd);
			return ret;
		}
		return -2;
	}
	return ret;
}

ssize_t CSocket::Sendto(int fd, __const void *buf, size_t n, int flags, const struct sockaddr *addrp,
			       socklen_t addr_len)
{
	int ret = sendto(fd, buf, n, flags, addrp, addr_len);
	if(-1 == ret)
	{
		if(errno!=EAGAIN &&
			errno!=EWOULDBLOCK &&
			errno!=EINTR &&
			errno!=ECONNABORTED)
		{
			TRACE_ERR(LOG_ERRORS, errno, 100, "Sendto datas to socket %d happen error", fd);
			return ret;
		}
		return -2;
	}
	return ret;
}

ssize_t CSocket::Send(int fd, const void *buf, size_t n, int flags)
{
	int ret = send(fd, buf, n, flags);
	if(-1 == ret)
	{
		if(errno!=EAGAIN &&
			errno!=EWOULDBLOCK &&
			errno!=EINTR &&
			errno!=ECONNABORTED)
		{
			TRACE_ERR(LOG_ERRORS, errno, 100, "Send datas to socket %d happen error", fd);
			return ret;
		}
		return -2;
	}
	return ret;
}

ssize_t CSocket::Read(int fd, void *buf, size_t count)
{
	ssize_t ret = read(fd, buf, count);
	if(-1 == ret)
	{
		if(errno!=EAGAIN &&
			errno!=EWOULDBLOCK &&
			errno!=EISDIR)
		{
			TRACE_ERR(LOG_ERRORS, errno, 100, "Read datas to socket %d happen error", fd);
			return ret;
		}
		return -2;
	}
	return ret;
}

ssize_t CSocket::Write(int fd, const void *buf, size_t count)
{
	ssize_t ret = write(fd, buf, count);
	if(-1 == ret)
	{
		if(errno!=EAGAIN &&
			errno!=EWOULDBLOCK &&
			errno!=EISDIR)
		{
			TRACE_ERR(LOG_ERRORS, errno, 100, "Write datas to socket %d happen error", fd);
			return ret;
		}
		return -2;
	}
	return ret;
}
