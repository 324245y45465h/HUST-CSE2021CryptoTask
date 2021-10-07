#include <cstdio>
#include <cstdlib>
#include <cstring>
#define MAX_LINK_LEN 10000
#define MAX_STR_LEN 9

int n;
char headStr[MAX_LINK_LEN][MAX_STR_LEN], tailStr[MAX_LINK_LEN][MAX_STR_LEN],
    res[MAX_STR_LEN];
unsigned int sha1[5], tmp[5];
unsigned int SHA1_tmp;

#define SHA1_ROTL(a, b) \
    (SHA1_tmp = (a),    \
     ((SHA1_tmp >> (32 - b)) & (0x7fffffff >> (31 - b))) | (SHA1_tmp << b))

#define SHA1_F(B, C, D, t)                                        \
    ((t < 40) ? ((t < 20) ? ((B & C) | ((~B) & D)) : (B ^ C ^ D)) \
              : ((t < 60) ? ((B & C) | (B & D) | (C & D)) : (B ^ C ^ D)))

int UnitSHA1(const char* str, int length, unsigned sha1[5]) {
    /*
    计算字符串SHA-1
    参数说明：
    str         字符串指针
    length      字符串长度
    sha1        用于保存SHA-1的字符串指针
    返回值为参数sha1
    */
    unsigned char *pp, *ppend;
    unsigned int l, i, K[80], W[80], TEMP, A, B, C, D, E, H0, H1, H2, H3, H4;
    H0 = 0x67452301, H1 = 0xEFCDAB89, H2 = 0x98BADCFE, H3 = 0x10325476,
    H4 = 0xC3D2E1F0;
    for (i = 0; i < 20; K[i++] = 0x5A827999)
        ;
    for (i = 20; i < 40; K[i++] = 0x6ED9EBA1)
        ;
    for (i = 40; i < 60; K[i++] = 0x8F1BBCDC)
        ;
    for (i = 60; i < 80; K[i++] = 0xCA62C1D6)
        ;
    l = length +
        ((length % 64 > 56) ? (128 - length % 64) : (64 - length % 64));
    if (!(pp = (unsigned char*)malloc((unsigned int)l))) return -1;
    for (i = 0; i < length; pp[i + 3 - 2 * (i % 4)] = str[i], i++)
        ;
    for (pp[i + 3 - 2 * (i % 4)] = 128, i++; i < l;
         pp[i + 3 - 2 * (i % 4)] = 0, i++)
        ;
    *((unsigned int*)(pp + l - 4)) = length << 3;
    *((unsigned int*)(pp + l - 8)) = length >> 29;
    for (ppend = pp + l; pp < ppend; pp += 64) {
        for (i = 0; i < 16; W[i] = ((unsigned int*)pp)[i], i++)
            ;
        for (i = 16; i < 80;
             W[i] = SHA1_ROTL((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]), 1),
            i++)
            ;
        A = H0, B = H1, C = H2, D = H3, E = H4;
        for (i = 0; i < 80; i++) {
            TEMP = SHA1_ROTL(A, 5) + SHA1_F(B, C, D, i) + E + W[i] + K[i];
            E = D, D = C, C = SHA1_ROTL(B, 30), B = A, A = TEMP;
        }
        H0 += A, H1 += B, H2 += C, H3 += D, H4 += E;
    }
    free(pp - l);
    sha1[0] = H0, sha1[1] = H1, sha1[2] = H2, sha1[3] = H3, sha1[4] = H4;
    return 0;
}

void getstr(unsigned n, char str[8]) {
    str[0] = 'a';
    str[1] = '0';
    str[2] = '0';
    str[3] = '0';
    str[4] = '0';
    str[5] = '0';
    str[6] = '0';
    str[7] = '0';

    int i = 2;

    while (n) {
        unsigned tmp = n % 36;

        if (tmp < 10)
            str[i++] = tmp + '0';
        else
            str[i++] = tmp - 10 + 'a';
        n = n / 36;
    }
}

void R(unsigned sha1[5], char str[8], int i) {
    getstr((sha1[0] + sha1[1] * i) % 2176782336, str);
}

bool findStr(char res[MAX_STR_LEN], unsigned int sha1[5]) {
    char strTmp[MAX_STR_LEN];
    unsigned int sha1Tmp[5];
    bool flg;
    for (int i = 0; i < n; ++i) {
        if (strcmp(res, tailStr[i]) == 0) {
            strcpy(strTmp, headStr[i]);
            for (int j = 0; j < MAX_LINK_LEN; ++j) {
                UnitSHA1(strTmp, 8, sha1Tmp);
                flg = true;
                for (int k = 0; k < 5; ++k) {
                    if (sha1Tmp[k] != sha1[k]) {
                        flg = false;
                        break;
                    }
                }
                if (flg) {
                    strcpy(res, strTmp);
                    return true;
                }
                R(sha1Tmp, strTmp, j % 100 + 1);
            }
        }
    }
    return false;
}

int main() {
    // freopen("test.in", "r", stdin);
    bool flg = false;
    scanf("%d", &n);
    for (int i = 0; i < n; ++i) scanf("%s%s", headStr + i, tailStr + i);
    for (int i = 0; i < 5; ++i) scanf("%8x", sha1 + i);

    for (int i = 0; i <= 100; ++i) {
        for (int j = 0; j < 5; ++j) tmp[j] = sha1[j];
        for (int j = i - 1; j < 9200; ++j) {
            R(tmp, res, j % 100 + 1);
            if (j % 100 == 99) {
                flg = findStr(res, sha1);
                if (flg) break;
            }
            UnitSHA1(res, 8, tmp);
        }
        if (flg) break;
    }
    if (flg)
        printf("%s\n", res);
    else
        printf("None\n");
    return 0;
}