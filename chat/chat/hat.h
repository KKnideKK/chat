#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <cstring>
#include <sys/epoll.h>
const int LISTENQ = 1024;
const int MAX_EVENTS = 10;
const int LISTENQ = 5;


