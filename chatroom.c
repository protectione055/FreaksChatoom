#include "chatroom.h"
void error(char *msg)
{
    puts(msg);
    system("pause");
    exit(1);
}

void sendtoclnt(clnt_info *clnt_head, char * msg, int str_len)
{
    clnt_info *cptr = clnt_head;
    while(cptr != NULL)
    {
        if(startswith(msg, cptr->nickname, strlen(cptr->nickname)))
        {
            cptr = cptr->next;
            continue;
        }
     
        //printf("send message to %s\n", cptr->nickname);
        if(send(cptr->sock, msg, str_len, 0) == -1)
        {
            printf("%s", strerror(errno));
        }
        cptr = cptr->next;
    }
}

int startswith(char *str1, char *str2, size_t len)
{
    int i;
    for(i = 0; i < len && str1[i] == str2[i]; ++i)
    {
        ;
    }
    if(str2[i] == '\0')
        return 1;
    else return 0;
}
