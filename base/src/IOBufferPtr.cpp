/*
 * IOBufferPtr.cpp
 *
 *  Created on: 2015年8月12日
 *      Author: dengxu
 */

#include"IOBufferPtr.h"
#include"logdef.h"
#include<string.h>
#include<arpa/inet.h>

template<class V>
static int read_mmset(struct mmset *mset, V &value)
{
	if(mset)
	{
		uint32 len = mset->wr_pos - mset->rd_pos;
		if(len >= sizeof(value))
		{
			memcpy(&value, &mset->base.at(mset->rd_pos), sizeof(value));
			value = htonl(value);
			mset->rd_pos += sizeof(value);
			return sizeof(value);
		}
	}
	return -1;
}

template<class V>
static int write_mmset(struct mmset *mset, const V value)
{
	if(mset)
	{
		if(mset->base.size()-mset->wr_pos>=sizeof(value))
		{
			V hval = htonl(value);
			memcpy(&mset->base[mset->wr_pos], (char *)&hval, sizeof(hval));
			mset->wr_pos += sizeof(hval);
			return sizeof(value);
		}
	}
	return -1;
}

CIOBufferPtr::CIOBufferPtr(size_t size)
{
#if 0
	这要做会导致段错误
	//memset(&m_mmset, 0, sizeof(struct mmset));
#endif
	m_mmset.rd_pos = 0;
	m_mmset.wr_pos = 0;
	m_mmset.base.resize(size);
}

CIOBufferPtr::~CIOBufferPtr()
{
	Clear();
}

void CIOBufferPtr::Resize(int size)
{
	AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);

	m_mmset.base.resize(size);
}

int CIOBufferPtr::GetSize()
{
	AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);

	return m_mmset.base.size();
}

int CIOBufferPtr::GetLength()
{
	AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);

	return (m_mmset.wr_pos - m_mmset.rd_pos);
}

//read data ............
int CIOBufferPtr::Read(int8 &value)
{
	AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);

	int ret = read_mmset(&m_mmset, value);
	if(-1 == ret)
	{
		TRACE_MSG(LOG_ERRORS, 100, "read_mmset is error");
		return -1;
	}
	return ret;
}

int CIOBufferPtr::Read(int16 &value)
{
	AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);

	int ret = read_mmset(&m_mmset, value);
	if(-1 == ret)
	{
		TRACE_MSG(LOG_ERRORS, 100, "read_mmset is error");
		return -1;
	}
	return ret;
}

int CIOBufferPtr::Read(int32 &value)
{
	AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);

	int ret = read_mmset(&m_mmset, value);
	if(-1 == ret)
	{
		TRACE_MSG(LOG_ERRORS, 100, "read_mmset is error");
		return -1;
	}
	return ret;
}

int CIOBufferPtr::Read(int64 &value)
{
	AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);

	int ret = read_mmset(&m_mmset, value);
	if(-1 == ret)
	{
		TRACE_MSG(LOG_ERRORS, 100, "read_mmset is error");
		return -1;
	}
	return ret;
}

int CIOBufferPtr::Read(char &value)
{
	AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);

	int ret = read_mmset(&m_mmset, value);
	if(-1 == ret)
	{
		TRACE_MSG(LOG_ERRORS, 100, "read_mmset is error");
		return -1;
	}
	return ret;
}

int CIOBufferPtr::Read(string &value, int len)
{
	AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);

	if(len <= m_mmset.wr_pos-m_mmset.rd_pos)
	{
		value.resize(len);
		memcpy(&value[0], &m_mmset.base.at(m_mmset.rd_pos), len);
		m_mmset.rd_pos += len;
		return len;
	}
	return -1;
}


//write data ..
int CIOBufferPtr::Write(const int8 value)
{
	AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);

	int ret = write_mmset(&m_mmset, value);
	if(-1 == ret)
	{
		TRACE_MSG(LOG_ERRORS, 100, "write_mmset is error");
		return -1;
	}
	return ret;
}

int CIOBufferPtr::Write(const int16 value)
{
	AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);

	int ret = write_mmset(&m_mmset, value);
	if(-1 == ret)
	{
		TRACE_MSG(LOG_ERRORS, 100, "write_mmset is error");
		return -1;
	}
	return ret;
}

int CIOBufferPtr::Write(const int32 value)
{
	AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);

	int ret = write_mmset(&m_mmset, value);
	if(-1 == ret)
	{
		TRACE_MSG(LOG_ERRORS, 100, "write_mmset is error");
		return -1;
	}
	return ret;
}

int CIOBufferPtr::Write(const int64 value)
{
	AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);

	int ret = write_mmset(&m_mmset, value);
	if(-1 == ret)
	{
		TRACE_MSG(LOG_ERRORS, 100, "write_mmset is error");
		return -1;
	}
	return ret;
}

int CIOBufferPtr::Write(const char value)
{
	AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);

	int ret = write_mmset(&m_mmset, value);
	if(-1 == ret)
	{
		TRACE_MSG(LOG_ERRORS, 100, "write_mmset is error");
		return -1;
	}
	return ret;
}

int CIOBufferPtr::Write(const string value)
{
	AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);

	if(m_mmset.base.size()-m_mmset.wr_pos>=value.size())
	{
		memcpy(&m_mmset.base[m_mmset.wr_pos], (char *)&value[0], value.size());
		m_mmset.wr_pos += value.size();
		return value.size();
	}
	return 0;
}

void CIOBufferPtr::Clear()
{
	AUTO_GUARD(g, THREAD_MUTEX, this->m_mutex);

	m_mmset.base.clear();
	m_mmset.rd_pos = 0;
	m_mmset.wr_pos = 0;
}
