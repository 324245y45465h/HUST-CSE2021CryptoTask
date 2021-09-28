#include <cctype>
#include <cstdio>

int n;
unsigned int key;
unsigned short subKey;
unsigned short plain;
unsigned short cipher;

unsigned short sbox4[16] = {0xE, 0x4, 0xD, 0x1, 0x2, 0xF, 0xB, 0x8,
                            0x3, 0xA, 0x6, 0xC, 0x5, 0x9, 0x0, 0x7};
unsigned short resbox4[16] = {0xe, 0x3, 0x4, 0x8, 0x1, 0xc, 0xa, 0xf,
                              0x7, 0xd, 0x9, 0x6, 0xb, 0x2, 0x0, 0x5};
unsigned short sbox16[0x10000], spbox16[0x10000];
unsigned short resbox16[0x10000], respbox16[0x10000];
unsigned short pos[16] = {0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400,
                          0x0200, 0x0100, 0x0080, 0x0040, 0x0020, 0x0010,
                          0x0008, 0x0004, 0x0002, 0x0001};
unsigned short pBox[16] = {0x8000, 0x0800, 0x0080, 0x0008, 0x4000, 0x0400,
                           0x0040, 0x0004, 0x2000, 0x0200, 0x0020, 0x0002,
                           0x1000, 0x0100, 0x0010, 0x0001};
unsigned short pbox16[0x10000];
void initSPbox() {
    for (int i = 0; i < 0x10000; ++i) {
        sbox16[i] = (sbox4[i >> 12] << 12) | (sbox4[(i >> 8) & 0xf] << 8) |
                    (sbox4[(i >> 4) & 0xf] << 4) | (sbox4[i & 0xf]);
        resbox16[i] = (resbox4[i >> 12] << 12) |
                      (resbox4[(i >> 8) & 0xf] << 8) |
                      (resbox4[(i >> 4) & 0xf] << 4) | (resbox4[i & 0xf]);
        for (int j = 0; j < 16; ++j) {
            if (sbox16[i] & pos[j]) spbox16[i] |= pBox[j];
            if (resbox16[i] & pos[j]) respbox16[i] |= pBox[j];
            if (i & pos[j]) pbox16[i] |= pBox[j];
        }
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

unsigned short reSPN(unsigned short cipher) {
    unsigned short plain = cipher;
    plain = respbox16[plain ^ getSubKey(key, 4)];
    plain = respbox16[plain ^ pbox16[getSubKey(key, 3)]];
    plain = respbox16[plain ^ pbox16[getSubKey(key, 2)]];
    plain = resbox16[plain ^ pbox16[getSubKey(key, 1)]];
    plain = plain ^ getSubKey(key, 0);
    return plain;
}

unsigned int read() {
    unsigned int x = 0;
    char ch = 0;
    while (ch < '0' || ch > 'f') {
        ch = getchar();
    }
    while (1) {
        if (ch >= '0' && ch <= '9') {
            x = x * 16 + (ch - '0');
        } else if (ch >= 'a' && ch <= 'f') {
            x = x * 16 + (ch - 'a') + 10;
        } else
            break;
        ch = getchar();
    }
    return x;
}

inline void write(unsigned short x) {
    static unsigned short sta[4];
    sta[0] = sta[1] = sta[2] = sta[3] = 0;
    int top = 0;
    do {
        sta[top++] = x % 16, x /= 16;
    } while (x);
    top = 4;
    while (top) {
        --top;
        if (sta[top] >= 10)
            putchar(sta[top] + 'a' - 10);
        else
            putchar(sta[top] + '0');
    }
}

int main() {
    freopen("t1.in", "r", stdin);
    freopen("t1.out", "w", stdout);
    initSPbox();
    scanf("%d", &n);
    for (int i = 0; i < n; ++i) {
        key = read();
        plain = read();
        cipher = SPN(plain);
        write(cipher);
        putchar(' ');
        write(reSPN(cipher ^ 1));
        putchar('\n');
    }
    return 0;
}