#include "hat.h"
using namespace std;
/*������շ�������Ϣ����*/
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

		//���յ�����exit�ַ���������˳�ͨ��
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
	/*�����׽��ֺ����ӷ�������ַ*/
	int sockfd;
	pthread_t recv_tid, send_tid;
	struct sockaddr_in servaddr;

	/*�ж��Ƿ�Ϊ�Ϸ�����*/
	if (argc != 2)
	{
		perror("usage:tcpcli <IPaddress>");
		return 1;
	}//if

	/*(1) �����׽���*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket error");
		return 1;
	}//if

	/*(2) �������ӷ�������ַ�ṹ*/
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5005);
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0)
	{
		cout << "inet_pton error for %s" << argv[1] << endl;
		return 1;
	}//if

	/*(3) �������ӷ���������*/
	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("connect error");
		return 1;
	}//if	

	/*�������̴߳���ÿͻ����ӽ�����Ϣ*/
	if (pthread_create(&recv_tid, NULL, recv_message, &sockfd) == -1)
	{
		perror("pthread create error.\n");
		return 1;
	}//if	

	/*����ͻ��˷�����Ϣ*/
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
			pthread_join(recv_tid, NULL); // �ȴ����߳̽���
			exit(0);
		}//if
		if (send(sockfd, data, strlen(data), 0) == -1)
		{
			perror("send error.\n");
			return 1;
		}//if


	}//while
}
