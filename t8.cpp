#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/pkcs7.h>

#include <cstdio>
#include <cstring>

#define ERR                \
    {                      \
        printf("ERROR\n"); \
        return;            \
    }
#define BUFFER_SIZE 4096

const char cacert[] =
    "\
-----BEGIN CERTIFICATE----- \n\
MIIB/zCCAaagAwIBAgIJAKKa0PAt9M1FMAoGCCqBHM9VAYN1MFsxCzAJBgNVBAYT \n\
AkNOMQ4wDAYDVQQIDAVIdUJlaTEOMAwGA1UEBwwFV3VIYW4xDTALBgNVBAoMBEhV \n\
U1QxDDAKBgNVBAsMA0NTRTEPMA0GA1UEAwwGY2Fyb290MB4XDTIwMDkyMDIwNTkx \n\
OVoXDTMwMDkxODIwNTkxOVowWzELMAkGA1UEBhMCQ04xDjAMBgNVBAgMBUh1QmVp \n\
MQ4wDAYDVQQHDAVXdUhhbjENMAsGA1UECgwESFVTVDEMMAoGA1UECwwDQ1NFMQ8w \n\
DQYDVQQDDAZjYXJvb3QwWTATBgcqhkjOPQIBBggqgRzPVQGCLQNCAASJ8mm28JJR \n\
bZKLr6DCbZKLr6DCo1+KWimpKEsiTfZM19Zi5ao7Au6YLosyN71256MWmjwkwXxJeLa0lCfm \n\
o1+KWimpKEsiTfZM19Zi5ao7Au6YLosyN71256MWmjwkwXxJeLa0lCfm \n\
kF/YWCX6qGQ0o1MwUTAdBgNVHQ4EFgQUAL5hW3RUzqvsiTzIc1gUHeK5uzQwHwYD \n\
VR0jBBgwFoAUAL5hW3RUzqvsiTzIc1gUHeK5uzQwDwYDVR0TAQH/BAUwAwEB/zAK \n\
BggqgRzPVQGDdQNHADBEAiAaZMmvE5zzXHx/TBgdUhjtpRH3Jpd6OZ+SOAfMtKxD \n\
LAIgdKq/v2Jkmn37Y9U8FHYDfFqk5I0qlQOAmuvbVUi3yvM= \n\
-----END CERTIFICATE----- \n\
";

const char pkeyB[] =
    "\
-----BEGIN EC PARAMETERS----- \n\
BggqgRzPVQGCLQ== \n\
-----END EC PARAMETERS----- \n\
-----BEGIN EC PRIVATE KEY----- \n\
MHcCAQEEINQhCKslrI3tKt6cK4Kxkor/LBvM8PSv699Xea7kTXTToAoGCCqBHM9V \n\
AYItoUQDQgAEH7rLLiFASe3SWSsGbxFUtfPY//pXqLvgM6ROyiYhLkPxEulwrTe8 \n\
kv5R8/NA7kSSvcsGIQ9EPWhr6HnCULpklw== \n\
-----END EC PRIVATE KEY----- \n\
";

X509* getX509(const char* cert) {
    BIO* bio;
    bio = BIO_new(BIO_s_mem());
    BIO_puts(bio, cert);
    return PEM_read_bio_X509(bio, NULL, NULL, NULL);
}
EVP_PKEY* getpkey(const char* private_key) {
    BIO* bio_pkey = BIO_new_mem_buf((char*)private_key, strlen(private_key));
    if (bio_pkey == NULL) return NULL;
    return PEM_read_bio_PrivateKey(bio_pkey, NULL, NULL, NULL);
}

bool verify_signature(PKCS7* p7Cipher, BIO* p7PlainBio, X509* cert) {
    STACK_OF(PKCS7_SIGNER_INFO)* sk = PKCS7_get_signer_info(p7Cipher);
    if (!sk) return false;
    X509_STORE* store = X509_STORE_new();
    X509_STORE_CTX* ctx = X509_STORE_CTX_new();
    X509_STORE_add_cert(store, cert);
    int signNum = sk_PKCS7_SIGNER_INFO_num(sk);
    int flag = true;
    for (int i = 0; i < signNum; ++i) {
        PKCS7_SIGNER_INFO* signInfo = sk_PKCS7_SIGNER_INFO_value(sk, i);
        auto res = PKCS7_dataVerify(store, ctx, p7PlainBio, p7Cipher, signInfo);
        PKCS7_SIGNER_INFO_free(signInfo);
        if (res <= 0) {
            flag = false;
            break;
        }
    }
    X509_STORE_free(store);
    X509_STORE_CTX_free(ctx);
    sk_PKCS7_SIGNER_INFO_free(sk);
    return flag;
}

void pkcs7_decrypt() {
    char p7CipherText[BUFFER_SIZE];
    fread(p7CipherText, sizeof(p7CipherText) - 1, 1, stdin);

    BIO* p7CipherBio =
        BIO_new_mem_buf((char*)p7CipherText, strlen(p7CipherText));
    if (!p7CipherBio) ERR;

    PKCS7* p7Cipher = PEM_read_bio_PKCS7(p7CipherBio, NULL, NULL, NULL);
    if (!p7Cipher) ERR;

    BIO* p7PlainBio = PKCS7_dataDecode(p7Cipher, getpkey(pkeyB), NULL, NULL);
    printf("!!!\n");
    if (!p7PlainBio) ERR;

    X509* cert = getX509(cacert);
    if (!cert) ERR;

    char p7PlainText[BUFFER_SIZE];
    int p7Plainlen = BIO_read(p7PlainBio, p7PlainText, sizeof(p7PlainText));
    if (p7Plainlen <= 0) ERR;

    bool signOk = verify_signature(p7Cipher, p7PlainBio, cert);
    if (!signOk) ERR;

    BIO* out = BIO_new_fd(fileno(stdout), BIO_NOCLOSE);
    BIO_write(out, p7PlainText, p7Plainlen);
    X509_free(cert);
}

int main() {
    freopen("test.in", "r", stdin);
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    pkcs7_decrypt();
    return 0;
}