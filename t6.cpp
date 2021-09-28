#include <gmp.h>
#include <cstdio>

int n;

void my_mpz_powm(mpz_t res, mpz_t m, mpz_t e, mpz_t n) {
    mpz_t i, remainder, partMul;
    mpz_inits(remainder, partMul, (mpz_ptr)0);
    mpz_init_set(i, e);
    mpz_init_set(partMul, m);
    mpz_set_ui(res, 1);
    while (mpz_cmp_ui(i, 0)) {
        mpz_fdiv_r_2exp(remainder, i, 1);
        mpz_fdiv_q_2exp(i, i, 1);

        if (!mpz_cmp_ui(remainder, 1)) {
            mpz_mul(res, res, partMul);
            mpz_mod(res, res, n);
        }
        mpz_mul(partMul, partMul, partMul);
        mpz_mod(partMul, partMul, n);
    }
}

int main() {
    // freopen("t6.in", "r", stdin);
    mpz_t e, m, p, q;
    scanf("%d", &n);
    for (int i = 0; i < n; ++i) {
        mpz_inits(e, m, p, q, (mpz_ptr)0);
        gmp_scanf("%Zd%Zd%Zd%Zd", e, m, p, q);

        mpz_t N, res;
        mpz_inits(N, res, (mpz_ptr)0);
        mpz_mul(N, p, q);
        my_mpz_powm(res, m, e, N);
        gmp_printf("%Zd\n", res);
    }
    return 0;
}