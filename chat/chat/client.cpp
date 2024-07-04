#include "hat.h"
using namespace std;
/*处理接收服务器消息函数*/
void* recv_message(void* fd)
{
	int sockfd = *(int*)fd;
	while (1)
	{
		char buffer[1024];
		memset(buffer, 0, 1024);
		int n;
		if ((n = recv(sockfd, buffer, 1024, 0)) <= 0)
		{
			perror("recv error.
				");
				exit(1);
		}//if
		buffer[n] = '\0';

		//若收到的是exit字符，则代表退出通信
		if (strcmp(buffer, "byebye.") == 0)
		{
			printf("Server is closed.
				");
				close(sockfd);
			exit(0);
		}//if

		printf("\nServer: %s\n", buffer);
	}//while
}


int main(int argc, char** argv)
{
	/*声明套接字和链接服务器地址*/
	int sockfd;
	pthread_t recv_tid, send_tid;
	struct sockaddr_in servaddr;

	/*判断是否为合法输入*/
	if (argc != 2)
	{
		perror("usage:tcpcli <IPaddress>");
		return 1;
	}//if

	/*(1) 创建套接字*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket error");
		return 1;
	}//if

	/*(2) 设置链接服务器地址结构*/
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5005);
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0)
	{
		cout << "inet_pton error for %s" << argv[1] << endl;
		return 1;
	}//if

	/*(3) 发送链接服务器请求*/
	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("connect error");
		return 1;
	}//if	

	/*创建子线程处理该客户链接接收消息*/
	if (pthread_create(&recv_tid, NULL, recv_message, &sockfd) == -1)
	{
		perror("pthread create error.\n");
		return 1;
	}//if	

	/*处理客户端发送消息*/
	char data[1024];
	memset(data, 0, 1024);
	while (fgets(data, 1024, stdin) != NULL)
	{
		if (strcmp(data, "exit\n") == 0)
		{
			cout << "byebye" << endl;
			memset(data, 0, 1024);
			strcpy(data, "byebye.\n");
			send(sockfd, data, strlen(data), 0);
			close(sockfd);
			pthread_join(recv_tid, NULL); // 等待子线程结束
			exit(0);
		}//if
		if (send(sockfd, data, strlen(data), 0) == -1)
		{
			perror("send error.\n");
			return 1;
		}//if


	}//while
}
