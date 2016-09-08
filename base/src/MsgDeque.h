/*
 * Deque.h
 *
 *  Created on: 2015年8月9日
 *      Author: dengxu
 */

#ifndef BASE_SRC_MSGDEQUE_H_
#define BASE_SRC_MSGDEQUE_H_

#include"Mutex.h"
#include"Timeout.h"
#include<bits/local_lim.h>
#include<errno.h>
#include<semaphore.h>

#include <deque>
using namespace std;

extern int errno;

template<class V, class MUTEX=THREAD_MUTEX>
class CMsgDeque
{
public:
	CMsgDeque(const size_t maxSize=50000)
	{
		m_bAbort  = false;
		m_bsNotep = false;
		m_bsNotfl = false;
		ResetInit(maxSize);
	}
	virtual ~CMsgDeque()
	{
		AbortWati();
		SafeClose();
	}
public:
	size_t ResetInit(size_t maxSize)
	{
		maxSize = maxSize>SEM_VALUE_MAX ? SEM_VALUE_MAX-1:maxSize;
		if(this->m_bAbort)
		{
				this->m_bAbort = false;
				SafeClose();
		}
		int rete = sem_init(&m_sNotEmpty, 0, 0);
		int retf = sem_init(&m_sNotFull, 0, maxSize);
		return ((rete==retf) && rete==0) ?
				this->m_bsNotep=true,this->m_bsNotfl=true,
				m_maxSize=maxSize:0;
	}

	inline void SetMaxQueueSize(size_t maxSize)
	{
		maxSize = maxSize>SEM_VALUE_MAX ? SEM_VALUE_MAX-1:maxSize;
		m_maxSize = maxSize;
	}

	inline void SafeClose()
	{
		if(!this->m_bsNotep &&
				!this->m_bsNotfl)
		{
			sem_destroy(&m_sNotEmpty);
			sem_destroy(&m_sNotFull);
			m_bsNotep = false;
			m_bsNotfl = false;
		}
	}

	inline void AbortWati()
	{
		this->m_bAbort = false;
		sem_post(&this->m_sNotEmpty);
		sem_post(&this->m_sNotFull);
	}

	inline bool isFull(int offset=0)
	{
		return (m_deque.size()+offset) >= m_maxSize ? 1:0;
	}

	inline bool isEmpty()
	{
		return m_deque.empty();
	}

	inline bool isAbort()
	{
		return this->m_bAbort;
	}

	bool Front(V &data, u_long out=0)
	{
		if(this->WaitforNotEmpty(out))
		{
			AUTO_GUARD(g, MUTEX, this->m_mutex);
			if(m_deque.size()>0)
			{
				data = m_deque.front();
				return true;
			}
			return false;
		}
		if(this->m_bAbort)
		{
			return false;
		}
		return false;
	}

	bool PopFront(V &data, u_long out=0)
	{
		if(!this->m_bAbort)
		{
			if(this->WaitforNotEmpty(out))
			{
				AUTO_GUARD(g, MUTEX, this->m_mutex);
				if(m_deque.size()>0)
				{
					data = m_deque.front();
					m_deque.pop_front();
					sem_post(&this->m_sNotFull);
					return true;
				}
				return false;
			}
		}
		else
		{
			AUTO_GUARD(g, MUTEX, this->m_mutex);
			if(!this->isEmpty())
			{
				data = m_deque.front();
				m_deque.pop_front();
				return true;
			}
			return false;
		}
		return false;
	}

	bool PushFront(const V &data, u_long out=0)
	{
		if(!this->m_bAbort)
		{
			if(this->WaitforNotFull(out))
			{
				AUTO_GUARD(g, MUTEX, this->m_mutex);
				if(!this->isFull())
				{
					m_deque.push_front(data);
					sem_post(&this->m_sNotEmpty);
					return true;
				}
			}
		}
		return false;
	}

	bool Back(V &data, u_long out=0)
	{
		if(this->WaitforNotEmpty(out))
		{
			AUTO_GUARD(g, MUTEX, this->m_mutex);
			data = m_deque.back();
			return true;
		}
		if(this->m_bAbort)
		{
			return false;
		}
		return false;
	}

	bool PopBack(V &data, u_long out=0)
	{
		if(!this->m_bAbort)
		{
			if(this->WaitforNotEmpty(out))
			{
				AUTO_GUARD(g, MUTEX, this->m_mutex);
				data = m_deque.back();
				m_deque.pop_back();
				sem_post(&this->m_sNotFull);
				return true;
			}
		}
		else
		{
			AUTO_GUARD(g, MUTEX, this->m_mutex);
			if(!m_deque.empty())
			{
				data = m_deque.back();
				m_deque.pop_back();
				return true;
			}
			return false;
		}
		return false;
	}

	bool PushBack(const V &data)
	{
		if(!this->m_bAbort)
		{
			AUTO_GUARD(g, MUTEX, this->m_mutex);
			if(!this->isFull())
			{
				m_deque.push_back(data);
				sem_post(&this->m_sNotEmpty);
				return true;
			}
		}
		return false;
	}

	bool Insert(typename std::deque<V>::const_iterator where, V *first, V *last)
	{
		if(!this->m_bAbort)
		{
			AUTO_GUARD(g, MUTEX, this->m_mutex);
			int offset = last-first;
			if(offset<0) offset=-offset;
			if(!this->isFull(offset))
			{
				m_deque.insert(where, first, last);
				sem_post(&this->m_sNotEmpty);
				return true;
			}
		}
		return false;
	}

	inline size_t Size()
	{
		return m_deque.size();
	}

	void Clear()
	{
		AbortWati();
		SafeClose();
		m_deque.clear();
	}

protected:
	bool WaitforNotEmpty(u_long out = 0)
	{
		CTimeout cnt;
		cnt.Init(out);
		while(!this->m_bAbort && this->m_bsNotep)
		{
			int ret = 0;
			u_long w = cnt.timeleft();
			switch(out)
			{
			case -1:
				ret = sem_wait(&this->m_sNotEmpty);
				break;
			default:
				struct timespec tim;
				cnt.timeofNowTime(tim, w);
				ret = sem_timedwait(&this->m_sNotEmpty, &tim);
				break;
			}
			if(0 == ret && !this->m_bAbort)
			{
				return true;
			}
			else if(EINTR == errno)
			{
				continue;
			}
			else{
				return false;
			}
		}
		return false;
	}

	bool WaitforNotFull(u_long out = 0)
	{
		CTimeout cnt;
		cnt.Init(out);
		while(!this->m_bAbort && this->m_bsNotfl)
		{
			int ret = 0;
			u_long w = cnt.timeleft();
			switch(out)
			{
			case -1:
				ret = sem_wait(&this->m_sNotEmpty);
				break;
			default:
				struct timespec tim;
				cnt.timeofNowTime(tim, w);
				ret = sem_timedwait(&this->m_sNotEmpty, &tim);
				break;
			}
			if(0 == ret && !this->m_bAbort)
			{
				return true;
			}
			else if(EINTR == errno)
			{
				continue;
			}
			else{
				return false;
			}
		}
		return false;
	}

private:
	bool m_bAbort;
	MUTEX m_mutex;
	deque<V>  m_deque;
	sem_t m_sNotEmpty;
	sem_t m_sNotFull;
	bool  m_bsNotep;
	bool  m_bsNotfl;
	size_t m_maxSize;
};

#endif /* BASE_SRC_MSGDEQUE_H_ */
