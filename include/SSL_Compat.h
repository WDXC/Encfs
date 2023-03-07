#ifndef _SSL_Compat_incl_
#define _SSL_Compat_incl_

// OpenSSL < 1.1.0 or LibreSSL
#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)

// Equivalent methods
#define EVP_MD_CTX_new EVP_MD_CTX_create
#define EVP_MD_CTX_free EVP_MD_CTX_destroy
#define HMAC_CTX_reset HMAC_CTX_cleanup

// Missing methods (based on 1.1.0 versions)
HMAC_CTX *HMAC_CTX_new(void) {
  HMAC_CTX *ctx = (HMAC_CTX *)OPENSSL_malloc(sizeof(HMAC_CTX));
  if (ctx != NULL) {
    memset(ctx, 0, sizeof(HMAC_CTX));
    HMAC_CTX_reset(ctx);
  }
  return ctx;
}

void HMAC_CTX_free(HMAC_CTX *ctx) {
  if (ctx != NULL) {
    HMAC_CTX_cleanup(ctx);
    OPENSSL_free(ctx);
  }
}
#endif

#endif
