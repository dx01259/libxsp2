/*
 * CMutex.cpp
 *
 *  Created on: 2015年8月7日
 *      Author: dengxu
 */

#include"Mutex.h"
#include"commfunc.h"

CMutex::CMutex()
{
	m_blocked = false;
	check_zero(pthread_mutex_init(&m_mutex, NULL));
}

CMutex::~CMutex()
{
	m_blocked = false;
	pthread_mutex_destroy(&m_mutex);
}

int CMutex::lock()
{
	m_blocked = true;
	int ret = pthread_mutex_lock(&m_mutex);
	if(0 != ret)
		m_blocked = false;
	return ret;
}

int CMutex::unlock()
{
	int ret = 0;
	ret = pthread_mutex_unlock(&m_mutex);
	if(ret == 0)
		m_blocked = false;
	return ret;
}

int CMutex::trylock()
{
	int ret = 0;
	ret = pthread_mutex_trylock(&m_mutex);
	if(ret == 0)
		m_blocked = true;
	return ret;
}

bool CMutex::islocked()
{
	return m_blocked;
}
