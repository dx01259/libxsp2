/*
 * LocalIpList.h
 *
 *  Created on: 2015年8月12日
 *      Author: dengxu
 */

#ifndef NET_LOCALIPLIST_H_
#define NET_LOCALIPLIST_H_

#include<string>
#include<vector>
using namespace std;

class CLocalIpList
{
public:
	CLocalIpList();
	virtual ~CLocalIpList();
public:
	int GetLocalIpList();
	int GetLocalIpList(vector<string> &ips);
	bool islocalIp(const char *ip);
private:
	vector<string> m_iplist;
};

#endif /* NET_LOCALIPLIST_H_ */
