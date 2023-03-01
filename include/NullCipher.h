#ifndef _NullCipher_incl_
#define _NullCipher_incl_

#include <stdint.h>

#include "Cipher.h"
#include "CipherKey.h"
#include "Interface.h"

namespace encfs {


  class NullCipher : public Cipher {
    Interface iface;

    public:
      NullCipher(const Interface& iface);
      virtual ~NullCipher();

      virtual Interface interface() const;

      virtual CipherKey newKey(const char* password, int passwdLength,
                               int& iterationCount, long desiredDuration,
                               const unsigned char* salt, int saltlen);
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

      virtual bool randomize(unsigned char* buf, int len, bool strongRandom) const;

      virtual uint64_t MAC_64(const unsigned char* data, int len,
                              const CipherKey& key, uint64_t* chainedIV) const;

      virtual bool streamEncode(unsigned char* in, int len, uint64_t iv64,
                                const CipherKey& key) const;
      virtual bool streamDecode(unsigned char* in, int len, uint64_t iv64,
                                const CipherKey& key) const;

      virtual bool blockEncode(unsigned char* buf, int size, uint64_t iv64,
                               const CipherKey& key) const;
      virtual bool blockDecode(unsigned char* buf, int size, uint64_t iv64,
                               const CipherKey& key) const;

      static bool Enabled();
  };


}

#endif

