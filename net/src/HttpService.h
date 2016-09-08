/*
 * HttpService.h
 *
 *  Created on: 2015年8月12日
 *      Author: dengxu
 */

#ifndef NET_SRC_HTTPSERVICE_H_
#define NET_SRC_HTTPSERVICE_H_

#include"AsyncSocket.h"
#include"../../base/src/EpollWatch.h"

class CHttpService:
		public CAsyncSocket,
		public CEpollWatch
{
public:
	CHttpService();
	virtual ~CHttpService();
};

#endif /* NET_SRC_HTTPSERVICE_H_ */
