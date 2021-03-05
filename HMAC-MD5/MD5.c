#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 四个轮函数
#define F(b, c, d) ((b & c) | (~b & d))
#define G(b, c, d) ((b & d) | (c & ~d))
#define H(b, c, d) (b ^ c ^ d)
#define I(b, c, d) (c ^ (b | ~d))

/*
循环左移，示例如下：
    10101010 10101010 10101010 10101010
    左移3位：
    01010 10101010 10101010 10101010 000
                                     010
    按位或即为正确结果
*/
#define CLS(x, s) ((x << s) | (x >> (32 -s))) //循环左移


// MD缓冲区的初始向量 IV(A, B, C, D)
const unsigned int IV[4] = {
    0x67452301,
    0xEFCDAB89,
    0x98BADCFE,
    0x10325476};

// 各轮循环中第 i 次迭代 (i = 1 .. 16) 使用的 X[k]
const int X_index[4][16] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12},
    {5, 8, 11, 14, 1, 4, 7, 10, 13, 0, 3, 6, 9, 12, 15, 2},
    {0, 7, 14, 5, 12, 3, 10, 1, 8, 15, 6, 13, 4, 11, 2, 9}};

// 各轮各次迭代运算采用的 T 值
const unsigned int T[4][16] = {
    {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
     0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821},

    {0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
     0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a},

    {0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
     0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665},

    {0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
     0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391}};

// 各轮各次迭代运算 (1 .. 64) 采用的左循环移位的位数 s 值
const int S[4][16] = {
    {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22},
    {5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20},
    {4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23},
    {6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21}};

int blockLen;                 // 512位长的分组数
unsigned int **paddedMessage; // 存放填充后message的32位无符号整形变量二维数组

// 取4个字节，按 little-endian 转移到1个32位无符号整型变量
void Byte2int(unsigned char *Byte)
{
    for (int i = 0; i < blockLen; ++i)
    {
        for (int j = 0; j < 16; ++j)
        {
            paddedMessage[i][j] = (Byte[i * 64 + j * 4]) |
                                  (Byte[i * 64 + j * 4 + 1] << 8) |
                                  (Byte[i * 64 + j * 4 + 2] << 16) |
                                  (Byte[i * 64 + j * 4 + 3] << 24);
        }
    }
}

// 将32位无符号整型转化成4个字符
void int2char(unsigned int *src, unsigned char *dst, long length)
{
    for(int i = 0; i < length; ++i)
    {
        dst[i * 4 + 3] = (src[i] >> 24) & 0x000000ff;
        dst[i * 4 + 2] = (src[i] >> 16) & 0x000000ff;
        dst[i * 4 + 1] = (src[i] >> 8) & 0x000000ff;
        dst[i * 4] = src[i] & 0x000000ff;
    }
}

// 填充函数
void padding(char *message, unsigned long long messageLen)
{
    blockLen = (messageLen / 64) + ((messageLen * 8) % 512 >= 448 ? 2 : 1);
    unsigned char temp[blockLen * 64];
    for (int i = 0; i < messageLen; ++i)
        temp[i] = message[i];
    temp[messageLen] = 0x80; // 0x80 -> 1000 0000
    for (int i = messageLen + 1; i < blockLen * 64; ++i)
        temp[i] = 0x00;

    // 动态分配二维数组的内存空间
    paddedMessage = (unsigned int **)malloc(blockLen * sizeof(unsigned int *));
    for (int i = 0; i < blockLen; ++i)
        paddedMessage[i] = (unsigned int *)malloc(16 * sizeof(unsigned int));

    Byte2int(temp);
    unsigned int left = ((messageLen * 8) >> 32) & 0x00000000ffffffff;
    unsigned int right = (messageLen * 8) & 0x00000000ffffffff;
    paddedMessage[blockLen - 1][15] = left;
    paddedMessage[blockLen - 1][14] = right;
}

// 循环置换：ABCD -> DABC
void buffer_iteration(unsigned int MD[4])
{
    unsigned int temp = MD[0];
    MD[0] = MD[3];
    MD[3] = MD[2];
    MD[2] = MD[1];
    MD[1] = temp;
}

// MD5压缩函数 a = b + ((a + g(b, c, d) + X[k] + T[i]) <<< s)
void HMD5(char *message, char * output, unsigned long long messageLen)
{
    padding(message, messageLen);
    unsigned int CV[4];
    //初始化CV
    for (int i = 0; i < 4; ++i)
        CV[i] = IV[i]; 

    for (int i = 0; i < blockLen; ++i)
    {
        // 首先记录下64轮迭代前的CV值
        unsigned int temp[4];
        for(int j = 0; j < 4; ++j)
            temp[j] = CV[j];

        // 第一轮循环F
        for (int j = 0; j < 16; ++j)
        {
            CV[0] = CV[1] + CLS((CV[0] + F(CV[1], CV[2], CV[3]) + paddedMessage[i][X_index[0][j]] + T[0][j]), S[0][j]);
            buffer_iteration(CV);
        }
        // 第二轮循环G
        for (int j = 0; j < 16; ++j)
        {
            CV[0] = CV[1] + CLS((CV[0] + G(CV[1], CV[2], CV[3]) + paddedMessage[i][X_index[1][j]] + T[1][j]), S[1][j]);
            buffer_iteration(CV);
        }
        // 第三轮循环H
        for (int j = 0; j < 16; ++j)
        {
            CV[0] = CV[1] + CLS((CV[0] + H(CV[1], CV[2], CV[3]) + paddedMessage[i][X_index[2][j]] + T[2][j]), S[2][j]);
            buffer_iteration(CV);
        }
        // 第四轮循环I
        for (int j = 0; j < 16; ++j)
        {
            CV[0] = CV[1] + CLS((CV[0] + I(CV[1], CV[2], CV[3]) + paddedMessage[i][X_index[3][j]] + T[3][j]), S[3][j]);
            buffer_iteration(CV);
        }

        for(int j = 0; j < 4; ++j)
            CV[j] += temp[j]; 
    }
    
    int2char(CV, output, 4);
}

/*
int main()
{
    char *s = "I love you, you are my sunshine!";
    unsigned char output[16];
    HMD5(s, output, 32);
    printf("%d\n", blockLen);
    for(int i = 0; i < 16; ++i)
        printf("%02x", output[i]);
    free(paddedMessage);
}*/