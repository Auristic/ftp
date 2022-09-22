#include "client.h"

//声明全局变量
int sockfd = 0;


//加载菜单
void menu(void)
{
	system("clear");
	pf("                    *                             \n");
	pf("                   * *           /                \n");
	pf("      .       \\ \\ *   *   / /                   \n");
	pf("           \\     *     *     /  /   .            \n");
	pf("          \\  \\ \\*       *  / /                 \n");
	pf("           \\ \\ *  \\  /   *  / /                \n");
	pf("              *   | | |   *                       \n");
	pf("             * \\ | | |  /  *                     \n");
	pf("*************               ***************       \n");
	pf(" *            <<.........>>             *         \n");
	pf("   *             .......               *          \n");
	pf("     *          .........            *            \n");
	pf("       *       ..欢迎访问..         *             \n");
	pf("         *    ..福大资源站..      *               \n");
	pf("         *   ...WELCOME TO...    *                \n");
	pf("        *    .....FZUFTP....      *               \n");
	pf("       *     ..............        *              \n");
	pf("      *     *  ...........   *      *             \n");
	pf("     *    *     .........      *     *            \n");
	pf("    *  *         .......          *   *           \n");
	pf("   * *           .......             * *          \n");
	pf("                .........                         \n");
	pf("               ===========                        \n");
	getch();
	while(1)
	{
		system("clear");//清屏
		pf("===================== 主菜单 =======================\n");
		pf("	Enter '1' : 上传    Enter '2' : 下载            \n");
		pf("	Enter '3' : 查看/修改服务端目录                  \n");
		pf("	Enter '4' : 查看客户端目录                       \n");
		pf("	Enter '0' : 退出                                \n");
		pf("====================================================\n");
		
		
		//得到 0 ~ 4 之间的字符
		switch (get_cmd('0', '4'))
		{
			case '1':
				upload();
				break;
			case '2':
				download();
				break;
			case '3':
				s_list();
				break;
			case '4':
				c_list();
				break;
			case '0':
				quit();
				return;
		}
	}
}

//上传
void upload(void)
{
	char up[20] = "我想上你";
	write(sockfd, up, strlen(up) + 1);

	// 打印当前目录文件
	c_list();

	int r_size = 0;
	int w_size = 0;
	char buf[1024] = {};
	char buf2[20] = {};
	r_size = read(sockfd, buf, sizeof(buf));
	if(strncmp(buf, "success", 10) != 0)
	{
		pf("[%s] 收到服务端异常数据\n", get_time(2));
		getch();
		return;
	}

	char pathname[100] = {};
	char *filename = malloc(50);
	memset(filename, 0, 50);
	while(1)
	{
		pf("请输入文件名:");
		get_str(pathname, 100);
		if(!strncmp(pathname, "..", 3) || !strncmp(pathname, ".", 3))
		{
			pf(".或..不是普通文件,请重新输入!\n");
			continue;
		}
		break;
	}	

	int fd = open(pathname, O_RDONLY);//只读模式

	if (fd == -1)
	{
		pf("[%s] 文件不存在\n", get_time(2));
		write(sockfd, "error", 6);
		getch();
	}
	else
	{
		write(sockfd, "success", 8);
		r_size = read(sockfd, buf, sizeof(buf));
		if(strncmp(buf, "success", 8) == 0)
		{
			pf("[%s] 服务端已经可以接收文件,准备发送文件名\n", get_time(2));
		}

		if (strrchr(pathname, '/') == NULL)
		{
			strncpy(filename, pathname, 50);
		}
		else
		{
			filename = strrchr(pathname, '/');//截取最后一个'/'后面的文件名
			filename += 1;
		}

		pf("[%s] 发送文件名:%s 至服务端\n", get_time(2), filename);
		write(sockfd, filename, strlen(filename) + 1);

		//读取服务端返回的文件名,用于判断服务端是否获取到正确的用户名
		r_size = read(sockfd, buf, sizeof(buf));
		if(strncmp(buf, filename, strlen(filename)) == 0)
		{
			//发送success给服务端,准备接收文件数据
			write(sockfd, "success", 8);
			pf("[%s] 校验服务端接收到的文件名成功,准备开始文件传输\n", get_time(2));
		}
		else
		{
			// 发送failed给服务端,终止接收文件
			write(sockfd, "failed", 7);
			pf("[%s] 校验服务端接收到的文件名失败,文件传输终止\n", get_time(2));
			return;
		}
		r_size = 0;

		memset(buf, 0, sizeof(buf));
		r_size = read(sockfd, buf, sizeof(buf));
		if(strncmp(buf, "success", 10) != 0)
		{
			pf("[%s] 收到服务端异常数据\n", get_time(2));
			getch();
			return;
		}
		else
		{
			pf("[%s] 收到服务端返回success,可以开始文件传输\n", get_time(2));
		}
		
		sleep(1);

		memset(buf, 0, sizeof(buf));
		snprintf(buf, 150, "ls -l %s | awk '{print $5}'", pathname);//第五列值（即文件大小,字节为单位）
		FILE* temp_fp = NULL;//指向的文件存放上传文件大小
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

		//设置文件读写位置为文件尾部
		lseek(fd, 0, SEEK_END);
		// 获取文件字节数（尾部位置）
		off_t end_pos = lseek(fd, 0, SEEK_CUR);
		//pf("end_pos:%d\n", end_pos);
		//设置文件读写位置为文件头部
		lseek(fd, 0, SEEK_SET);

		do
		{
			//pf("进入while循环...\n");
			r_size = read(fd, buf, 1024);

			pf("[读取文件字节数:%d ", r_size);

			w_size = write(sockfd, buf, r_size);
			pf("发送字节数:%d ", w_size);

			read(sockfd, buf2, sizeof(buf2));
			if(strncmp(buf2, "success", 10) == 0)
			{
				pf("成功收到服务端返回的success]\n");
			}
			usleep(10000);

			off_t cur_pos = lseek(fd, 0, SEEK_CUR);
			//pf("cur_pos:%d\n", cur_pos);
			if(cur_pos == end_pos && w_size == 1024)
			{
				char end[1] = "\0";
				pf("[读取文件字节数:1 ");
				w_size = write(sockfd, end, sizeof(end));
				pf("发送字节数:%d ", w_size);
				read(sockfd, buf2, sizeof(buf2));
				
				if(strncmp(buf2, "success", 10) == 0)
				{
					pf("成功收到服务端返回的success]\n");
				}
				else
				{
					pf("收到服务端返回的异常数据:%s]\n", buf2);
				}
				break;
			}
		} while (r_size == 1024);

		close(fd);

		char result[20] = {};
		read(sockfd, result, sizeof(result));
		if(strncmp(buf2, "success", 10) == 0)
		{
			pf("[%s] 成功收到服务端返回值:%s,服务器接收文件成功\n", get_time(2), result);
		}
		else if(strncmp(buf2, "error", 10) == 0)
		{
			pf("[%s] 成功收到服务端返回值:%s,服务器接收文件异常\n", get_time(2), result);
		}
		else
		{
			pf("[%s] 收到服务端返回值:%s,数据异常\n", get_time(2), result);
		}
			
		getch();
	}

	return;
}

//下载
void download(void)
{
	int r_size = 0;
	int w_size = 0;
	char buf[1024] = {};
	char filename[50] = {};
	char list[1024] = {};
	char down[20] = "我想下你";
	write(sockfd, down, strlen(down) + 1);

	r_size = read(sockfd, buf, sizeof(buf));
	if(strncmp(buf, "success", 10) != 0)
	{
		pf("[%s] 收到服务端异常数据\n", get_time(2));
		getch();
		return;
	}
	else
	{
		pf("[%s] 服务端成功接收命令\n", get_time(2));
	}

	//发送给服务端success,告知可以开始目录列表的发送
	write(sockfd, "success", 8);
	read(sockfd, list, sizeof(list));
	pf("服务端目录列表:%s\n", list);

	usleep(1000);

	while(1)
	{
		pf("请输入要下载的文件名:");
		get_str(filename, 50);
		if(!strncmp(filename, "..", 3) || !strncmp(filename, ".", 3))
		{
			pf(".或..不是普通文件,无法下载,请重新输入!\n");
			continue;		
		}
		break;
	}
	write(sockfd, filename, strlen(filename) + 1);

	char result[20] = {};
	read(sockfd, result, sizeof(result));
	if(strncmp(result, "success", 8) == 0)
	{
		pf("[%s] 收到服务端发送的数据:%s 文件准备下载\n", get_time(2), result);
	}
	else if(strncmp(result, "error", 8) == 0)
	{
		pf("[%s] 收到服务端发送的数据:%s 文件不存在\n", get_time(2), result);
		pf("按任意键继续");
		getch();
		return;
	}
	else
	{
		pf("[%s] 收到服务端发送的数据:%s 数据异常,下载终止\n", get_time(2), result);
		pf("按任意键继续");
		getch();
		return;
	}

	int fd = open(filename, O_CREAT | O_RDWR, 0777);

	do
	{
		usleep(500);
		memset(buf, 0, sizeof(buf));
		r_size = read(sockfd, buf, sizeof(buf));
		pf("[收到字节数:%d ", r_size);
		w_size = write(fd, buf, r_size);
		pf("写入文件字节数:%d ", w_size);
		w_size = write(sockfd, "success", 8);
		pf("发送success给服务端]\n");
	} while (r_size == 1024);
	usleep(1000000);
	pf("[%s]     文件:%s 下载完毕\n", get_time(2), filename);
	close(fd);
	getch();
	return;
}

//服务端文件列表
void s_list(void)
{
	char see[20] = "我想看你";
	write(sockfd, see, strlen(see) + 1);

	char list[1024] = {};
	read(sockfd, list, sizeof(list));
	pf("服务端目录列表: %s\n", list);

	pf("输入cd+空格+目录名,修改服务器工作目录,否则返回主菜单\n>>>");
	char cmd[50] = {};
	get_str(cmd, 50);
	if (strstr(cmd, "cd ") == NULL)
	{
		pf("非cd指令,按任意键返回主界面\n");
		snprintf(cmd, 20, "...");
		write(sockfd, cmd, strlen(cmd));
		getch();
	}
	else
	{
		char *dir = malloc(20);
		dir = strrchr(cmd, ' ');
		dir += 1;//定位目录名
		write(sockfd, dir, strlen(dir) + 1);

		read(sockfd, list, sizeof(list));
		if(strncmp(list, "目录名错误", 10) == 0){
			pf("服务端没有这个目录\n");
		}else{
			pf("服务端目录列表: %s\n", list);
		}
		pf("按任意键继续");
		getch();
	}
	return;
}

//客户端文件列表
void c_list(void)
{
	pf("当前目录列表:");
	DIR *dir;
	dir = opendir(".");
	struct dirent *dirent;
	while ((dirent = readdir(dir)) != NULL)
	{
		pf(" %s ", dirent->d_name);
	}
	pf("\n按任意键继续\n");
	closedir(dir);
	getch();
	return;
}

//退出程序
void quit(void)
{
	pf("[%s] 告知服务端,我要走了\n", get_time(2));
	char quit[20] = "我要走了";
	write(sockfd, quit, strlen(quit) + 1);

	usleep(10000);

	pf("[%s] 客户端程序退出\n", get_time(2));
	return;
}
