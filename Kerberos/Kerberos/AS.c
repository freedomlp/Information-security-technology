#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include "DES.c"

#define AS_PORT 1500         //端口号
#define BACKLOG 100          //最大监听数
#define MAX_DATA 200         //缓冲区大小
#define time_limit 10000000  //有效期

char ID[] = "Bob";
char K_client[] = "AAAAAAAAAAAAAAAA";
char K_client_TGS[] = "BBBBBBBBBBBBBBBB";
char K_TGS[] = "CCCCCCCCCCCCCCCC";

void serve(struct sockaddr_in * client_addr, int client_socket)
{
    printf("Client ip is %s, client port is %d\n", inet_ntoa(client_addr->sin_addr), htons(client_addr->sin_port));
    printf("accepting message C and D...\n");
    char buf[MAX_DATA];       //储存接收数据
    int messageLen = recv(client_socket, buf, MAX_DATA, 0);
    buf[messageLen] = 0;
    if(strcmp(ID, buf) != 0)
    {
        printf("ID is not correct");
        send(client_socket, "ID is not correct", strlen("ID is not correct"), 0);
        return;
    }
    else
    {
        printf("sending message A...\n");
        char *A = DES_encrypt(K_client_TGS, K_client);
        send(client_socket, A, strlen(A), 0);

        sleep(1);
        printf("sending message B....\n");
        char TGT[100];
        sprintf(TGT, "<%s,%s,%ld,%s>", ID, inet_ntoa(client_addr->sin_addr),time(NULL) + time_limit, K_client_TGS);
        printf("TGT is %s\n", TGT);
        char *B = DES_encrypt(TGT, K_TGS);
        send(client_socket, B, strlen(B), 0);

        printf("AS server has finished!\n");
    }
}

int main()
{
    int AS_socket, client_socket;     //AS用于监听的套接字和建立连接后的套接字
    struct sockaddr_in AS_addr;       //AS的地址信息结构体，下面有具体的属性赋值
    struct sockaddr_in client_addr;   //客户端地址信息
    int addr_len = sizeof(struct sockaddr_in);

    AS_socket = socket(AF_INET, SOCK_STREAM, 0);     //建立socket
    if (AS_socket == -1)
    {
        perror("AS socket");
        return -1;
    }
    else
    {
        printf("AS socket number = %d\n", AS_socket);
    }
    AS_addr.sin_family = AF_INET;                   //用于网络通信
    AS_addr.sin_port = htons(AS_PORT);              //端口号
    AS_addr.sin_addr.s_addr = htonl(INADDR_ANY);    //IP，括号内容表示本机IP
    bzero(&(AS_addr.sin_zero), 8);                  //将sin_zero字段置0
    if (bind(AS_socket, (struct sockaddr *)&AS_addr, sizeof(struct sockaddr)) < 0) //绑定地址结构体和socket
    {
        perror("bind error");
        return -1;
    }
    else
    {
        printf("bind succeed\n");
    }
    listen(AS_socket, BACKLOG);     //开启监听 ，第二个参数是最大监听数
    printf("AS server is listening at %d port\n", AS_PORT);

    while (1)
    {
        //阻塞直到有客户端连接
        client_socket = accept(AS_socket, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
        if (client_socket == -1)
        {
            printf("connect client failed");
            return -1;
        }
        
        printf("connect succeed\n");
        pid_t pid = fork();
        if(pid == 0)
            serve(&client_addr, client_socket);
    }
    close(AS_socket);
    return 0;
}