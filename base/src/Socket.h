/*
 * Socket.h
 *
 *  Created on: 2015年8月10日
 *      Author: dengxu
 */

#ifndef BASE_SRC_SOCKET_H_
#define BASE_SRC_SOCKET_H_

#include<sys/socket.h>
#include<unistd.h>

enum PROTOCOL
{
	PROTO_TCP=0,
	PROTO_UDP,
	PROTO_HTTP
};

class CSocket
{
public:
	CSocket();
	virtual ~CSocket();
protected:
	int Socket(int domain, int type, int protocol);

	int Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);

	int Bind(int fd, const struct sockaddr * addrp, socklen_t len);

	int Listen(int fd, int n);

	int Accept(int fd, struct sockaddr *__restrict addrp, socklen_t *__restrict addr_len);

	int Connect(int fd, const struct sockaddr * addrp, socklen_t len);

	int Close(int fd);

	ssize_t Recvfrom(int fd, void *__restrict buf, size_t n, int flags,
			struct sockaddr *__restrict addrp, socklen_t *__restrict addr_len);

	ssize_t Recv(int fd, void *buf, size_t n, int flags);

	ssize_t Sendto(int fd, __const void *buf, size_t n, int flags, const struct sockaddr *addrp,
			       socklen_t addr_len);

	ssize_t Send(int fd, const void *buf, size_t n, int flags);

	ssize_t Read(int fd, void *buf, size_t count);

	ssize_t Write(int fd, const void *buf, size_t count);
};

#endif /* BASE_SRC_SOCKET_H_ */
