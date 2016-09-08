/*
 * IOBufferPtr.h
 *
 *  Created on: 2015年8月12日
 *      Author: dengxu
 */

#ifndef BASE_SRC_IOBUFFERPTR_H_
#define BASE_SRC_IOBUFFERPTR_H_

#include"Mutex.h"
#include"datatype.h"

#include<string>
#include<deque>
using namespace std;

struct mmset
{
	int rd_pos;
	int wr_pos;
	deque<byte> base;
};

class CIOBufferPtr
{
public:
	CIOBufferPtr(size_t size=1024/*1MB的缓存*/);
	virtual ~CIOBufferPtr();
public:
	void Resize(int size);
	int GetSize();
	int GetLength();

	//read data
	int Read(int8 &value);
	int Read(int16 &value);
	int Read(int32 &value);
	int Read(int64 &value);
	int Read(char &value);
	int Read(string &value, int len);

	//write data
	int Write(const int8 value);
	int Write(const int16 value);
	int Write(const int32 value);
	int Write(const int64 value);
	int Write(const char value);
	int Write(const string value);

	void Clear();
private:
	struct mmset m_mmset;
	CMutex m_mutex;
};

#endif /* BASE_SRC_IOBUFFERPTR_H_ */
