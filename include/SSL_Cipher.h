#ifndef _SSL_Cipher_incl_
#define _SSL_Cipher_incl_

#include <memory>
#include <stdint.h>

#include "Cipher.h"
#include "CipherKey.h"
#include "Interface.h"

#ifndef EVP_CIPHER
struct evp_cipher_st

using EVP_CIPHER = struct evp_cipher_st;
#endif

namespace encfs {
  class SSLKey;

  class SSL_Cipher : public Cipher {
    Interface iface;
    Interface realIface;
    const EVP_CIPHER *_blockCipher;
    const EVP_CIPHER *_streamCipher;
    unsigned int _keySize;
    unsigned int _ivLength;

    public:
      SSL_Cipher(const Interface& iface, const Interface& readlIface,
                 const EVP_CIPHER* blockCipher, const EVP_CIPHER* streamCipher,
                 int keyLength);
      virtual ~SSL_Cipher();

      virtual Interface interface() const;

      virtual CipherKey newKey(const char* password, int passwdLength,
                               int& iterationCount, long desiredDuration,
                               const unsigned char* salt, int saltLen);

      virtual CipherKey newKey(const char* password, int passwdLength);

      virtual CipherKey newRandomKey();

      virtual CipherKey readKey(const unsigned char* data,
                                const CipherKey& encodingKey, bool checkKey);
      virtual void writeKey(const CipherKey& key, unsigned char* data,
                            const CipherKey& encodingKey);
      virtual bool compareKey(const CipherKey& A, const CipherKey& B) const;

      virtual int keySize() const;
      virtual int encodedKeySize() const;
      virtual int cipherBlockSize() const;

      virtual bool randomize(const unsigned char* src, int len,
                             const CipherKey& key, uint64_t* augment) const;

      virtual bool streamEncode(unsigned char* in, int len, uint64_t iv64,
                                const CipherKey& key) const;
      virtual bool streamDecode(unsigned char* in, int len, uint64_t iv64,
                                const CipherKey& key) const;

      virtual bool blockEncode(unsigned char* buf, int size, uint64_t iv64,
                               const CipherKey& key) const;
      virtual bool blockDecode(unsigned char* buf, int size, uint64_t iv64,
                               const CipherKey& key) const;

      static bool Enabled();

    private:
      void setIVec(unsigned char* ivec, uint64_t seed,
                   const std::shared_ptr<SSLKey>& key) const;
      void setIVec_old(unsigned char* ivec, unsigned int seed,
                       const std::shared_ptr<SSLKey>& key) const;
  };

}


#endif
