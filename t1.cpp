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
// 标记16位p盒置换的原位置
unsigned short oldPos[16] = {0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400,
                             0x0200, 0x0100, 0x0080, 0x0040, 0x0020, 0x0010,
                             0x0008, 0x0004, 0x0002, 0x0001};
// 标记16位p盒置换后要去的位置
unsigned short newPos[16] = {0x8000, 0x0800, 0x0080, 0x0008, 0x4000, 0x0400,
                             0x0040, 0x0004, 0x2000, 0x0200, 0x0020, 0x0002,
                             0x1000, 0x0100, 0x0010, 0x0001};
unsigned short pbox16[0x10000];
void initSPbox() {
    for (int i = 0; i < 0x10000; ++i) {
        // 穷举所有可能的16位输入u，把u拆成4份分别进行sbox4，然后合成sbox16
        sbox16[i] = (sbox4[i >> 12] << 12) | (sbox4[(i >> 8) & 0xf] << 8) |
                    (sbox4[(i >> 4) & 0xf] << 4) | (sbox4[i & 0xf]);
        resbox16[i] = (resbox4[i >> 12] << 12) |
                      (resbox4[(i >> 8) & 0xf] << 8) |
                      (resbox4[(i >> 4) & 0xf] << 4) | (resbox4[i & 0xf]);
        for (int j = 0; j < 16; ++j) {
            // 对v的每一位进行置换，当从左往右第j位（从0开始算）有1时，把1放到该去的新位子上
            if (sbox16[i] & oldPos[j]) spbox16[i] |= newPos[j];
            if (resbox16[i] & oldPos[j]) respbox16[i] |= newPos[j];
            if (i & oldPos[j]) pbox16[i] |= newPos[j];
        }
    }
}

unsigned short getSubKey(unsigned int key, int idx) {
    // 根据课本p59的demo进行密钥迭代
    idx = 4 - idx;
    return key >> (idx << 2);
}

unsigned short SPN(unsigned short plain) {
    // 按照课本p59内容进行加密
    unsigned short cipher = plain;
    cipher = spbox16[cipher ^ getSubKey(key, 0)];
    cipher = spbox16[cipher ^ getSubKey(key, 1)];
    cipher = spbox16[cipher ^ getSubKey(key, 2)];
    cipher = sbox16[cipher ^ getSubKey(key, 3)];
    cipher = cipher ^ getSubKey(key, 4);
    return cipher;
}

unsigned short reSPN(unsigned short cipher) {
    // 按照等价解密（mooc课件《分组迭代密码的设计思想》）流程进行解密
    unsigned short plain = cipher;
    plain = respbox16[plain ^ getSubKey(key, 4)];
    plain = respbox16[plain ^ pbox16[getSubKey(key, 3)]];
    plain = respbox16[plain ^ pbox16[getSubKey(key, 2)]];
    plain = resbox16[plain ^ pbox16[getSubKey(key, 1)]];
    plain = plain ^ getSubKey(key, 0);
    return plain;
}

unsigned int read() {
    // 快速读入
    unsigned int x = 0;
    char ch = 0;
    while (ch < '0' || ch > 'f')
        ch = getchar();
    while (1) {
        if (ch >= '0' && ch <= '9')
            x = x * 16 + (ch - '0');
        else if (ch >= 'a' && ch <= 'f')
            x = x * 16 + (ch - 'a') + 10;
        else
            break;
        ch = getchar();
    }
    return x;
}

void write(unsigned short x) {
    // 快速输出
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
    // freopen("t1.in", "r", stdin);
    // freopen("t1.out", "w", stdout);
    // 初始化sbox16、pbox16以及spbox16
    initSPbox();
    scanf("%d", &n);
    for (int i = 0; i < n; ++i) {
        // 读入数据
        key = read();
        plain = read();
        // 加密
        cipher = SPN(plain);
        write(cipher);
        putchar(' ');
        // 末尾翻转后解密
        write(reSPN(cipher ^ 1));
        putchar('\n');
    }
    return 0;
}