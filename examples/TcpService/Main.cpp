/*
 * Main.cpp
 *
 *  Created on: 2015年8月13日
 *      Author: dengxu
 */

#include"../../net/src/TcpService.h"
#include"../../base/src/logdef.h"
#include"../../base/src/commfunc.h"
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>

static void sig_exit(int signo);
CTcpService g_service;

int main(int argc, char *argv[])
{
	//SET_CYCLE(1);
	//TRACE_OPEN("log", 5, 1, 60);
	signal(SIGINT, sig_exit);
	printf("This is main Start\n");
	if(argc < 3)
	{
		printf("Parameter error\n");
		printf("sample:\n");
		printf("\t./TcpService 8000 10\n");
		return -1;
	}
	else {
		short 	port = atoi(argv[1]);
		int 	threadNum = atoi(argv[2]);
		printf("port:%d, threadNum:%d\n", port, threadNum);
		g_service.Start(port, threadNum);
	}
	while(1){
		printf("程序后续循环\n");
		sleep(60);
	}
	printf("This is main end\n");
	return 0;
}

void sig_exit(int signo)
{
	g_service.Stop();
	exit(0);
}
