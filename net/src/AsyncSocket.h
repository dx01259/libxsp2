/*
 * AsyncSocket.h
 *
 *  Created on: 2015年8月10日
 *      Author: dengxu
 */

#ifndef NET_ASYNCSOCKET_H_
#define NET_ASYNCSOCKET_H_

#include"../../base/src/Socket.h"


class CAsyncSocket:
		public CSocket
{
public:
	CAsyncSocket();
	virtual ~CAsyncSocket();
public:
	inline int GetSocket()
	{
		return m_socket;
	}
public:
	int CreateSocket(const char *ip="127.0.0.1", const short port=10000, int protocol=PROTO_TCP);
	int AsyncAccept(struct sockaddr *__restrict addrp, socklen_t *__restrict addr_len);
	void CloseSocket();
	virtual ssize_t Recv(void *buf, size_t len);
	virtual ssize_t Send(const char *buf, size_t len);
protected:
	static int SetAsyncSocket(int sockfd);
private:
	int m_socket;
};

#endif /* NET_ASYNCSOCKET_H_ */
