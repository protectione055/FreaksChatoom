#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<signal.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/fcntl.h>


#define BUFF_SIZE 1024
#define EPOLL_SIZE 50
#define NAME_LEN 20

typedef struct clnt_info
{
    int sock;
    char nickname[20];
    struct clnt_info *next;
}clnt_info;

void setnonblocking(int fd);
void sendtoclnt(clnt_info *clnt_head, char *msg, int str_len);
void error(char *msg);
int startswith(char *str1, char *str2, size_t len);