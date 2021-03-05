#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const int IP[64] =
    {
        58, 50, 42, 34, 26, 18, 10, 2,
        60, 52, 44, 36, 28, 20, 12, 4,
        62, 54, 46, 38, 30, 22, 14, 6,
        64, 56, 48, 40, 32, 24, 16, 8,
        57, 49, 41, 33, 25, 17, 9, 1,
        59, 51, 43, 35, 27, 19, 11, 3,
        61, 53, 45, 37, 29, 21, 13, 5,
        63, 55, 47, 39, 31, 23, 15, 7}; //初始置换IP

const int IP_1[64] =
    {
        40, 8, 48, 16, 56, 24, 64, 32,
        39, 7, 47, 15, 55, 23, 63, 31,
        38, 6, 46, 14, 54, 22, 62, 30,
        37, 5, 45, 13, 53, 21, 61, 29,
        36, 4, 44, 12, 52, 20, 60, 28,
        35, 3, 43, 11, 51, 19, 59, 27,
        34, 2, 42, 10, 50, 18, 58, 26,
        33, 1, 41, 9, 49, 17, 57, 25}; //逆置换IP_1

const int E[48] =
    {
        32, 1, 2, 3, 4, 5,
        4, 5, 6, 7, 8, 9,
        8, 9, 10, 11, 12, 13,
        12, 13, 14, 15, 16, 17,
        16, 17, 18, 19, 20, 21,
        20, 21, 22, 23, 24, 25,
        24, 25, 26, 27, 28, 29,
        28, 29, 30, 31, 32, 1

}; //E扩展

const int PC_1[56] =
    {
        57, 49, 41, 33, 25, 17, 9,
        1, 58, 50, 42, 34, 26, 18,
        10, 2, 59, 51, 43, 35, 27,
        19, 11, 3, 60, 52, 44, 36,
        63, 55, 47, 39, 31, 23, 15,
        7, 62, 54, 46, 38, 30, 22,
        14, 6, 61, 53, 45, 37, 29,
        21, 13, 5, 28, 20, 12, 4}; //PC_1,生成子密钥

const int PC_2[] =
    {
        14, 17, 11, 24, 1, 5,
        3, 28, 15, 6, 21, 10,
        23, 19, 12, 4, 26, 8,
        16, 7, 27, 20, 13, 2,
        41, 52, 31, 37, 47, 55,
        30, 40, 51, 45, 33, 48,
        44, 49, 39, 56, 34, 53,
        46, 42, 50, 36, 29, 32}; //PC_2,生成子密钥

const int S[8][4][16] =
    {
        //s1
        14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
        0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
        4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
        15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13,

        //s2
        15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
        3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
        0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
        13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9,

        //s3
        10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
        13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
        13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
        1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12,

        //s4
        7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
        13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
        10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
        3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14,

        //s5
        2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
        14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
        4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
        11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3,

        //s6
        12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
        10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
        9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
        4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13,

        //s7
        4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
        13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
        1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
        6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12,

        //s8
        13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
        1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
        7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
        2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}; //S-盒是一类选择函数，用于二进制6-4转换。Feistel轮函数使用8个S-盒S1, …, S8。

const int P[] =
    {
        16, 7, 20, 21,
        29, 12, 28, 17,
        1, 15, 23, 26,
        5, 18, 31, 10,
        2, 8, 24, 14,
        32, 27, 3, 9,
        19, 13, 30, 6,
        22, 11, 4, 25}; //P置换

void IP_displace(int block[64], int left[32], int right[32])
{
    int temp[64];
    for (int i = 0; i < 64; ++i)
        temp[i] = block[IP[i] - 1];
    for (int i = 0; i < 32; ++i)
    {
        left[i] = temp[i];
        right[i] = temp[i + 32];
    }
}

void IP_1_displace(int before[], int after[])
{
    for (int i = 0; i < 64; ++i)
        after[i] = before[IP_1[i] - 1];
}

void PC_1_displace(int key[], int C[], int D[])
{
    int temp[56];
    for (int i = 0; i < 56; ++i)
        temp[i] = key[PC_1[i] - 1];
    for (int i = 0; i < 28; ++i)
    {
        C[i] = temp[i];
        D[i] = temp[i + 28];
    }
}

void PC_2_displace(int CD[], int sub_key[])
{
    for (int i = 0; i < 48; ++i)
        sub_key[i] = CD[PC_2[i] - 1];
}

void P_displace(int before_p[], int after_p[])
{
    for (int i = 0; i < 32; ++i)
        after_p[i] = before_p[P[i] - 1];
}

void bin_to_hex(int bin[][64], char hex[], int block_num) //2进制转换16进制
{
    int count = 0;
    for (int i = 0; i < block_num; ++i)
    {
        for (int j = 0; j < 64; ++j)
        {
            if ((j + 1) % 4 == 0)
            {
                int temp = bin[i][j] + bin[i][j - 1] * 2 + bin[i][j - 2] * 4 + bin[i][j - 3] * 8;
                if (temp > 9)
                    hex[count] = temp - 10 + 'A';
                else
                    hex[count] = temp + '0';
                ++count;
            }
        }
    }
    hex[count] = '\0';
}

void hex_to_bin(char *hex, int bin[]) //16进制转换2进制
{
    int length = strlen(hex);
    for (int i = 0; i < length; ++i)
    {
        int temp;
        if (hex[i] > 'A' || hex[i] == 'A')
            temp = hex[i] - 'A' + 10;
        else
            temp = hex[i] - '0';
        for (int j = 0; j < 4; ++j)
        {
            bin[(i + 1) * 4 - 1 - j] = temp % 2;
            temp = temp / 2;
        }
    }
}

void bin_to_dec(int block[][64], int a[], int block_num) //2进制转换10进制
{
    int count = 0;
    for (int i = 0; i < block_num; ++i)
    {
        for (int j = 0; j < 64; j = j + 8)
        {
            a[count] = block[i][j] * 128 + block[i][j + 1] * 64 + block[i][j + 2] * 32 + block[i][j + 3] * 16 + block[i][j + 4] * 8 + block[i][j + 5] * 4 + block[i][j + 6] * 2 + block[i][j + 7] * 1;
            ++count;
        }
    }
}

void xor_(int t1[], int t2[], int res[], int mode) //需要用到32位异或和48位异或
{
    for (int i = 0; i < mode; ++i)
        res[i] = t1[i] ^ t2[i];
}

void fill(int binary[], int start, int remainder) //按照PKCS#5 (RFC 8018）规范进行字节填充
{
    switch (remainder)
    {
    case 0:
        for (int i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 8; ++j)
                binary[start + (8 * (i + 1)) - 1 - j] = (8 >> j) & 1;
        }
        break;
    case 1:
        for (int i = 0; i < 7; ++i)
        {
            for (int j = 0; j < 8; ++j)
                binary[start + (8 * (i + 1)) - 1 - j] = (7 >> j) & 1;
        }
        break;
    case 2:
        for (int i = 0; i < 6; ++i)
        {
            for (int j = 0; j < 8; ++j)
                binary[start + (8 * (i + 1)) - 1 - j] = (6 >> j) & 1;
        }
        break;
    case 3:
        for (int i = 0; i < 5; ++i)
        {
            for (int j = 0; j < 8; ++j)
                binary[start + (8 * (i + 1)) - 1 - j] = (5 >> j) & 1;
        }
        break;
    case 4:
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 8; ++j)
                binary[start + (8 * (i + 1)) - 1 - j] = (4 >> j) & 1;
        }
        break;
    case 5:
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 8; ++j)
                binary[start + (8 * (i + 1)) - 1 - j] = (3 >> j) & 1;
        }
        break;
    case 6:
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 8; ++j)
                binary[start + (8 * (i + 1)) - 1 - j] = (2 >> j) & 1;
        }
        break;
    case 7:
        for (int i = 0; i < 1; ++i)
        {
            for (int j = 0; j < 8; ++j)
                binary[start + (8 * (i + 1)) - 1 - j] = (1 >> j) & 1;
        }
        break;
    }
}

void group(int block[][64], char *s, int block_num, char mode) //分组函数,将明文分成8个字节一组
{
    int remainder; //判断需要填充的字节个数
    int str_len = strlen(s);
    int binary[block_num * 64]; //先将字符串转换成二进制并将其填充，再对其分组
    if (mode == 'E')            //加密模式下对char型的字符作二进制转换
    {
        remainder = str_len % 8;
        for (int i = 0; i < str_len; ++i)
        {
            for (int j = 0; j < 8; ++j)
                binary[(8 * (i + 1)) - 1 - j] = (s[i] >> j) & 1;
        }
        fill(binary, str_len * 8, remainder);
    }
    else                       //解密模式下对16进制的字符作二进制转换
        hex_to_bin(s, binary); //无需填充
    for (int i = 0; i < block_num; ++i)
    {
        for (int j = 0; j < 64; ++j)
            block[i][j] = binary[i * 64 + j];
    }
}

void LS(int A[], int flag)
{
    if ((flag == 1) || (flag == 2) || (flag == 9) || (flag == 16))
    {
        int temp = A[0];
        for (int i = 0; i < 27; ++i)
            A[i] = A[i + 1];
        A[27] = temp;
    }
    else
    {
        int temp0 = A[0];
        int temp1 = A[1];
        for (int i = 0; i < 26; ++i)
            A[i] = A[i + 2];
        A[26] = temp0;
        A[27] = temp1;
    }
}

void S_boxing(int bit_48[], int bit_32[]) //S盒函数
{
    int bit_6[8][6];
    int bit_4[8][4];
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 6; ++j)
            bit_6[i][j] = bit_48[i * 6 + j];
        int n = 2 * bit_6[i][0] + bit_6[i][5];
        int m = 8 * bit_6[i][1] + 4 * bit_6[i][2] + 2 * bit_6[i][3] + bit_6[i][4];
        int temp = S[i][n][m];
        for (int j = 0; j < 4; ++j)
            bit_4[i][3 - j] = (temp >> j) & 1;
    }
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 4; ++j)
            bit_32[i * 4 + j] = bit_4[i][j];
    }
}

void get_sub_key(int key[], int sub_key[16][48]) //将密钥分成16个子密钥
{
    int C[28], D[28];
    PC_1_displace(key, C, D);
    for (int i = 0; i < 16; ++i)
    {
        LS(C, i + 1);
        LS(D, i + 1);
        int CD[56];
        for (int i = 0; i < 28; ++i)
        {
            CD[i] = C[i];
            CD[i + 28] = D[i];
        }
        PC_2_displace(CD, sub_key[i]);
    }
}

void Feistel(int right[], int sub_key[], int res[])
{
    int E_R[48];
    for (int i = 0; i < 48; ++i)
        E_R[i] = right[E[i] - 1];
    xor_(E_R, sub_key, E_R, 48);
    int after_s[32];
    S_boxing(E_R, after_s);
    P_displace(after_s, res);
}

void T_iteration(int left[32], int right[32], int sub_key[16][48]) //T迭代
{
    int temp_right[32];
    int temp_left[32];
    for (int i = 0; i < 16; ++i)
    {
        for (int i = 0; i < 32; ++i)
        {
            temp_right[i] = right[i];
            temp_left[i] = left[i];
            left[i] = right[i];
        }
        int after_f[32];
        Feistel(right, sub_key[i], after_f);
        xor_(temp_left, after_f, right, 32);
    }
}

void encryption_or_decryption(char *input, int output[][64], int sub_key[16][48], int block_num, int mode) //加密和解密用同一个函数实现
{
    int block[block_num][64];
    group(block, input, block_num, mode);
    for (int i = 0; i < block_num; ++i)
    {
        int left[32], right[32];
        IP_displace(block[i], left, right);
        T_iteration(left, right, sub_key);
        int temp[64];
        for (int j = 0; j < 32; ++j) //交换置换W
        {
            temp[j] = right[j];
            temp[j + 32] = left[j];
        }
        IP_1_displace(temp, output[i]);
    }
}

char *DES_encrypt(char *input, char *key_16)
{
    int key[64], sub_key[16][48], block_num;
    block_num = (strlen(input) / 8) + 1;
    int res[block_num][64];
    char * output = (char *)malloc(sizeof(char) * (block_num * 16 + 1));
    char mode = 'E';

    hex_to_bin(key_16, key); //将16进制key转换为二进制
    get_sub_key(key, sub_key);
    encryption_or_decryption(input, res, sub_key, block_num, mode);
    bin_to_hex(res, output, block_num);
    return output;
}

char *DES_decrypt(char *input, char *key_16)
{
    int key[64], sub_key[16][48], reverse_sub_key[16][48], block_num;
    block_num = (strlen(input) / 16);
    int res[block_num][64];
    int plaintext[block_num * 8];
    char mode = 'D';

    hex_to_bin(key_16, key);
    get_sub_key(key, sub_key);
    for (int i = 0; i < 16; ++i)
    {
        for (int j = 0; j < 48; ++j)
            reverse_sub_key[i][j] = sub_key[15 - i][j];
    }

    encryption_or_decryption(input, res, reverse_sub_key, block_num, mode);
    bin_to_dec(res, plaintext, block_num);
    int fill = res[block_num - 1][60] * 8 + res[block_num - 1][61] * 4 + res[block_num - 1][62] * 2 + res[block_num - 1][63];
    char * output = (char *)malloc(sizeof(char) * ((block_num * 8) - fill + 1));
    for(int i = 0; i < (block_num * 8) - fill; ++i)
        output[i] = plaintext[i];
    output[(block_num * 8) - fill] = 0;
    return output;
}

/*
int main()
{
    char K_client[] = "I love you!";
    char K_client_TGS[] = "4142434445464748";
    char *A = DES_encrypt(K_client, K_client_TGS);
    printf("%s\n", A);
    
    char *B = DES_decrypt(A, K_client_TGS);
    printf("%s", B);
}*/