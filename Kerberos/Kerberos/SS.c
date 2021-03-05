#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include "DES.c"

#define SS_PORT 1700         //端口号
#define BACKLOG 100          //最大监听数
#define MAX_DATA 200         //缓冲区大小
#define time_limit 10000000  //有效期


char K_SS[] = "EEEEEEEEEEEEEEEE";

void serve(struct sockaddr_in * client_addr, int client_socket)
{
    char bufE[MAX_DATA];                //接收消息E的缓冲区
    char bufG[MAX_DATA];                //接收消息G的缓冲区
    char *authenticationG;              //从消息G中解密得到的认证G
    char serviceID[MAX_DATA];           //消息E解密得到
    char *ST;                           //服务票据
    char K_client_SS[MAX_DATA];    
    char clientID1[MAX_DATA];           //ST中得到用户ID
    char clientID2[MAX_DATA];           //消息G中解密得到用户ID   
    char client_address[MAX_DATA];      //ST中得到的客户端地址
    time_t validity;                    //ST中得到的票据有效期
    time_t timestamp;                   //申请服务时客户端的时间戳
    char *H;                            //发回给客户端的消息H
    char temp[MAX_DATA];                //可重用的临时缓冲区

    printf("Client ip is %s, client port is %d\n", inet_ntoa(client_addr->sin_addr), htons(client_addr->sin_port));
    //接收消息E
    printf("accepting message E...\n");
    int ELen = recv(client_socket, bufE, MAX_DATA, 0);
    bufE[ELen] = 0;
    sscanf(bufE, "%[^,]", serviceID);
    int start = strlen(serviceID) + 1;
    for(int i = start; i < ELen; ++i)
        temp[i - start] = bufE[i];
    temp[ELen - start] = 0;
    printf("service ID is %s\n", serviceID);

    //解密得到K_client_SS
    ST = DES_decrypt(temp, K_SS);
    printf("ST is %s\n", ST);
    sscanf(ST, "<%[^,], %[^,], %ld, %[^>]>", clientID1, client_address, &validity, K_client_SS);
    printf("K_client_SS is %s\n", K_client_SS);

    //接收消息G并解密得到认证内容G
    printf("accepting message G...\n");
    int GLen = recv(client_socket, bufG, MAX_DATA, 0);
    bufG[GLen] = 0;
    authenticationG = DES_decrypt(bufG, K_client_SS);
    sscanf(authenticationG, "<%[^,], %ld>", clientID2, &timestamp);

    //对比ST和认证G中的clientID来进行认证
    if(strcmp(clientID1, clientID2) != 0)
    {
        printf("authentication failed! ClientID is not correct\n");
        return;
    }
    printf("authentication secceed!\n");

    //生成并发送消息H
    sprintf(temp, "<%s,%ld>", clientID1, timestamp + 1);
    H = DES_encrypt(temp, K_client_SS);
    printf("sending message H...\n");
    send(client_socket, H, strlen(H), 0);

    printf("SS server has finished!\n");
}

int main()
{
    int SS_socket, client_socket;     //SS用于监听的套接字和建立连接后的套接字
    struct sockaddr_in SS_addr;       //SS的地址信息结构体，下面有具体的属性赋值
    struct sockaddr_in client_addr;   //客户端地址信息
    int addr_len = sizeof(struct sockaddr_in);


    SS_socket = socket(AF_INET, SOCK_STREAM, 0);     //建立socket
    if (SS_socket == -1)
    {
        perror("SS socket");
        return -1;
    }
    else
    {
        printf("SS socket number = %d\n", SS_socket);
    }
    SS_addr.sin_family = AF_INET;                   //用于网络通信
    SS_addr.sin_port = htons(SS_PORT);              //端口号
    SS_addr.sin_addr.s_addr = htonl(INADDR_ANY);    //IP，括号内容表示本机IP
    bzero(&(SS_addr.sin_zero), 8);                  //将sin_zero字段置0
    if (bind(SS_socket, (struct sockaddr *)&SS_addr, sizeof(struct sockaddr)) < 0) //绑定地址结构体和socket
    {
        perror("bind error");
        return -1;
    }
    else
    {
        printf("bind succeed\n");
    }
    listen(SS_socket, BACKLOG);     //开启监听 ，第二个参数是最大监听数
    printf("SS server is listening at %d port\n", SS_PORT);

    while (1)
    {
        //阻塞直到有客户端连接
        client_socket = accept(SS_socket, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
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
    close(SS_socket);
    return 0;
}