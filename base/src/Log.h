#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#ifndef BASE_SRC_CLOG_H_
#define BASE_SRC_CLOG_H_

#include"datatype.h"
#include"MsgTime.h"

#include<string>
#include<deque>
#include<vector>
using namespace std;

enum LOG_TYPE
{
	LOG_SYSTEM=0,
   LOG_ERRORS=1,
   LOG_MESSAGES=2,
   LOG_DEBUGS=3
};

class CLog
{
public:
	CLog();
	virtual ~CLog();
public:
	void set_curpath(const char *path);
	void set_cycle(const size_t cycle);
   bool log_open(const char *logname, int level=0, int threadnum=1, size_t second=60);
   void log_quit(int level, int error, int len, const char *fmt, ...);
   void log_dump(int level, int len, const char *fmt, ...);
   void log_sys(int level, int len, const char *fmt, ...);
   void log_err(int level, int error, int len, const char *fmt, ...);
   void log_msg(int level, int len, const char *fmt, ...);
protected:
   inline void log_doit(int errflag, int error, int len, const char *fmt, va_list arg);
   inline void log_puts(const char *data, const size_t size);
   static void *log_write(void *valp);
   static void clean_proc(void *valp);
   inline bool should_newfile();
protected:
   sem_t m_ToWriteBuff;
   pthread_mutex_t m_mutex;
   size_t m_sleepTime;
   vector<pthread_t> m_vPthreadID;
   CMsgTime m_FileTime;
private:
   deque<byte> m_deque;	//保存日志信息的缓存
   int m_level;				//日志信息的级别
   int m_fputsType;			//日志打印的方式，0：直接输出到IO流，1：输出到日志文件
   FILE *m_pLogFile;		//日志文件指针
   string m_curpath;		//日志文件的保存的根目录
   size_t m_cycle;			//日志保存的周期（月为单位）
   string m_logPrefix;		//日志文件的名称前缀
   size_t m_logIndex;		//日志文件的后缀索引
};

extern CLog g_log;

#endif /*BASE_SRC_CLOG_H_*/
