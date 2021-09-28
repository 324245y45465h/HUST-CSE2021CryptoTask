#include <cctype>
#include <cstdio>
#include <cstring>
#define MAX_G1 100
#define MAX_G2 3

int n, flg;
int cnt[16][16], cnt2[16][16];
unsigned short plains[8010], ciphers[8010];
int maxCnt = -1;
unsigned char maxl1, maxl2, maxl3, maxl4;
unsigned char u41, u42, u43, u44, z;
unsigned int key;

unsigned short sbox4[16] = {0xE, 0x4, 0xD, 0x1, 0x2, 0xF, 0xB, 0x8,
                            0x3, 0xA, 0x6, 0xC, 0x5, 0x9, 0x0, 0x7};
unsigned short resbox4[16] = {0xe, 0x3, 0x4, 0x8, 0x1, 0xc, 0xa, 0xf,
                              0x7, 0xd, 0x9, 0x6, 0xb, 0x2, 0x0, 0x5};
unsigned short sbox16[0x10000], spbox16[0x10000];
unsigned short pos[16] = {0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400,
                          0x0200, 0x0100, 0x0080, 0x0040, 0x0020, 0x0010,
                          0x0008, 0x0004, 0x0002, 0x0001};
unsigned short pBox[16] = {0x8000, 0x0800, 0x0080, 0x0008, 0x4000, 0x0400,
                           0x0040, 0x0004, 0x2000, 0x0200, 0x0020, 0x0002,
                           0x1000, 0x0100, 0x0010, 0x0001};

void initSPbox() {
    for (int i = 0; i < 0x10000; ++i) {
        sbox16[i] = (sbox4[i >> 12] << 12) | (sbox4[(i >> 8) & 0xf] << 8) |
                    (sbox4[(i >> 4) & 0xf] << 4) | (sbox4[i & 0xf]);
        for (int j = 0; j < 16; ++j)
            if (sbox16[i] & pos[j]) spbox16[i] |= pBox[j];
    }
}

unsigned short getSubKey(unsigned int key, int idx) {
    idx = 4 - idx;
    return key >> (idx << 2);
}

unsigned short SPN(unsigned short plain) {
    unsigned short cipher = plain;
    cipher = spbox16[cipher ^ getSubKey(key, 0)];
    cipher = spbox16[cipher ^ getSubKey(key, 1)];
    cipher = spbox16[cipher ^ getSubKey(key, 2)];
    cipher = sbox16[cipher ^ getSubKey(key, 3)];
    cipher = cipher ^ getSubKey(key, 4);
    return cipher;
}

unsigned int read() {
    unsigned int x = 0;
    char ch = 0;
    while (ch < '0')
        ch = getchar();
    int i = 4;
    while (i--) {
        if (isdigit(ch))
            x = x * 16 + (ch - '0');
        else
            x = x * 16 + (ch - 'a') + 10;
        ch = getchar();
    }
    return x;
}

void abs(int cnt[16][16]) {
    for (unsigned char i = 0; i <= 0xf; ++i)
        for (unsigned char j = 0; j <= 0xf; ++j)
            cnt[i][j] =
                (cnt[i][j] > 4000) ? (cnt[i][j] - 4000) : (4000 - cnt[i][j]);
}

int main() {
    // freopen("t2.in", "r", stdin);
    // freopen("t2.out", "w", stdout);
    scanf("%d", &n);
    initSPbox();
    for (int i = 0; i < n; ++i) {
        flg = 0;
        memset(cnt, 0, sizeof(cnt));
        for (int j = 0; j < 8000; ++j)
            plains[j] = read(), ciphers[j] = read();
        for (int j = 0; j < 8000; ++j) {
            unsigned char tx =
                (plains[j] >> 11) ^ (plains[j] >> 9) ^ (plains[j] >> 8);
            unsigned char ty1 = ciphers[j] >> 8 & 0xf;
            unsigned char ty2 = ciphers[j] & 0xf;
            for (int l2 = 0; l2 < 0x10; ++l2) {
                for (int l4 = 0; l4 < 0x10; ++l4) {
                    u42 = resbox4[l2 ^ ty1];
                    u44 = resbox4[l4 ^ ty2];
                    cnt[l2][l4] +=
                        (tx ^ (u42 >> 2) ^ u42 ^ (u44 >> 2) ^ u44) & 0x1u;
                }
            }
        }
        abs(cnt);
        for (int g1 = 0; g1 < MAX_G1; ++g1) {
            maxCnt = -1;
            for (int l2 = 0; l2 < 0x10; ++l2) {
                for (int l4 = 0; l4 < 0x10; ++l4) {
                    if (cnt[l2][l4] > maxCnt) {
                        maxCnt = cnt[l2][l4];
                        maxl2 = l2;
                        maxl4 = l4;
                    }
                }
            }
            cnt[maxl2][maxl4] = 0;

            memset(cnt2, 0, sizeof(cnt2));
            for (int j = 0; j < 8000; ++j) {
                unsigned char tx = (plains[j] >> 11) ^ (plains[j] >> 10) ^
                                   (plains[j] >> 9) ^ (plains[j] >> 8);
                unsigned char u42 = resbox4[maxl2 ^ (ciphers[j] >> 8) & 0xf];
                unsigned char ty1 = (ciphers[j] >> 12) & 0xf;
                unsigned char ty2 = (ciphers[j] >> 4) & 0xf;
                unsigned char ty3 = (u42 >> 2) ^ u42;
                for (int l1 = 0; l1 < 0x10; ++l1) {
                    for (int l3 = 0; l3 < 0x10; ++l3) {
                        unsigned char u41 = resbox4[l1 ^ ty1];
                        unsigned char u43 = resbox4[l3 ^ ty2];
                        cnt2[l1][l3] +=
                            (tx ^ (u41 >> 2) ^ u41 ^ ty3 ^ (u43 >> 2) ^ u43) &
                            0x1u;
                    }
                }
            }
            abs(cnt2);
            for (int g2 = 0; g2 < MAX_G2; ++g2) {
                maxCnt = -1;
                for (int l1 = 0; l1 < 0x10; ++l1) {
                    for (int l3 = 0; l3 < 0x10; ++l3) {
                        if (cnt2[l1][l3] > maxCnt) {
                            maxCnt = cnt2[l1][l3];
                            maxl1 = l1;
                            maxl3 = l3;
                        }
                    }
                }
                cnt2[maxl1][maxl3] = 0;

                unsigned short rear16bitKey =
                    (maxl1 << 12) | (maxl2 << 8) | (maxl3 << 4) | maxl4;
                for (int front16bitKey = 0; front16bitKey < 0x10000;
                     ++front16bitKey) {
                    key = (front16bitKey << 16) | rear16bitKey;
                    if (SPN(plains[0]) != ciphers[0]) continue;
                    if (SPN(plains[1]) != ciphers[1]) continue;
                    flg = 1;
                    break;
                }
                if (flg) break;
            }
            if (flg) break;
        }
        printf("%08x\n", key);
    }
    return 0;
}