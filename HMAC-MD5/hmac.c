#include <stdio.h>
#include <string.h>
#include "MD5.c"

#define BLOCKSIZE 64  // b = 64 * 8 = 512位
#define ipad 0x36     // 00110110
#define opad 0x5c     // 01011100

char *key;    // secrete key, |k| <= b
char *Kplus;  //对共享密钥 k 左边补0，生成一个 b 位的数据块 K+
char *Si;     // K+ ^ ipad
char *So;     // K+ ^ opad

void getKplus()
{
    Kplus = (char *)malloc(sizeof(char) * BLOCKSIZE);
    for(int i = 0; i < strlen(key); ++i)
        Kplus[i] = key[i];
    for(int i = strlen(key); i < BLOCKSIZE; ++i)
        Kplus[i] = 0;
}

void getSi_So()
{
    Si = (char *)malloc(sizeof(char) * BLOCKSIZE);
    for(int i = 0; i < BLOCKSIZE; ++i)
        Si[i] = Kplus[i] ^ ipad;
    So = (char *)malloc(sizeof(char) * BLOCKSIZE);
    for(int i = 0; i < BLOCKSIZE; ++i)
        So[i] = Kplus[i] ^ opad;
}

// 字符串拼接函数
void add_str(char * str1, int len1, char * str2, int len2, char * res)
{
    for(int i = 0; i < len1; ++i)
        res[i] = str1[i];
    for(int i = 0; i < len2; ++i)
        res[len1 + i] = str2[i];
}

void hmac(unsigned char * M, unsigned long long Mlen, unsigned char * key, unsigned char * HMAC)
{
    getKplus();
    getSi_So();
    char SM[BLOCKSIZE + Mlen];
    add_str(Si, BLOCKSIZE, M, Mlen, SM);
    unsigned char hash1[16];
    HMD5(SM, hash1, BLOCKSIZE + Mlen);
    char SH[BLOCKSIZE + 16];
    add_str(So, BLOCKSIZE, hash1, 16, SH);
    HMD5(SH, HMAC, BLOCKSIZE + 16);
}

// 将利用malloc函数动态分配的内存空间统一释放
void freeall()
{
    free(paddedMessage);
    free(Kplus);
    free(Si);
    free(So);
}

int main()
{
    char *s = "I love you, you are my sunshine!";
    key = "abcde";
    unsigned char output[16];
    hmac(s, 32, key, output);
    for(int i = 0; i < 16; ++i)
        printf("%02x", output[i]);
    freeall();
}