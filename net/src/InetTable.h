/*
 * InetTable.h
 *
 *  Created on: 2015年8月12日
 *      Author: dengxu
 */

#ifndef NET_INETTABLE_H_
#define NET_INETTABLE_H_

#include"InetAddr.h"
#include"../../base/src/TMap.h"

class CInetTable
{
public:
	CInetTable();
	virtual ~CInetTable();
public:
	int GetIpAddr(const string &szID, CInetAddr *addrp);
	void AddIpAddr(const string &szID,  const CInetAddr *addrp);
	bool RemoveIpAddr(const string &szID);
	void ClearIpAddr();
private:
	CTMap<string, CInetAddr *> m_iptables;
};

#endif /* NET_INETTABLE_H_ */
