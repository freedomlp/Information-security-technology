#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include "DES.c"

#define TGS_PORT 1600         //端口号
#define BACKLOG 100           //最大监听数
#define MAX_DATA 200          //缓冲区大小
#define time_limit 10000000   //有效期

char K_TGS[] = "CCCCCCCCCCCCCCCC";
char K_client_SS[] = "DDDDDDDDDDDDDDDD";
char K_SS[] = "EEEEEEEEEEEEEEEE";

void serve(struct sockaddr_in * client_addr, int client_socket)
{
    char bufC[MAX_DATA];               //接收client发送的消息C的缓冲区
    char bufD[MAX_DATA];               //接收client发送的消息D的缓冲区
    char serviceID[MAX_DATA];          //客户端申请的服务ID
    char B[MAX_DATA];                  //消息C中包含的消息B
    char clientID1[MAX_DATA];          //消息C中包含的消息B中得到的clientID
    char clientID2[MAX_DATA];          //消息D中解压得到的clientID
    char client_address[MAX_DATA];     //消息C中包含的消息B中得到的客户端地址
    time_t validity;                   //消息C中包含的消息B中得到的TGT票据有效期
    time_t timestamp;                  //申请认证时客户端的时间戳
    char K_client_TGS[MAX_DATA];       //Client-TGS会话密钥 
    char E[MAX_DATA];                  //发送给client的消息E
    char *F;                           //发送给client的消息F

    printf("Client ip is %s, client port is %d\n", inet_ntoa(client_addr->sin_addr), htons(client_addr->sin_port));
    printf("accepting message C...\n");
    int CLen = recv(client_socket, bufC, MAX_DATA, 0);
    bufC[CLen] = 0;
    printf("accepting message D...\n");
    int DLen = recv(client_socket, bufD, MAX_DATA, 0);
    bufD[DLen] = 0;

    //得到serviceID和消息B
    sscanf(bufC, "%[^,]", serviceID);          //从bufC中取出“,”前的字符串
    printf("service ID is %s\n", serviceID);
    int start = strlen(serviceID) + 1;
    for(int i = start; i < CLen; ++i)
        B[i - start] = bufC[i];
    B[CLen - start] = 0;

    printf("decrypting message B...\n");
    char * TGT = DES_decrypt(B, K_TGS);
    sscanf(TGT, "<%[^,], %[^,], %ld, %[^>]>", clientID1, client_address, &validity, K_client_TGS);
    printf("K_client_TGS is %s\n", K_client_TGS);

    printf("decrypting message D...\n");
    char * authentication = DES_decrypt(bufD, K_client_TGS);
    sscanf(authentication, "<%[^,], %ld>", clientID2, &timestamp);
    printf("message D: <%s, %ld>\n", clientID2, timestamp);

    //对比消息B和消息D中的clientID来进行认证
    if(strcmp(clientID1, clientID2) != 0)
    {
        printf("authentication failed! ClientID is not correct\n");
        send(client_socket, "invalid ID", strlen("invalid ID"), 0);
        return;
    }
    if(timestamp > validity)
    {
        printf("authentication failed! TGT expired\n");
        send(client_socket, "invalid TGT", strlen("invalid TGT"), 0);
        return;
    }
    printf("authentication secceed!\n");

    //生成并发送消息E
    printf("sending message E...\n");
    char temp[MAX_DATA];
    sprintf(temp, "<%s,%s,%ld,%s>", clientID1, client_address, time(NULL) + time_limit, K_client_SS);
    printf("ST is %s\n", temp);
    char *ST = DES_encrypt(temp, K_SS);       //服务票据，包括clientID、客户网络地址、票据有效期限、Client-SS会话密钥
    sprintf(E, "<%s,%s>", serviceID, ST);
    send(client_socket, E, strlen(E), 0);

    sleep(1);
    //生成并发送消息F
    printf("sending message F...\n");
    F = DES_encrypt(K_client_SS, K_client_TGS);
    send(client_socket, F, strlen(F), 0);

    printf("TGS server has finished!\n");
}

int main()
{
    int TGS_socket, client_socket;     //TGS用于监听的套接字和建立连接后的套接字
    struct sockaddr_in TGS_addr;       //TGS的地址信息结构体，下面有具体的属性赋值
    struct sockaddr_in client_addr;    //客户端地址信息
    int addr_len = sizeof(struct sockaddr_in);


    TGS_socket = socket(AF_INET, SOCK_STREAM, 0);     //建立socket
    if (TGS_socket == -1)
    {
        perror("TGS socket");
        return -1;
    }
    else
    {
        printf("TGS socket number = %d\n", TGS_socket);
    }
    TGS_addr.sin_family = AF_INET;                    //用于网络通信
    TGS_addr.sin_port = htons(TGS_PORT);              //端口号
    TGS_addr.sin_addr.s_addr = htonl(INADDR_ANY);     //IP，括号内容表示本机IP
    bzero(&(TGS_addr.sin_zero), 8);                   //将sin_zero字段置0
    if (bind(TGS_socket, (struct sockaddr *)&TGS_addr, sizeof(struct sockaddr)) < 0) //绑定地址结构体和socket
    {
        perror("bind error");
        return -1;
    }
    else
    {
        printf("bind succeed\n");
    }
    listen(TGS_socket, BACKLOG);     //开启监听 ，第二个参数是最大监听数
    printf("TGS server is listening at %d port\n", TGS_PORT);

    while (1)
    {
        //阻塞直到有客户端连接
        client_socket = accept(TGS_socket, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
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
    close(TGS_socket);
    return 0;
}