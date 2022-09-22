/*服务器头文件和函数声明*/

#ifndef SERVER_H

#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include<setjmp.h>
#include<signal.h>
#include "tools.h"

#define pf printf//方便调试,减少代码量

/*不要在头文件中声明全局变量,不然当多个文件引入同一头文件时就会有变量重复定义错误*/
extern char cmd[];//引用全局变量
extern jmp_buf jmpbuf;//跳转的位置

typedef struct LS
{
	char mode[15];	// 文件的模式
	int dir_num;	// 是否目录或目录中包含目录的数量
	char user[20];	// 文件的用户名
	char group[20]; // 文件的组名
	long size;		// 文件的字节数
	char time[30];	// 文件的最后修改时间
	int st_mode;	// 文件类型和权限
	char name[20];	// 文件名
} LS;

/*服务器函数声明*/
// SIGNO信号的执行函数
void sig_quit();
// 执行退出线程
void *callback(void *arg);
// 开始运行线程
void *start_run(void *arg);
// 客户端上传
void c_up(int *clifd);
// 客户端下载
void c_down(int *clifd);
// 返回文件列表
void c_list(int *clifd);

#endif