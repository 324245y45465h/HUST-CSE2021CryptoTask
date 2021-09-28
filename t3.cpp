#include <cstdio>
#include <cstring>
#include <cctype>
#define MAX_G1 10
#define MAX_G2 10
#define STARTX 12345
#define ENDX 20567

typedef unsigned char uchar;
typedef unsigned short ushort;

int n, flg;
int cnt[16][16], cnt2[16][16];
ushort plains[0x10000], ciphers[0x10000];
bool used[16][16];
int maxCnt = -1;
uchar maxl1, maxl2, maxl3, maxl4;
uchar u41, u42, u43, u44, z;
unsigned int key;

ushort sbox4[16] = {
    0xE, 0x4, 0xD, 0x1, 0x2, 0xF, 0xB, 0x8, 
    0x3, 0xA, 0x6, 0xC, 0x5, 0x9, 0x0, 0x7
};
ushort resbox4[16] = {
    0xe, 0x3, 0x4, 0x8, 0x1, 0xc, 0xa, 0xf, 
    0x7, 0xd, 0x9, 0x6, 0xb, 0x2, 0x0, 0x5
};
ushort sbox16[0x10000], spbox16[0x10000];
ushort pos[16] = {
    0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400, 0x0200, 0x0100,
    0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004, 0x0002, 0x0001};
ushort pBox[16] = {
    0x8000, 0x0800, 0x0080, 0x0008, 0x4000, 0x0400, 0x0040, 0x0004,
    0x2000, 0x0200, 0x0020, 0x0002, 0x1000, 0x0100, 0x0010, 0x0001};

void initSPbox() {
    for (int i = 0; i < 0x10000; ++i) {
        sbox16[i] = (sbox4[i >> 12] << 12) 
            | (sbox4[(i >> 8) & 0xf] << 8)
            | (sbox4[(i >> 4) & 0xf] << 4)
            | (sbox4[i & 0xf]);
        for (int j = 0; j < 16; ++j)
            if (sbox16[i] & pos[j]) 
                spbox16[i] |= pBox[j];
    }
}

ushort getSubKey(unsigned int key, int idx) {
    idx = 4 - idx;
    return key >> (idx << 2);
}

ushort SPN(ushort plain) {
    ushort cipher = plain;
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
    while (ch < '0') ch = getchar();
    int i = 4;
    while (i--) {
        if (isdigit(ch)) x = x * 16 + (ch - '0');
        else x = x * 16 + (ch - 'a') + 10;
        ch = getchar();
    }
    return x;
}

int main() {
    // freopen("t3.in", "r", stdin);
    // freopen("t3my.out", "w", stdout);
    scanf("%d", &n);
    initSPbox();
    for (int i = 0; i < n; ++i) {
        flg = 0;
        memset(cnt, 0, sizeof(cnt));
        memset(cnt2, 0, sizeof(cnt));
        for (int j = 0; j < 0x10000; ++j) {
            ciphers[j] = read();
        }
        
        ushort xx = 0x0b00;
        for (ushort x1 = STARTX; x1 < ENDX; ++x1) {
            uchar v421, v441, u421, u441, v422, v442, u422, u442;
            ushort x2 = x1 ^ xx;
            ushort y1 = ciphers[x1];
            ushort y2 = ciphers[x2];
            if ((y1 ^ y2) & 0xf0f0) continue;
            for (uchar l2 = 0; l2 <= 0xf; ++l2) {
                for (uchar l4 = 0; l4 <= 0xf; ++l4) {
                    v421 = l2 ^ ((y1 >> 8) & 0xf);
                    v441 = l4 ^ (y1 & 0xf);
                    u421 = resbox4[v421];
                    u441 = resbox4[v441];
                    v422 = l2 ^ ((y2 >> 8) & 0xf);
                    v442 = l4 ^ (y2 & 0xf);
                    u422 = resbox4[v422];
                    u442 = resbox4[v442];
                    cnt[l2][l4] += ((u421 ^ u422) == 0x6 && (u441 ^ u442) == 0x6) & 0x1u;

                }
            }
        }

        xx = 0x0020;
        for (ushort x1 = STARTX; x1 < ENDX; ++x1) {
            uchar v411, v431, u411, u431, v412, v432, u412, u432;
            ushort x2 = x1 ^ xx;
            ushort y1 = ciphers[x1];
            ushort y2 = ciphers[x2];
            if ((y1 ^ y2) & 0x0f0f) continue;
            for (uchar l1 = 0; l1 <= 0xf; ++l1) {
                for (uchar l3 = 0; l3 <= 0xf; ++l3) {
                    v411 = l1 ^ ((y1 >> 12) & 0xf);
                    v431 = l3 ^ ((y1 >> 4) & 0xf);
                    u411 = resbox4[v411];
                    u431 = resbox4[v431];
                    v412 = l1 ^ ((y2 >> 12) & 0xf);
                    v432 = l3 ^ ((y2 >> 4) & 0xf);
                    u412 = resbox4[v412];
                    u432 = resbox4[v432];
                    cnt2[l1][l3] += ((u411 ^ u412) == 0x5 && (u431 ^ u432) == 0x5) & 0x1u;

                }
            }
        }

        for (int g1 = 0; g1 < MAX_G1; ++g1) {
            int maxn24 = -1;
            for (uchar l2 = 0; l2 <= 0xf; ++l2) {
                for (uchar l4 = 0; l4 <= 0xf; ++l4) {
                    if (maxn24 < cnt[l2][l4]) {
                        maxn24 = cnt[l2][l4];
                        maxl2 = l2;
                        maxl4 = l4;
                    }
                }
            }
            cnt[maxl2][maxl4] = 0;
            memset(used, false, sizeof(used));
            for (int g2 = 0; g2 < MAX_G2; ++g2) {
                int maxn13 = -1;
                for (uchar l1 = 0; l1 <= 0xf; ++l1) {
                    for (uchar l3 = 0; l3 <= 0xf; ++l3) {
                        if (maxn13 < cnt2[l1][l3] && !used[l1][l3]) {
                            maxn13 = cnt2[l1][l3];
                            maxl1 = l1;
                            maxl3 = l3;
                        }
                    }
                }
                used[maxl1][maxl3] = true;
                ushort rear16BitKey = 
                    (maxl1 << 12) | (maxl2 << 8) | (maxl3 << 4) | maxl4;
                for (int front16BitKey = 0; front16BitKey <= 0xffff; ++front16BitKey) {
                    key = (front16BitKey << 16) | rear16BitKey;
                    ushort test1 = 0x3647, test2 = 0x45ab;
                    if (SPN(test1) != ciphers[test1]) continue;
                    if (SPN(test2) != ciphers[test2]) continue;
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