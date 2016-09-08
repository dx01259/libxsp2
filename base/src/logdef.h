/*
 * logdef.h
 *
 *  Created on: 2015年8月7日
 *      Author: dengxu
 */

#ifndef BASE_SRC_LOGDEF_H_
#define BASE_SRC_LOGDEF_H_

#include"Log.h"
#include<errno.h>

extern int errno;

extern CLog g_log;

/*************************************************************************************************************
 * function: SET_CURPATH(const char *path)
 * describe: 设置日期文件的创建时所在的根目录
 * input:		path	:	日志文件的根路径
 * 	output:	none
 * 	result：	none
 *************************************************************************************************************/
#define SET_CURPATH		g_log.set_curpath

/*************************************************************************************************************
 * function: SET_CYCLE(const size_t cycle)
 * describe: 设置日志文件保存的时间，以月为单位
 * input:		cycle	:	保存日志文件的周期（月为单位）
 * 	output:	none
 * 	result：	none
 *************************************************************************************************************/
#define SET_CYCLE 		g_log.set_cycle

/*************************************************************************************************************
 * function: TRACE_OPEN(const char *logname, int level, int threadnum, size_t second)
 * describe: 打开日志文件，创建处理日志信息的的线程数
 * input:		logname	:	日志文件的名称，不可以指定路径
 * 				level		:	打印日志的级别
 * 				threadnum	:	处理日志的线程数
 * 				second		:	线程等待的秒数
 * 	output:	none
 * 	result：	none
 *************************************************************************************************************/
#define TRACE_OPEN(logname, level, threadnum, second)\
	g_log.log_open(logname, level, threadnum, second);

/*************************************************************************************************************
 * function: TRACE_QUIT(int level, int error, int len, const char *fmt, ...)
 * describe: 以退出程序的方式，打印日志信息，可以指定系统错误码，获取更加详细的错误信息
 * input:		level	:	打印日志的级别
 * 				error	:  指定返回系统错误的ID
 * 				len 	:	指定打印数据的长度
 * 				fmt		:	数据编码的格式
 * 	output:	none
 * 	result：	none
 *************************************************************************************************************/
#define TRACE_QUIT(level, error, len, fmt, ...) \
		g_log.log_quit(level, error, len+100, "FILE: "__FILE__", LINE: %d: "fmt "\n", __LINE__, ##__VA_ARGS__);

/*************************************************************************************************************
 * function: TRACE_DUMP(int level, int len, const char *fmt, ...)
 * describe: 以退出的方式打印日志信息，不携带系统错误提示
 * input:		level	:	打印日志的级别
 * 				len 	:	指定打印数据的长度
 * 				fmt		:	数据编码的格式
 * 	output:	none
 * 	result：	none
 *************************************************************************************************************/
#define TRACE_DUMP(level, len, fmt, ...) \
		g_log.log_dump(level, len+100, "FILE: "__FILE__", LINE: %d: "fmt "\n", __LINE__, ##__VA_ARGS__);

/*************************************************************************************************************
 * function: TRACE_SYS(int level, int len, const char *fmt, ...)
 * describe: 打印系统日志信息，无错误信息
 * input:		level	:	打印日志的级别
 * 				len 	:	指定打印数据的长度
 * 				fmt		:	数据编码的格式
 * 	output:	none
 * 	result：	none
 *************************************************************************************************************/
#define TRACE_SYS(level, len, fmt, ...) \
		g_log.log_sys(level, len+100, "FILE: "__FILE__", LINE: %d: "fmt "\n", __LINE__, ##__VA_ARGS__);

/*************************************************************************************************************
 * function: TRACE_ERR(int level, int error, int len, const char *fmt, ...)
 * describe: 打印错误信息，不退出程序，且可以获取对应的系统错误信息
 * input:		level	:	打印日志的级别
 * 				error	:  指定返回系统错误的ID
 * 				len 	:	指定打印数据的长度
 * 				fmt		: 	数据编码的格式
 * 	output:	none
 * 	result：	none
 *************************************************************************************************************/
#define TRACE_ERR(level, error, len, fmt, ...) \
		g_log.log_err(level, error, len+100, "FILE: "__FILE__", LINE: %d: "fmt "\n", __LINE__, ##__VA_ARGS__);

/*************************************************************************************************************
 * function: TRACE_MSG(int level, int len, const char *fmt, ...)
 * describe: 打印消息信息，无错误信息
 * input:		level	:	打印日志的级别
 * 				len 	:	指定打印数据的长度
 * 				fmt		:	数据编码的格式
 * 	output:	none
 * 	result：	none
 *************************************************************************************************************/
#define TRACE_MSG(level, len, fmt, ...) \
		g_log.log_msg(level, len+100, "FILE: "__FILE__", LINE: %d: "fmt "\n", __LINE__, ##__VA_ARGS__);


#endif /* BASE_SRC_LOGDEF_H_ */
