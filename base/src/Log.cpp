#include "Log.h"

#include<stdarg.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<linux/limits.h>
#include<errno.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<dirent.h>
#include"commfunc.h"
#include"ThreadDefine.h"
#include"Timeout.h"

extern int errno;
CLog g_log;

static bool get_curChiledPath(const char *cur, char *child, size_t len);

CLog::CLog()
{
	this->m_level = 0;
	m_fputsType   = 0;
	m_sleepTime   = 30;
	m_pLogFile    = NULL;
	m_cycle       = 1;
	m_curpath		 = "";
	m_logIndex 	 = 0;
	check_zero(sem_init(&m_ToWriteBuff, 0, 0));
	check_zero(pthread_mutex_init(&m_mutex, NULL));
}

CLog::~CLog()
{
	for(int i=0; i<m_vPthreadID.size(); ++i)
	{
		pthread_t &pid = m_vPthreadID[i];
		pthread_cancel(pid);
	}
	m_vPthreadID.clear();
	this->m_deque.clear();
	sem_destroy(&this->m_ToWriteBuff);
	pthread_mutex_destroy(&m_mutex);
	if(m_pLogFile)
	{
		fclose(m_pLogFile);
		m_pLogFile = NULL;
	}
}

void CLog::set_curpath(const char *path)
{
	if(path)
	{
		size_t len = strlen(path);
		m_curpath.resize(len);
		bzero(&m_curpath[0], len);
		memcpy(&m_curpath[0], path, len);
		if(m_curpath[len-1] == '/' ||
				m_curpath[len-1] == '\\')
		{
			m_curpath.resize(len-1);
		}
	}
}

void CLog::set_cycle(const size_t cycle)
{
	cycle>1 ? m_cycle = cycle:m_cycle=1;
}

bool CLog::log_open(const char *logname, int level, int threadnum, size_t second)
{
	if(logname)
	{
		size_t len;
		char curpath[PATH_MAX]={0};
		this->m_sleepTime = second;
		if(m_curpath.size() <= 0)
		{
			getcwd(curpath, PATH_MAX);
			set_curpath(curpath);
			len = strlen(curpath);
			m_curpath.resize(len);
			memcpy(&m_curpath[0], curpath, len);
		}
		check_nozero(get_curChiledPath(m_curpath.c_str(), curpath, PATH_MAX));
		len = strlen(curpath);
		CMsgTime now;
		now.SetCurrentTime();
		snprintf(&curpath[len], PATH_MAX-len, "/%s_%s_%d", logname, now.ToDayString().c_str(), m_logIndex);
		printf("fopen curpath:%s", curpath);
		this->m_pLogFile = fopen(curpath, "a+");
		if(NULL == this->m_pLogFile)
		{
			printf_f("fopen the file:%s is error", curpath);
			return false;
		}
		m_FileTime.SetCurrentTime();
		len = strlen(logname);
		m_logPrefix.resize(len);
		memcpy(&m_logPrefix[0], logname, len);
		m_logIndex += 1;
		for(int i=0; i<threadnum; ++i)
		{
			pthread_t thread;
			int ret = pthread_create(&thread, NULL, log_write, this);
			if(ret != 0) return false;
			this->m_vPthreadID.push_back(thread);
		}
		this->m_fputsType = 1;
	}
	return false;
}

inline bool CLog::should_newfile()
{
	CMsgTime now;
	now.SetCurrentTime();
	struct tm nowtm = now.GetTime();
	struct tm logtm = m_FileTime.GetTime();
	if(logtm.tm_year>=nowtm.tm_year &&
			logtm.tm_mon>=nowtm.tm_mon &&
			logtm.tm_mday>=nowtm.tm_mday)
	{
		return false;
	}

	return true;
}

void CLog::log_doit(int errflag, int error, int len, const char *fmt, va_list arg)
{
	try
	{
		if(len < 2048)
		{
			char strTemp[PATH_MAX];
			bzero(strTemp, sizeof(strTemp));
			vsnprintf(strTemp, sizeof(strTemp), fmt, arg);
			if(errflag)
			{
				int curpos = strlen(strTemp);
				snprintf(strTemp+curpos, PATH_MAX-curpos,":%s",strerror(error));
			}
			log_puts(&strTemp[0], strlen(strTemp)+1);
		}
		else
		{
			vector<char> strTemp(len+PATH_MAX);
			bzero(&strTemp[0], len);
			vsnprintf(&strTemp[0], len, fmt, arg);
			if(errflag)
			{
				int curpos = strlen(&strTemp[0]);
				snprintf(&strTemp[0]+curpos, len+PATH_MAX-curpos,":%s",strerror(error));
			}
			log_puts(&strTemp[0], strlen(&strTemp[0])+1);
		}
	}catch(...)
	{
		printf_f("Happen exceptions when call log_doit function");
	}
}

void CLog::log_puts(const char *data, const size_t size)
{
	int type = this->m_fputsType;
	switch(type)
	{
	case 0://将日志内容直接输出到IO流
		{
			fflush(stdout);
			fputs(data, stdout);
			fflush(stdout);
		}
		break;
	case 1://将日志内容输出到日志文件
		{
			pthread_mutex_lock(&m_mutex);
			deque<byte>::iterator iterbegin = m_deque.begin();
			m_deque.insert(iterbegin, data, size+data);
			if(1024*1024 < m_deque.size()*sizeof(byte))
			{
				//当数据满1MB时，将数据写到日志文件里面
				int ret = sem_post(&this->m_ToWriteBuff);
				if(0 != ret)
				{
					pthread_mutex_unlock(&m_mutex);
					printf_f("sem_post is error:%s", strerror(errno));
					return;
				}
			}
			pthread_mutex_unlock(&m_mutex);
		}
		break;
	}
}

void CLog::log_quit(int level, int error, int len, const char *fmt, ...)
{
	if(level > this->m_level)
	{
		va_list arg;
		va_start(arg, fmt);
		log_doit(1, error, len, fmt, arg);
		va_end(arg);
		exit(1);
	}
}

void CLog::log_dump(int level, int len, const char *fmt, ...)
{
	if(level > this->m_level)
	{
		va_list arg;
		va_start(arg, fmt);
		log_doit(0, 0, len, fmt, arg);
		va_end(arg);
		exit(1);
	}
}

void CLog::log_sys(int level, int len, const char *fmt, ...)
{
	if(level > this->m_level)
	{
		va_list arg;
		va_start(arg, fmt);
		log_doit(0, 0, len, fmt, arg);
		va_end(arg);
	}
}

void CLog::log_err(int level, int error, int len, const char *fmt, ...)
{
	if(level > this->m_level)
	{
		va_list arg;
		va_start(arg, fmt);
		log_doit(1, error, len, fmt, arg);
		va_end(arg);
	}
}

void CLog::log_msg(int level, int len, const char *fmt, ...)
{
	if(level > this->m_level)
	{
		va_list arg;
		va_start(arg, fmt);
		log_doit(0, 0, len, fmt, arg);
		va_end(arg);
	}
}

void *CLog::log_write(void *valp)
{
	CLog *plog = (CLog *)valp;
	cleanup_push_thread(clean_proc, plog);
	while(plog)
	{
		CTimeout out;
		struct timespec spec;
		out.timeofNowTime(spec, plog->m_sleepTime*1000/*毫秒*/);
		printf("sleepTime:%d\n", plog->m_sleepTime);
		printf("spec:%d, %d\n", spec.tv_sec, spec.tv_nsec);
		printf("hello\n");
		int ret = sem_timedwait(&plog->m_ToWriteBuff, &spec);
		if(0 == ret || errno==ETIMEDOUT)
		{
			printf("hell02\n");
			if(plog->should_newfile())
			{
				printf("hell03\n");
				fclose(plog->m_pLogFile);
				int ret = false;
				ret = plog->log_open(plog->m_logPrefix.c_str(),
						plog->m_level, 0, plog->m_cycle);
				if(!ret) continue;
			}
			printf("hell04\n");
			//收到信号，表示需要将缓存里面的数据写到文件里面去
			pthread_mutex_lock(&plog->m_mutex);
			size_t writesize = 0;
			do
			{
				writesize = plog->m_deque.size();
				if(writesize > 0)
				{
					size_t wsize = writesize > 1024 ? 1024:writesize;
					writesize = fwrite(&plog->m_deque[0], wsize, 1, plog->m_pLogFile)*wsize;
					if(writesize > 0)
					{
						deque<byte>::iterator iterbegin = plog->m_deque.begin();
						plog->m_deque.erase(iterbegin, iterbegin+writesize);
					}
				}
			}while(writesize>0);
			pthread_mutex_unlock(&plog->m_mutex);
		}
	}
	cleanup_pop_thread(1);
	return NULL;
}

void CLog::clean_proc(void *valp)
{
	CLog *pThis = (CLog *)valp;
	if(pThis)
	{
		pthread_mutex_lock(&pThis->m_mutex);
		size_t writesize = 0;
		do
		{
			writesize = pThis->m_deque.size();
			if(writesize > 0)
			{
				size_t wsize = writesize > 1024 ? 1024:writesize;
				writesize = fwrite(&pThis->m_deque[0], wsize, 1, pThis->m_pLogFile)*wsize;
				if(writesize > 0)
				{
					deque<byte>::iterator iterbegin = pThis->m_deque.begin();
					pThis->m_deque.erase(iterbegin, iterbegin+writesize);
				}
			}
		}while(writesize>0);
		pthread_mutex_unlock(&pThis->m_mutex);
	}
}

bool get_curChiledPath(const char *cur, char *child, size_t len)
{
	CMsgTime date;
	date.SetCurrentTime();
	string dirDate = cur;
	dirDate += "/"+date.ToDayString();
	if(NULL == opendir(dirDate.c_str()))
	{
		if(0 != mkdir_p(dirDate.c_str(), S_IRWXU))
		{
			printf_f("创建目录: %s失败, %s", dirDate.c_str(), strerror(errno));
			return false;
		}
	}
	bzero(child, len);
	size_t size = min(len-1, dirDate.size());
	memcpy(child, dirDate.c_str(), size);
	return true;
}
