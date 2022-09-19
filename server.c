#include "server.h"

int main()
{
	pf("[%s] 服务器创建socket...\n", get_time(2));
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);//TCP协议，ipv4
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
	while(1)
	{
		struct sockaddr_in addrcli = {};//创建客户端地址
		int *clifd = (int*)malloc(sizeof(int));//创建指向连接客户端套接字描述符的指针
		*clifd = accept(sockfd, (struct sockaddr *)&addrcli, &len);//接收请求，建立与客户端的连接
		if (*clifd == -1)
		{
			perror("accept error");
			continue;
		}
		//创建子线程
		pthread_t pid;
		pthread_create(&pid, NULL, start_run, (void *)clifd);
	}

	return 0;
}

