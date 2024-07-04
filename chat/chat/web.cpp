#include "hat.h"
using namespace std;



int setnonblocking(int fd)
{
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void* recv_message(void* fd)
{
    int sockfd = *(int*)fd;
    char buffer[1024];
    while (1)
    {
        memset(buffer, 0, 1024);
        int n;
        if ((n = recv(sockfd, buffer, 1024, 0)) == -1)
        {
            perror("recv error.");
            exit(1);
        }
        if (strcmp(buffer, "byebye.") == 0)
        {
            cout << "Client closed." << endl;
            close(sockfd);
            exit(1);
        }
        printf("Client: %s
            ", buffer);
    }
}

int main()
{
    int listenfd, connfd;
    socklen_t clilen;
    pthread_t recv_tid;
    struct sockaddr_in servaddr, cliaddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket error.");
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(5005);

    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind error.");
        return 1;
    }

    if (listen(listenfd, LISTENQ) < 0)
    {
        perror("listen error.");
        return 1;
    }

    clilen = sizeof(cliaddr);
    if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen)) < 0)
    {
        perror("accept error.");
        return 1;
    }

    cout << "server: got connection from :" << inet_ntoa(cliaddr.sin_addr) << endl;

    if (pthread_create(&recv_tid, NULL, recv_message, &connfd) == -1)
    {
        perror("pthread create error.");
        return 1;
    }

    int epollfd = epoll_create1(0);
    if (epollfd == -1)
    {
        perror("epoll_create1 error.");
        exit(1);
    }

    struct epoll_event event;
    event.data.fd = connfd;
    event.events = EPOLLIN | EPOLLET; // Edge-triggered (ET) mode
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &event) == -1)
    {
        perror("epoll_ctl error.");
        exit(1);
    }

    char data[1024];
    while (true)
    {
        struct epoll_event events[MAX_EVENTS];
        int num_events = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < num_events; i++)
        {
            if (events[i].data.fd == connfd)
            {
                if (events[i].events & EPOLLIN)
                {
                    memset(data, 0, 1024);
                    int n = recv(connfd, data, 1024, 0);
                    if (n <= 0)
                    {
                        close(connfd);
                        break;
                    }
                    if (strcmp(data, "byebye.") == 0)
                    {
                        cout << "Client closed." << endl;
                        close(connfd);
                        break;
                    }
                    printf("Client: %s
                        ", data);
                }
            }
        }
    }

    close(listenfd);
    return 0;
}
