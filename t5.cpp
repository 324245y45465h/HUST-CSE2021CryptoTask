#include <gmp.h>
#define ERR                \
    {                      \
        printf("ERROR\n"); \
        continue;          \
    }
#define MIN_DIFF 1 << 31
#define MIN_E 1 << 16
#define MAX_GCD 1 << 4

int n;

bool checkPQok(const mpz_t p, const mpz_t q) {
    mpz_t mid, tmp;
    mpz_inits(mid, tmp, (mpz_ptr)0);
    mpz_mul(mid, p, q);
    mpz_sqrt(mid, mid);

    mpz_sub(tmp, p, mid);
    mpz_abs(tmp, tmp);
    if (mpz_cmp_ui(tmp, MIN_DIFF) <= 0) return false;
    mpz_sub(tmp, q, mid);
    mpz_abs(tmp, tmp);
    if (mpz_cmp_ui(tmp, MIN_DIFF) <= 0) return false;
    return true;
}

void my_mpz_gcd(mpz_t rop, const mpz_t op1, const mpz_t op2) {
    // 自己实现的mpz_gcd，参数格式同原mpz_gcd
    // 求op1与op2的gcd，结果赋rop
    // 需要注意的是，因为mpz_t类型本质上是一种指针，所以这里相当于是引用传参
    // 如果在函数中修改了op1和op2，都会影响到main函数中的数值
    mpz_t t1, t2, t3;
    mpz_init_set(t1, op1);
    mpz_init_set(t2, op2);
    mpz_init(t3);
    mpz_mod(t3, t1, t2);
    while (mpz_cmp_ui(t3, 0)) {
        mpz_set(t1, t2);
        mpz_set(t2, t3);
        mpz_mod(t3, t1, t2);
    }
    mpz_set(rop, t2);
}

bool my_mpz_invert(mpz_t rop, const mpz_t op1, const mpz_t op2) {
    // 自己实现的mpz_invert，同上
    mpz_t gcd;
    mpz_t op2_bak;
    mpz_init_set(op2_bak, op2);
    mpz_init(gcd);
    my_mpz_gcd(gcd, op1, op2);
    if (mpz_cmp_ui(gcd, 1)) return false;

    mpz_t x1, x2, x3, y1, y2, y3, p, k, z1, z2, z3;
    mpz_init_set_ui(x1, 1);
    mpz_init_set_ui(x2, 0);
    mpz_init_set(x3, op2);
    mpz_init_set_ui(y1, 0);
    mpz_init_set_ui(y2, 1);
    mpz_init_set(y3, op1);
    if (mpz_cmp(x3, y3) < 0) mpz_swap(x3, y3);
    mpz_inits(p, k, z1, z2, z3, (mpz_ptr)0);
    while (1) {
        if (!mpz_cmp_ui(y3, 0)) {
            mpz_set_ui(rop, 0);
            break;
        } else if (!mpz_cmp_ui(y3, 1)) {
            mpz_set(rop, y2);
            break;
        }

        mpz_mod(p, x3, y3);
        mpz_sub(k, x3, p);
        mpz_cdiv_q(k, k, y3);
        mpz_mul(z1, k, y1);
        mpz_sub(z1, x1, z1);
        mpz_mul(z2, k, y2);
        mpz_sub(z2, x2, z2);
        mpz_mul(z3, k, y3);
        mpz_sub(z3, x3, z3);
        mpz_set(x1, y1);
        mpz_set(x2, y2);
        mpz_set(x3, y3);
        mpz_set(y1, z1);
        mpz_set(y2, z2);
        mpz_set(y3, z3);
    }
    if (mpz_cmp_ui(rop, 0) <= 0) mpz_add(rop, rop, op2_bak);
    return true;
}

int main() {
    // freopen("test.in", "r", stdin);
    // freopen("test.out", "w", stdout);
    scanf("%d", &n);
    mpz_t e, p, q, d, N, phi_N;
    mpz_inits(e, p, q, d, N, phi_N, (mpz_ptr)0);
    for (int i = 0; i < n; ++i) {
        gmp_scanf("%Zd%Zd%Zd", e, p, q);
        // 检查e是否过小
        if (mpz_cmp_ui(e, MIN_E) <= 0) ERR;

        // 检查p、q是否都是素数，这里直接调用了
        int isPrime = mpz_probab_prime_p(p, 20) + mpz_probab_prime_p(q, 20);
        if (isPrime < 2) ERR;

        // 检查p、q取值是否过于平滑，即检查其余(p * q) ^ (1/2)的差是否过小
        if (!checkPQok(p, q)) ERR;

        // 计算phi_N = (p-1)*(q-1)
        // 同时检查(p-1)与(q-1)的最大公倍数不要太大
        mpz_t gcd;
        mpz_init(gcd);
        mpz_sub_ui(p, p, 1);
        mpz_sub_ui(q, q, 1);
        mpz_mul(phi_N, p, q);
        my_mpz_gcd(gcd, p, q);
        if (mpz_cmp_ui(gcd, MAX_GCD) > 0) ERR;

        // 检查e是否与phi_N是否互素，也就是是否存在e的逆d
        // 如果存在，则返回1，d赋值为phi_N下e的逆元
        // 如果不存在，则返回0，报错
        if (!my_mpz_invert(d, e, phi_N)) ERR;
        gmp_printf("%Zd\n", d);
        mpz_clear(gcd);
    }
    mpz_clears(e, p, q, d, N, phi_N, NULL);
    return 0;
}