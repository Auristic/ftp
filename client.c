#include "client.h"

int main()
{
	pf("[%s] 创建客户端socket...\n", get_time(2));
	sockfd = socket(AF_INET, SOCK_STREAM, 0);//创建socket,ipv4通信,TCP协议
	
	if (sockfd == -1)
	{
		perror("socket error");
		return -1;
	}

	pf("[%s] 创建并设置服务器socket地址...\n", get_time(2));
	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;//ipv4
	addr.sin_port = htons(8888);//8888端口
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");//服务器地址
	socklen_t len = sizeof(addr);//套接字结构体长度

	pf("[%s] 绑定连接服务器...\n", get_time(2));
	if (connect(sockfd, (struct sockaddr *)&addr, len))
	{
		perror("connect error");
		return -1;
	}

	menu();//加载菜单

	close(sockfd);//关闭套接字

	return 0;
}
