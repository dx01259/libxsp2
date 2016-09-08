/*
 * commfunc.h
 *
 *  Created on: 2015年8月6日
 *      Author: dengxu
 */

#ifndef BASE_SRC_COMMFUNC_H_
#define BASE_SRC_COMMFUNC_H_

#include"MsgTime.h"
#include<stdio.h>
#include<sys/types.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>

extern int errno;

/*************************************************************************************************************
 * function: printf_f(format,...)
 * describe: 可以答应文件名和所在行信息的printf函数
 * input:		format	:	格式控制字符串
 * 				...		:	不定参数
 * 	output:	none
 * 	result：	none
 *************************************************************************************************************/
#define printf_f(format,...) printf("FILE: "__FILE__", LINE: %d: "format "\n", __LINE__, ##__VA_ARGS__)

/*************************************************************************************************************
 * function: InvalidTime(const CMsgTime &time)
 * describe: 用来判断CMsgTime对象设置的时间是否有效
 * input:		time	:	时间变量，用来判断是否是有效的时间
 * 	output:	none
 * 	result：	none
 *************************************************************************************************************/
bool InvalidTime(const CMsgTime &time);

#define min(a, b) (a)>(b) ? (b):(a)
#define max(a, b) (a)>(b) ? (a):(b)

/*************************************************************************************************************
 * function: mkdir_p(const char *pathname, mode_t mode)
 * describe: 用来递归创建目录，如果目录已经存在，则不会再次创建
 * input:		pathname	:	路径字符串，可以是绝对路径也可以是相对路径，推荐使用相对路径
 * 				mode		:	创建路径指定的权限,可以查考open函数的mode参数说明
 * 	output:	none
 * 	result：	none
 *************************************************************************************************************/
int mkdir_p(const char *pathname, mode_t mode);

#define check_zero(retval) \
do{\
	if(0 != retval)\
	{\
		printf_f("参数返回的值为:%d,系统错误信息:%s", retval, strerror(errno));\
		exit(0);\
	}\
}while(0)

#define check_nozero(retval) \
do{\
	if(0 == retval)\
	{\
		printf_f("参数返回的值为:%d,系统错误信息:%s", retval, strerror(errno));\
		exit(0);\
	}\
}while(0)

#define delete_memory(ptr)\
do{\
	if(ptr)\
	{\
		delete ptr;\
		ptr = NULL;\
	}\
}while(0)

#define delete_arrmemory(ptr)\
do{\
	if(ptr)\
	{\
		delete[] ptr;\
		ptr = NULL;\
	}\
}while(0)

#define free_memory(ptr)\
do{\
	if(ptr)\
	{\
		free(ptr);\
		ptr = NULL;\
	}\
}while(0)


#endif /* BASE_SRC_COMMFUNC_H_ */
