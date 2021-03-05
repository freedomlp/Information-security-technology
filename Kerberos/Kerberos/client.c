#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include "DES.c"

#define AS_PORT 1500
#define TGS_PORT 1600
#define SS_PORT 1700 
#define DEST_IP "127.0.0.1"   //本机回环IP地址
#define MAX_DATA 200          //缓冲区大小

char ID[] = "Bob";
char K_client[] = "AAAAAAAAAAAAAAAA";

struct sockaddr_in * set_sock_addr(int port)
{
    struct sockaddr_in *server_addr =  (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr - inet_addr(DEST_IP);
    server_addr->sin_port = htons(port);
    bzero(&(server_addr->sin_zero), 8);
    return server_addr;
}

int create_socket()
{
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
        perror("client socket");
    return client_socket;
}

int main()
{
    int client_socket;
    int messageLen;                //recv函数返回值
    time_t timestamp;              //客户端申请服务时的时间戳，每个阶段用到时要更新
    char temp[MAX_DATA];           //可重用的临时缓冲区
    char bufA[MAX_DATA];           //存放消息A的缓冲区
    char bufB[MAX_DATA];           //存放消息B的缓冲区
    char C[MAX_DATA];              //消息C
    char *D;                       //消息D
    char bufE[MAX_DATA];           //存放消息E的缓冲区
    char bufF[MAX_DATA];           //存放消息F的缓冲区
    char *G;                       //消息G
    char bufH[MAX_DATA];           //存放消息H的缓冲区
    char *K_client_TGS;
    char *K_client_SS;
    char serviceID[] = "study";    //申请服务的ID


    //设置三个服务器的地址
    struct sockaddr_in * AS_addr = set_sock_addr(AS_PORT);
    struct sockaddr_in * TGS_addr = set_sock_addr(TGS_PORT);
    struct sockaddr_in * SS_addr = set_sock_addr(SS_PORT);

    //第一阶段
    if((client_socket = create_socket()) == -1)
        return -1;
    
    //与AS服务器建立连接
    if (connect(client_socket, (struct sockaddr *)AS_addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("AS connect");
        return -1;
    }
    printf("+-----------------------------------------------------+\n");
    printf("connect AS server succeed, first stage begin!\n");

    printf("sending ID to server...\n");
    send(client_socket, ID, strlen(ID), 0);

    printf("accepting message A...\n");
    messageLen = recv(client_socket, bufA, MAX_DATA, 0);
    bufA[messageLen] = 0;
    if(strcmp(bufA, "ID is not correct") == 0)
    {
        printf("ID is not correct, please check your ID\n");
        return -1;
    }

    printf("accepting message B...\n");
    messageLen = recv(client_socket, bufB, MAX_DATA, 0);
    bufB[messageLen] = 0;

    K_client_TGS = DES_decrypt(bufA, K_client);
    printf("K_client_TGS is %s\n", K_client_TGS);
    close(client_socket);
    printf("first stage has finished!\n");
    printf("+-----------------------------------------------------+\n");

    //第二阶段
    if((client_socket = create_socket()) == -1)
        return -1;

    //与TGS服务器建立连接
    if (connect(client_socket, (struct sockaddr *)TGS_addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("TGS connect");
        return -1;
    }
    printf("connect TGS server succeed, second stage begin!\n");
    timestamp = time(NULL);

    //生成并发送消息C
    printf("sending message C...\n");
    sprintf(C, "%s,%s", serviceID, bufB);
    send(client_socket, C, strlen(C), 0);

    sleep(1);
    //生成并发送消息D
    printf("sending message D...\n");
    sprintf(temp, "<%s,%ld>", ID, timestamp);
    D = DES_encrypt(temp, K_client_TGS);
    send(client_socket, D, strlen(D), 0);

    printf("accepting message E...\n");
    messageLen = recv(client_socket, bufE, MAX_DATA, 0);
    bufE[messageLen] = 0;

    //认证过程
    if(strcmp(bufE, "invalid ID") == 0 || strcmp(bufE, "invalid TGT") == 0)
    {
        printf("authentication failed, check your ID or TGT!\n");
        return -1;
    }

    printf("accepting message F...\n");
    messageLen = recv(client_socket, bufF, MAX_DATA, 0);
    bufF[messageLen] = 0;
    K_client_SS = DES_decrypt(bufF, K_client_TGS);
    printf("K_client_SS is %s\n", K_client_SS);
    close(client_socket);
    printf("second stage has finished!\n");
    printf("+-----------------------------------------------------+\n");

    //第三阶段
    if((client_socket = create_socket()) == -1)
        return -1;

    //与SS服务器建立连接
    if (connect(client_socket, (struct sockaddr *)SS_addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("TGS connect");
        return -1;
    }
    printf("connect SS server succeed, third stage begin!\n");
    timestamp = time(NULL);

    //发送消息E
    printf("sending message E...\n");
    send(client_socket, bufE, strlen(bufE), 0);
    sleep(1);

    //生成并发送消息G
    printf("sending message G...\n");
    sprintf(temp, "<%s,%ld>", ID, timestamp);
    G = DES_encrypt(temp, K_client_SS);
    send(client_socket, G, strlen(G), 0);
    
    //接收消息H
    printf("accepting message H...\n");
    messageLen = recv(client_socket, bufH, MAX_DATA, 0);
    bufH[messageLen] = 0;
    char *H = DES_decrypt(bufH, K_client_SS);
    printf("message H is %s\n", H);
    char H_client_ID[MAX_DATA];
    time_t new_timestamp;
    sscanf(H, "<%[^,], %ld>", H_client_ID, &new_timestamp);

    if(timestamp + 1 == new_timestamp)
    {
        printf("SS is reliable!\n");
        printf("third stage has finished!\n");
        printf("+-----------------------------------------------------+\n");
        printf("could start service!\n");
    }
    else
    {
        printf("SS is not reliable!\n");
    }
    return 0;
}