/*
 * CMsgTime.cpp
 *
 *  Created on: 2015年8月8日
 *      Author: dengxu
 */

#include "MsgTime.h"
#include <stdio.h>

CMsgTime::CMsgTime(time_t utc)
{
	gmtime_r(&utc, &m_time);
}

CMsgTime::CMsgTime(int year, int month, int day, int hour, int minute, int second)
{
	if(year > 1900)
	{
		m_time.tm_year = year-1900;
		m_time.tm_mon  = month-1;
		m_time.tm_mday = day;
		m_time.tm_hour = hour;
		m_time.tm_min  = minute;
		m_time.tm_sec  = second;
	}
}

CMsgTime::~CMsgTime()
{
	// TODO Auto-generated destructor stub
}

bool CMsgTime::operator == (const CMsgTime &other)
{
	time_t theTime = mktime(&m_time);
	time_t othTime = mktime((struct tm *)&(other.m_time));

	return theTime == othTime ? true:false;
}

bool CMsgTime::operator != (const CMsgTime &other)
{
	time_t theTime = mktime(&m_time);
	time_t othTime = mktime((struct tm *)&(other.m_time));

	return theTime != othTime ? true:false;
}

bool CMsgTime::operator >= (const CMsgTime &other)
{
	time_t theTime = mktime(&m_time);
	time_t othTime = mktime((struct tm *)&(other.m_time));

	return theTime >= othTime ? true:false;
}

bool CMsgTime::operator <= (const CMsgTime &other)
{
	time_t theTime = mktime(&m_time);
	time_t othTime = mktime((struct tm *)&(other.m_time));

	return theTime <= othTime ? true:false;
}

bool CMsgTime::operator >  (const CMsgTime &other)
{
	time_t theTime = mktime(&m_time);
	time_t othTime = mktime((struct tm *)&(other.m_time));

	return theTime > othTime ? true:false;
}

bool CMsgTime::operator <  (const CMsgTime &other)
{
	time_t theTime = mktime(&m_time);
	time_t othTime = mktime((struct tm *)&(other.m_time));

	return theTime < othTime ? true:false;
}

time_t CMsgTime::GetUtcTime()
{
	return  mktime(&m_time);
}

struct tm CMsgTime::GetTime() const
{
	return this->m_time;
}

void CMsgTime::SetCurrentTime()
{
	time_t now;
	time(&now);
	localtime_r(&now, &m_time);
}

string CMsgTime::ToString()
{
	string szResult(20, 0);
	snprintf(&szResult[0], 20,
			"%04u-%02u-%02u %02u:%02u:%02",
			m_time.tm_year+1900, m_time.tm_mon+1, m_time.tm_mday,
			m_time.tm_hour, m_time.tm_min, m_time.tm_sec
		);
	return szResult;
}

string CMsgTime::ToDayString()
{
	string szResult(12, 0);
	snprintf(&szResult[0], 11,
			"%04u-%02u-%02u",
			m_time.tm_year+1900, m_time.tm_mon+1, m_time.tm_mday
		);
	return szResult;
}
