#include "server.h"


int main()
{
	pf("[%s] 开始运行服务端，输入q退出...\n", get_time(2));
	pf("[%s] 服务器创建socket...\n", get_time(2));
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);//TCP协议,ipv4
	if (sockfd == -1)
	{
		perror("socket error");
		return -1;
	}

	pf("[%s] 准备地址...\n", get_time(2));
	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	socklen_t len = sizeof(addr);

	signal(SIGIO, sig_quit);//注册信号量,SIGIO可打断阻塞程序

	pf("[%s] 绑定socket与地址...\n", get_time(2));
	if (bind(sockfd, (struct sockaddr *)&addr, len))
	{
		perror("bind error");
		return -1;
	}

	pf("[%s] 设置监听...\n", get_time(2));
	if (listen(sockfd, 5))//客户端最大连接数为5
	{
		perror("listen error");
		return -1;
	}

	pf("[%s] 等待客户端连接...\n", get_time(2));

	pid_t m_pid;//主进程id
	m_pid = getpid();
	pthread_t tid;//子线程id
	pthread_create(&tid,NULL,callback,&m_pid);//创建退出线程,方便服务端退出

	while(1)
	{
		if(setjmp(jmpbuf)) {//设置跳转点
			pf("[%s] 退出服务端\n", get_time(2));
        	break;
    	}
		struct sockaddr_in addrcli = {};//创建客户端地址
		int *clifd = (int*)malloc(sizeof(int));//创建指向连接客户端套接字描述符的指针
		*clifd = accept(sockfd, (struct sockaddr *)&addrcli, &len);//接收请求,建立与客户端的连接
		if (*clifd == -1){
			perror("accept error");
			continue;
		}
		//每个与客户端的连接都创建一个子线程
		pthread_t pid;
		pthread_create(&pid, NULL, start_run, (void *)clifd);
	}

	return 0;
}

