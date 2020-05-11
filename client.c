/*chatroom client*/
#include<pthread.h>
#include<fcntl.h>
#include "chatroom.h"

#define NAME_SIZE 20

void *readfromnet(void *sock);
void *writetonet(void *sock);

char name[NAME_SIZE];

//pthread_mutex_t mutex;

int main(void)
{
    // pthread_mutex_init(&mutex, NULL);

    //print welcome
    char *filename = "start";
    char buff[BUFF_SIZE];
    int file = open(filename, O_RDONLY);
    while(read(file, buff, sizeof(buff))) puts(buff);
    close(file);
    

    printf("Welcome to the Chatroom v0.01!\n");
    printf("please input your name: ");
    scanf("%s", name);

    int clnt_sock = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//test
    serv_addr.sin_port = htons(2200);

    if(connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error("connect error");
    puts("connected to the chatroom");
    
    int sock_flag = fcntl(clnt_sock, F_GETFL, 0);
    fcntl(clnt_sock, F_SETFL, sock_flag|O_NONBLOCK);

    //memset(buff, 0, sizeof(buff));
    sprintf(buff, "[%s]", name);
    send(clnt_sock, buff, strlen(buff), 0);//tell everyone you're in

    pthread_t recv_id, send_id;
    pthread_create(&recv_id, NULL, readfromnet, (void*)&clnt_sock);
    pthread_create(&send_id, NULL, writetonet, (void*)&clnt_sock);

    void *thread_return;
    pthread_join(send_id, &thread_return);
    pthread_join(recv_id, &thread_return);
    

    close(clnt_sock);
    return 0;
}

void *readfromnet(void *sock)
{
    int clnt_sock = *(int*)sock;
    int str_len;

    while(1)
    {
        //pthread_mutex_lock(&mutex);
        char buff[BUFF_SIZE];
        //puts("reading...");

        str_len = read(clnt_sock, buff, sizeof(buff));
        if(str_len == -1)
        {
            //pthread_mutex_unlock(&mutex);
            sleep(1);
            continue;
        }
        else if(str_len == 0)
        {
            puts("server disconnected");
            break;
        }
        buff[str_len] = '\0';
        printf("%s\n", buff);

        //pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void *writetonet(void *sock)
{
    int clnt_sock = *(int*)sock;

    while(1)
    {
        //pthread_mutex_lock(&mutex);
        //puts("writing..");
        char buff[BUFF_SIZE];
        sprintf(buff, "[%s]: ", name);
        //printf("[%s]: ", name);
        scanf("%s", &buff[strlen(buff)]);
        if(!strcmp(buff, ":q")||!strcmp(buff, ":Q"))
        {
            puts("bye~!");
            //pthread_mutex_unlock(&mutex);
            close(clnt_sock);
            return NULL;
        }
        send(clnt_sock, buff, strlen(buff), 0);

        //pthread_mutex_unlock(&mutex);
    }
}


