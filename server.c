/*Multiperson Internet Chatroom, based on TCP*/
/*Using epoll architecture*/
#include<sys/epoll.h>
#include "chatroom.h"

static int flag = 1;
void close_server(int sig);

int main(void)
{
    int str_len = 0, clnt_cnt = 0;
    char buff[BUFF_SIZE];
    clnt_info *clnt_head = NULL, *clnt_tail = NULL;
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(2200);

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error("bind error");
    if(listen(serv_sock, 10) == 0)
        puts("server is listening on port: 2200");

    int epfd = epoll_create(EPOLL_SIZE);
    struct epoll_event event;
    struct epoll_event *ep_events = malloc(sizeof(event)*EPOLL_SIZE);
    event.data.fd = serv_sock;
    event.events = EPOLLIN|EPOLLET;//EdgeTrigger
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    signal(SIGINT, close_server);

    while(flag)//use signal to break the loop
    {
        //printf("serv_sock = %d\n", serv_sock);
        //printf("remaining %d people in the chat room\n", clnt_cnt);
        int epoll_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if(epoll_cnt == -1)
            error("epoll error");
        
        int i;
        char name[20];
        for(i = 0; i < epoll_cnt; ++i)
        {
            if(ep_events[i].data.fd == serv_sock)
            {
                clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_len);
                clnt_cnt++;
                event.data.fd = clnt_sock;
                event.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);

                str_len = read(clnt_sock, name, sizeof(name));//get client's name
                name[str_len] = '\0';

                if(clnt_head == NULL)
                {
                    clnt_head = malloc(sizeof(clnt_info));
                    clnt_tail = clnt_head;
                    clnt_head->sock = clnt_sock;
                    strcpy(clnt_head->nickname, name);
                    clnt_head->next = NULL;
                }
                else
                {
                    clnt_tail->next = malloc(sizeof(clnt_info));
                    clnt_tail = clnt_tail->next;
                    clnt_tail->sock = clnt_sock;
                    clnt_tail->next = NULL;
                    strcpy(clnt_tail->nickname, name);
                }
                sprintf(buff, "%s entered the room.", name);
                sendtoclnt(clnt_head, buff, strlen(buff));//tell all the socket a new client entered
                printf("%s entered the room.\n", name);
                int sockflag = fcntl(clnt_sock, F_GETFL, 0);
                fcntl(clnt_sock, F_SETFL, sockflag|O_NONBLOCK);
            }
            else
            {
                while(1)
                {
                    str_len = read(ep_events[i].data.fd, buff, sizeof(buff));
                    if(str_len == 0)//close current clnt_sock
                    {
                        int tmp_sock = ep_events[i].data.fd;
                        clnt_info *tmp_ptr = clnt_head, *prvs_ptr = clnt_head;
                        epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
                        while(tmp_ptr->sock != tmp_sock)//delete the sock from link
                        {
                            prvs_ptr = tmp_ptr;
                            tmp_ptr = tmp_ptr->next;
                        }
                        sprintf(buff, "%s left the room\n", tmp_ptr->nickname);
                        puts(buff);
                        prvs_ptr->next = tmp_ptr->next;
                        sendtoclnt(clnt_head, buff, strlen(buff));
                        free(tmp_ptr);
                        clnt_cnt--;
                        close(ep_events[i].data.fd);
                        break;
                    }
                    else if(str_len < 0)
                    {
                        //puts("What had happened...");
                        //printf("%s", strerror(errno));
                        break;
                    }

                    else
                    {
                        buff[str_len] = '\0';
                        sendtoclnt(clnt_head, buff, str_len);//send messages to all clients
                        puts(buff);
                    }
                }
            }
            
        }
    }
    close(serv_sock);
    close(epfd);
    return 0;
}

void close_server(int sig)
{
    if(sig == SIGINT)
    {
        puts("chatroom server closed!");
        flag = 0;
        exit(0);
    }
}