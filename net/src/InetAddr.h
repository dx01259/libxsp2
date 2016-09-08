/*
 * InetAddr.h
 *
 *  Created on: 2015年8月12日
 *      Author: dengxu
 */

#ifndef NET_INETADDR_H_
#define NET_INETADDR_H_

#include"../../base/src/datatype.h"
#include<netinet/in.h>

#include<string>
using namespace std;


class CInetAddr
{
public:
	CInetAddr(u_short usPort, const char *addr);
	CInetAddr(const sockaddr_in addr);
	virtual ~CInetAddr();
public:
	string GetIp() const;
	u_short GetPort() const;
	void SetSockType(const int socktype);
	int  GetSockType() const;
	void GetInetAddr(sockaddr_in &addr);
private:
	sockaddr_in m_addr;
	int m_socktype;
};

#endif /* NET_INETADDR_H_ */
