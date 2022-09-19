/*客户端头文件和函数声明*/

#ifndef CLIENT_H

#define CLIENT_H

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
#include "tools.h"

#define pf printf//方便调试，减少代码量

extern int sockfd;//引用全局变量

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


/*客户端函数声明*/

//菜单
void menu(void);
//上传
void upload(void);
//下载
void download(void);
//显示服务器目录和文件
void s_list(void);
//显示客户端目录和文件
void c_list(void);
//退出程序
void quit(void);

#endif