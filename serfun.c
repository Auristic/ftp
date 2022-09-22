#include "server.h"

//声明全局变量
char cmd[20] = {};
jmp_buf jmpbuf;

//用来执行退出线程，输入q退出程序
void *callback(void *arg)
{
	pid_t* pid = (pid_t *)arg;
	char q = '\0';
    // pf("子线程:%ld\n",pthread_self());
	do{
		q = getch();
	}while(q != 'q');
	kill(*pid, SIGIO);//给主进程发送退出信号
	// pf("退出子线程\n");
	// pthread_exit(NULL);
    return NULL;
}

//接收SIGNO信号退出程序
void sig_quit()
{
    longjmp(jmpbuf, 1);//就是加强版goto,可以再不同函数间跳转
}

//开始运行
void *start_run(void *arg)
{
	int *clifd = (int *)arg;
	char up[20] = "我想上你";
	char down[20] = "我想下你";
	char see[20] = "我想看你";
	char quit[20] = "我要走了";
	int c_size = 0;
	while(1)
	{
		c_size = read(*clifd, cmd, sizeof(cmd));
		if(c_size == -1)
		{
			pf("[%s] read函数出错!\n", get_time(2));
		}
		
		if (strcmp(up, cmd) == 0)
		{
			pf("[%s] 收到客户端的上传指令\n", get_time(2));
			c_up(clifd);
			memset(cmd, 0, 20);
		}
		else if (strcmp(down, cmd) == 0)
		{
			pf("[%s] 收到客户端的下载指令\n", get_time(2));
			c_down(clifd);
			memset(cmd, 0, 20);
		}
		else if (strcmp(see, cmd) == 0)
		{
			pf("[%s] 收到客户端的目录指令\n", get_time(2));
			c_list(clifd);
			memset(cmd, 0, 20);
		}
		else if (strcmp(quit, cmd) == 0)
		{
			pf("[%s] 收到服务端的退出指令\n", get_time(2));
			pthread_exit(0);//结束线程
			return (void *)NULL;
		}
		
	}
}

//上传
void c_up(int *clifd)
{
	int flag = 0;
	int r_size = 0;//read_size
	int w_size = 0;
	char buf[1024] = {};

	w_size = write(*clifd, "success", 8);
	read(*clifd, buf, 10);//判断客户端用户是否输入了正确的文件名
	if(strncmp(buf, "error", 10) == 0)
	{
		printf("[%s] 收到客户端返回error,接收终止\n", get_time(2));
		return;
	}
	else if(strncmp(buf, "success", 10) == 0)
	{
		printf("[%s] 收到客户端返回success,继续接收\n", get_time(2));
	}
	else
	{
		printf("[%s] 收到客户端异常数据:%s,接收终止\n", get_time(2), buf);
		return;
	}

	//用于存储文件名,长度50字节
	char filename[50] = {};
	memset(filename, 0, sizeof(filename));
	//发送success给客户端,告知客户端可以传输文件名
	w_size = write(*clifd, "success", 8);
	int f_size = read(*clifd, filename, sizeof(filename));
	if(f_size == -1)
	{
		pf("[%s] read函数出错!\n", get_time(2));
	}
	pf("[%s] 收到文件名:%s\n", get_time(2), filename);
	usleep(1000);

	//传回文件名,用于数据可靠性校验
	w_size = write(*clifd, filename, strlen(filename) + 1);
	//读取客户端返回的结果
	r_size = read(*clifd, buf, sizeof(buf));
	if(strncmp(buf, "success", 7) == 0)
	{
		pf("[%s] 文件名校验成功,准备接收文件\n", get_time(2));
	}
	else
	{
		pf("[%s] 文件名校验失败,终止接收文件\n", get_time(2));
		return;
	}

	//发送success给客户端,告知可以开始文件传输
	w_size = write(*clifd, "success", 8);
	pf("[%s] 发送success给客户端,可以开始文件传输\n", get_time(2));

	int fd = open(filename, O_CREAT | O_RDWR, 0777);//读写模式打开/新建文件

	do
	{
		memset(buf, 0, sizeof(buf));
		r_size = read(*clifd, buf, sizeof(buf));
		pf("[收到字节数:%d ", r_size);

		w_size = write(fd, buf, r_size);
		pf("写入文件字节数:%d ", w_size);

		usleep(10000);
		
		w_size = write(*clifd, "success", 8);
		pf("发送success给客户端 ]\n");

		flag++;
	} while (r_size == 1024);

	sleep(1);

	if (flag > 0)
	{
		char result[20] = "success";
		pf("[%s]     文件传输完毕 返回客户端success\n", get_time(2));
		write(*clifd, result, strlen(result) + 1);
	}
	else
	{
		char result[20] = "error";
		pf("[%s]     文件传输失败 返回客户端error\n", get_time(2));
		write(*clifd, result, strlen(result) + 1);
	}
	close(fd);
	return;
}

//下载
void c_down(int *clifd)
{
	DIR *dir;
	dir = opendir(".");
	char list[1024] = {};
	struct dirent *dirent;
	int r_size = 0;
	int w_size = 0;
	char buf[1024] = {};
	char buf2[20] = {};
	char filename[50] = {};
	char filename2[51] = {};

	usleep(10000);
	w_size = write(*clifd, "success", 8);
	r_size = read(*clifd, buf, sizeof(buf));
	if(strncmp(buf, "success", 8) == 0)
	{
		pf("[%s] 收到客户端success信息,可以进行目录列表发送\n", get_time(2));
	}

	//获取目录下所有文件名
	while ((dirent = readdir(dir)) != NULL)
	{
		strcat(list, dirent->d_name);
		strcat(list, " ");
	}
	pf("当前目录列表:%s\n", list);
	pf("strlen(list):%d\n", (int)strlen(list));
	int l_size = write(*clifd, list, strlen(list)+1);
	if(l_size == -1)
	{
		pf("[%s] read函数出错!\n", get_time(2));
		return;
	}
	pf("[%s] 发送当前下载目录列表给客户端\n", get_time(2));

	pf("[%s] 等待接收文件名...\n", get_time(2));
	int f_size = read(*clifd, filename, sizeof(filename));
	if(f_size == -1)
	{
		pf("[%s] read函数出错!\n", get_time(2));
	}
	//pf("filename:%s\n", filename);
	strncpy(filename2, filename, 50);
	strcat(filename2, " ");
	if (strstr(list, filename2) == NULL || strncmp(filename2, " ", 1) == 0 || strncmp(filename2, "  ", 2) == 0)
	{
		char result[6] = "error";
		pf("[%s] 文件:%s 不存在,下载终止\n", get_time(2), filename);

		write(*clifd, result, strlen(result));
		return;
	}
	else
	{
		char result[8] = "success";
		pf("[%s] 文件:%s 存在,开始传输文件内容\n", get_time(2), filename);
		write(*clifd, result, strlen(result));

		memset(buf, 0, sizeof(buf));
		snprintf(buf, 150, "ls -l %s | awk '{print $5}'", filename);//获取下载文件大小，awk '{print $5}'输入文件第五列（文件大小）
		FILE* temp_fp = NULL;
		temp_fp = popen(buf, "r");
		if (temp_fp == NULL)
		{
			pf("[%s] 获取文件大小失败\n", get_time(2));
		}
		memset(buf, 0, sizeof(buf));
		fscanf(temp_fp, "%s", buf);
		pf("[%s] 文件大小:%sB\n", get_time(2), buf);
		pclose(temp_fp);
		memset(buf, 0, sizeof(buf));

		int fd = open(filename, O_RDONLY);

		//设置文件读写位置为文件尾部
		lseek(fd, 0, SEEK_END);
		//获取文件字节数（尾部位置）
		off_t end_pos = lseek(fd, 0, SEEK_CUR);
		//pf("end_pos:%d\n", end_pos);
		//设置文件读写位置为文件头部
		lseek(fd, 0, SEEK_SET);

		sleep(1);

		do
		{
			memset(buf, 0, sizeof(buf));
			r_size = read(fd, buf, sizeof(buf));
			pf("[读取文件字节数:%d ", r_size);
			w_size = write(*clifd, buf, r_size);
			pf("发送字节数:%d ", w_size);
			read(*clifd, result, sizeof(result));
			if(strncmp(result, "success", 10) == 0)
			{
				pf("成功收到客户端端返回的success]\n");
			}
			usleep(10000);

			off_t cur_pos = lseek(fd, 0, SEEK_CUR);
			//pf("cur_pos:%d\n", cur_pos);
			//添加结束符'\0'
			if(cur_pos == end_pos && w_size == 1024)
			{
				char end[1] = "\0";
				pf("[读取文件字节数:1 ");
				w_size = write(*clifd, end, sizeof(end));
				pf("发送字节数:%d ", w_size);
				read(*clifd, buf2, sizeof(buf2));
				
				if(strncmp(buf2, "success", 10) == 0)
				{
					pf("成功收到客户端端返回的success]\n");
				}
				else
				{
					pf("收到客户端返回的异常数据:%s]\n", buf2);
				}
				break;
			}
		} while (r_size == 1024);
		sleep(1);
		pf("[%s]     文件:%s 发送完毕\n", get_time(2), filename);
		close(fd);
	}

	return;
}

//文件列表
void c_list(int *clifd)
{
	DIR *dir;
	dir = opendir(".");
	char list[1024] = {};
	struct dirent *dirent;
	while ((dirent = readdir(dir)) != NULL)
	{
		strcat(list, dirent->d_name);
		strcat(list, " ");
	}

	int l_size = write(*clifd, list, strlen(list) + 1);//发送当前目录给客户端
	if(l_size == -1)
	{
		pf("[%s] write函数出错!\n", get_time(2));
	}

	memset(list, 0, 1024);

	char dirname[20] = {};
	int d_size = read(*clifd, dirname, sizeof(dirname));
	if(d_size == -1)
	{
		pf("[%s] read函数出错!\n", get_time(2));
	}
	pf("[%s] 收到客户端的数据:%s\n", get_time(2), dirname);

	if(strncmp(dirname, "...", 20) == 0)//非cd指令
	{
		closedir(dir);
		return;
	}

	if(strncmp(dirname, ".", 20) == 0)
	{
		dir = opendir(".");
		while ((dirent = readdir(dir)) != NULL)
		{
			strcat(list, dirent->d_name);
			strcat(list, " ");
		}
		l_size = write(*clifd, list, strlen(list) + 1);
	}
	else if (strncmp(dirname, "..", 20) == 0)
	{
		chdir("..");//切换为上级目录
		dir = opendir(".");
		while ((dirent = readdir(dir)) != NULL)
		{
			strcat(list, dirent->d_name);
			strcat(list, " ");
		}
		l_size = write(*clifd, list, strlen(list) + 1);
	}
	else
	{
		int re = chdir(dirname);
		if (re == -1)
		{
			char result[20] = "目录名错误";
			pf("[%s] 是cd指令,但没有这个目录\n", get_time(2));
			int err = write(*clifd, result, strlen(result) + 1);
			if(err == -1)
			{
				pf("[%s] write函数出错!\n", get_time(2));
			}
		}
		else
		{
			pf("[%s] 修改服务器目录成功\n", get_time(2));
			dir = opendir(".");
			while ((dirent = readdir(dir)) != NULL)
			{
				strcat(list, dirent->d_name);
				strcat(list, " ");
			}
			l_size = write(*clifd, list, strlen(list) + 1);
		}
	}
	closedir(dir);
	return;
}