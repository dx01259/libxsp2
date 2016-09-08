/*
 * MsgTime.h
 *
 *  Created on: 2015年8月8日
 *      Author: dengxu
 */

#ifndef BASE_SRC_MSGTIME_H_
#define BASE_SRC_MSGTIME_H_

#include<sys/time.h>
#include<limits.h>

#include<string>
using namespace std;

class CMsgTime
{
public:
	CMsgTime(time_t utc=0);
	CMsgTime(int year, int month, int day, int hour, int minute, int second);
	CMsgTime(const CMsgTime &other)
	{
		this->operator=(other);
	}
	CMsgTime &operator=(const CMsgTime &other)
	{
		if(&other != this)
		{
			this->m_time = other.m_time;
		}
		return *this;
	}
	virtual ~CMsgTime();
public:
	bool operator == (const CMsgTime &other);
	bool operator != (const CMsgTime &other);
	bool operator >= (const CMsgTime &other);
	bool operator <= (const CMsgTime &other);
	bool operator >  (const CMsgTime &other);
	bool operator <  (const CMsgTime &other);
public:
	time_t GetUtcTime();
	struct tm GetTime() const;
	void SetCurrentTime();
	string ToString();
	string ToDayString();
private:
	struct tm m_time;
};

#endif /* BASE_SRC_MSGTIME_H_ */
