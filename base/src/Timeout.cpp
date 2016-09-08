/*
 * CTimeout.cpp
 *
 *  Created on: 2015年8月9日
 *      Author: dengxu
 */

#include "Timeout.h"
#include <limits.h>
#include <time.h>

CTimeout::CTimeout(const u_long to)
{
	this->m_timeout = to;
	Reset();
}

CTimeout::~CTimeout()
{
	// TODO Auto-generated destructor stub
}

void CTimeout::Init(u_long out)
{
	this->m_timeout = out;
	Reset();
}

void CTimeout::Reset()
{
	m_startTime = timeGetTime();
}

u_long CTimeout::Start()
{
	return (m_started=true, m_startTime=timeGetTime());
}

void CTimeout::SetTimeout(u_long out)
{
	m_timeout = out;
}

bool CTimeout::IsTimeout() const
{
	return (elapse() > m_timeout);
}

u_long CTimeout::elapse() const
{
	u_long end = timeGetTime();
	return m_started ? ((m_startTime<=end)?(end-m_startTime):(INT_MAX-end)):0;
}

u_long CTimeout::timeleft()
{
	return (m_timeout>elapse() ? m_timeout-elapse():0);
}

u_long CTimeout::timeGetTime() const
{
	u_long uptime=0;
	struct timespec spec;
	if(clock_gettime(CLOCK_MONOTONIC, &spec)==0)
	{
		uptime = spec.tv_sec*1000+spec.tv_nsec/1000000;
	}
	return uptime;
}

bool CTimeout::timeofNowTime(struct timespec &spec, const u_long minisec)
{
	struct timeval val;
	int ret = gettimeofday(&val, NULL);
	spec.tv_sec = val.tv_sec+minisec/1000;
	u_long minitmp = minisec % 1000;
	u_long usec = val.tv_usec + minitmp*1000;
	spec.tv_sec += usec/(1000000);
	usec %= 1000000;
	spec.tv_nsec = usec*1000;
	spec.tv_sec += (spec.tv_nsec/(1000000000));
	spec.tv_nsec %=1000000000;
	return 0==ret ? true:false;
}
