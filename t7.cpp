#include <gmp.h>
#include <cstdio>

int n;

void my_mpz_gcd(mpz_t rop, mpz_t op1, mpz_t op2) {
    mpz_t t1, t2, t3;
    mpz_init_set(t1, op1);
    mpz_init_set(t2, op2);
    mpz_init(t3);
    if (mpz_cmp(op1, op2) < 0) mpz_swap(op1, op2);
    mpz_mod(t3, t1, t2);
    while (mpz_cmp_ui(t3, 0)) {
        mpz_set(t1, t2);
        mpz_set(t2, t3);
        mpz_mod(t3, t1, t2);
    }
    mpz_set(rop, t2);
    mpz_clears(t1, t2, t3, nullptr);
}

bool my_mpz_invert(mpz_t rop, mpz_t op1, mpz_t op2) {
    mpz_t gcd;
    mpz_t op2_bak;
    mpz_init_set(op2_bak, op2);
    mpz_init(gcd);
    my_mpz_gcd(gcd, op1, op2);
    if (mpz_cmp_ui(gcd, 1)) return false;

    if (mpz_cmp(op1, op2) > 0) mpz_swap(op1, op2);
    mpz_t x1, x2, x3, y1, y2, y3, p, k, z1, z2, z3;
    mpz_init_set_ui(x1, 1);
    mpz_init_set_ui(x2, 0);
    mpz_init_set(x3, op2);
    mpz_init_set_ui(y1, 0);
    mpz_init_set_ui(y2, 1);
    mpz_init_set(y3, op1);
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

void my_rsa_crt(mpz_t res, mpz_t c, mpz_t d, mpz_t p, mpz_t q, mpz_t dp, mpz_t dq, mpz_t qinv, mpz_t N) {
    mpz_t m1, m2, h, tmp;
    mpz_inits(m1, m2, h, tmp, (mpz_ptr)0);
    mpz_powm(m1, c, dp, p);
    mpz_powm(m2, c, dq, q);

    mpz_sub(h, m1, m2);
    mpz_mul(h, h, qinv);
    mpz_mod(h, h, p);

    mpz_mul(res, h, q);
    mpz_add(res, res, m2);
    mpz_mod(res, res, N);
}

int main() {
    freopen("t7.in", "r", stdin);
    freopen("t7.out", "w", stdout);
    mpz_t p, q, dp, dq, qinv, e, d, c, res, p1, q1, N, phi_N;
    mpz_inits(p, q, dp, dq, qinv, e, d, c, res, p1, q1, N, phi_N, (mpz_ptr)0);
    scanf("%d", &n);
    gmp_scanf("%Zd%Zd%Zd", p, q, e);
    mpz_sub_ui(p1, p, 1);
    mpz_sub_ui(q1, q, 1);
    mpz_mul(N, p, q);
    mpz_mul(phi_N, p1, q1);
    my_mpz_invert(d, e, phi_N);
    mpz_mod(dp, d, p1);
    mpz_mod(dq, d, q1);
    mpz_invert(qinv, q, p);
    for (int i = 0; i < n; ++i) {
        gmp_scanf("%Zd", c);
        my_rsa_crt(res, c, d, p, q, dp, dq, qinv, N);
        gmp_printf("%Zd\n", res);
    }
    return 0;
}