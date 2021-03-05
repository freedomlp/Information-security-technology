#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gmp.h>

int k;//N的长度

void get_N_p_q(mpz_t N, mpz_t p, mpz_t q)
{
    gmp_randstate_t grt;
    gmp_randinit_default(grt); ////设置随机数生成算法为默认
    gmp_randseed_ui(grt, time(NULL));

    mpz_urandomb(p, grt, 1024); //随机生成一个0-2^1024的一个数
    mpz_urandomb(q, grt, 1024);

    mpz_nextprime(p, p);
    mpz_nextprime(q, q);
    mpz_mul(N, p, q);
}

void getPS(unsigned char PS[], int lenPS)
{
    for (int i = 0; i < lenPS; ++i)
    {
        int a = rand() % 255 + 1;
        PS[i] = a;
    }
}

void padding(unsigned char M[], unsigned char EM[], int lenPS)
{
    EM[0] = 0x00;
    EM[1] = 0x02;
    unsigned char PS[lenPS];
    getPS(PS, lenPS);
    for (int i = 0; i < lenPS; ++i)
    {
        EM[i + 2] = PS[i];
    }
    EM[2 + lenPS] = 0x00;
    for (int i = 0; i < strlen(M); ++i)
    {
        EM[i + 3 + lenPS] = M[i];
    }
}

void OS2IP(unsigned char octet[], mpz_t intm, int length)
{
    mpz_t a;
    mpz_init(a);
    mpz_set_str(a, "256", 10);
    for (int i = 0; i < length; ++i)
    {
        mpz_mul_ui(intm, intm, 256);
        mpz_add_ui(intm, intm, octet[i]);
    }
}

void I2OSP(mpz_t intm, unsigned char octet[], int length)
{
    mpz_t temp, src;
    mpz_init(src);
    mpz_set(src, intm);
    for (int i = length - 1; i >= 0; --i)
    {
        mpz_init(temp);
        mpz_mod_ui(temp, src, 256);
        octet[i] = mpz_get_ui(temp);
        mpz_div_ui(src, src, 256);
    }
}

void encrypt(mpz_t N, mpz_t e, unsigned char *M, unsigned char *ciphertext)
{
    int mlen = strlen(M);
    if (mlen > k - 11)
        printf("message too long\n");
    else
    {
        unsigned char EM[k];
        padding(M, EM, k - mlen - 3);
        mpz_t intm;
        mpz_init(intm);
        OS2IP(EM, intm, k);
        mpz_powm(intm, intm, e, N);
        int size = mpz_sizeinbase(intm, 2);
        int length = size / 8 + (size % 8 ? 1 : 0);
        if (length != k)
            printf("Error");
        else
            I2OSP(intm, ciphertext, length);
    }
}

void decrypt(mpz_t N, mpz_t d, unsigned char *C, unsigned char *EM)
{
    mpz_t intc;
    mpz_init(intc);
    OS2IP(C, intc, k);
    mpz_powm(intc, intc, d, N);
    I2OSP(intc, EM, k);
}

int main()
{
    mpz_t N, fiN, p, q, e, d;
    mpz_init(N);
    mpz_init(fiN);
    mpz_init(p);
    mpz_init(q);
    mpz_init(e);
    mpz_init(d);
    get_N_p_q(N, p, q);
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(fiN, p, q);
    mpz_set_str(e, "65537", 10);//e一般取65537
    mpz_invert(d, e, fiN);
    int size_k = mpz_sizeinbase(N, 2);
    k = size_k / 8 + (size_k % 8 ? 1 : 0);
    unsigned char m[] = "I love you! You are my sunshine!";
    unsigned char ciphertext[k];
    unsigned char EM[k]; 
    unsigned char * plaintext;
    encrypt(N, e, m, ciphertext);
    decrypt(N, d, ciphertext, EM);
    int start;
    for(start = 2; start < k; ++start)
    {
        if(EM[start] == 0x00)
            break;
    }
    int length = k - start - 1;
    plaintext = (char *)malloc(sizeof(char) * length + 1);
    for(int i = 0; i < length; ++i)
    {
        plaintext[i] = EM[start + 1 + i];
    }
    plaintext[length] = '\0';
    printf("%s\n", plaintext);
}