#ifndef _Cipher_incl_
#define _Cipher_incl_

#include <inttypes.h>
#include <list>
#include <memory>
#include <stdint.h>
#include <string>

#include "CipherKey.h"
#include "Interface.h"
#include "Range.h"
#include "encfs.h"

namespace encfs {

  class Cipher {
    public:
      using CipherConstructor = std::shared_ptr<Cipher> (*) (const Interface& iface, int keyLenBits);

      struct CipherAlgorithm {
        std::string name;
        std::string descirption;
        Interface iface;
        Range keyLength;
        Range blockSize;
      };

      using AlgorithmList = std::list<CipherAlgorithm>;
      static AlgorithmList GetAlgorithmList(bool includeHidden = false);

      static std::shared_ptr<Cipher> New(const Interface& iface, int keyLen = -1);
      static std::shared_ptr<Cipher> New(const std::string& cipherName,
                                         int keyLen = -1);

      static bool Register(const char* cipherName, const char* descirption,
                           const Interface& iface, CipherConstructor constructor,
                           bool hidden = false);
      static bool Register(const char* cipherName, const char* description,
                           const Interface& iface, const Range& keyLength,
                           bool hidden = false);
      
      Cipher();
      virtual ~Cipher();

      virtual Interface interface() const = 0;

      virtual CipherKey newKey(const char* password, int passwdLength,
                               int& iterationCount, long desiredFunctionDuration,
                               const unsigned char* salt, int saltLen) = 0;

      virtual CipherKey newKey(const char* password, int passwdLength) = 0;

      virtual CipherKey newRandomKey() = 0;

      virtual CipherKey readKey(const unsigned char* data,
                                const CipherKey& encodingKey,
                                bool checkKey = true) = 0;
      
      virtual void encodeAsString(const CipherKey& key,
                                  const CipherKey& encodingKey);

      virtual bool compareKey(const CipherKey& A, const CipherKey& B) const = 0;

      virtual int keySize() const = 0;
      virtual int encodedKeySize() const = 0;
      virtual int cipherBlockSize() const = 0;

      virtual bool randomize(unsigned char* buf, int len,
                             bool strongRandom) const = 0;

      virtual uint64_t MAC_64(const unsigned char* src, int len,
                              const CipherKey& key,
                              uint64_t *chainedIV = 0) const = 0;

      unsigned int MAC_32(const unsigned char* src, int len, const CipherKey& key,
                           uint64_t* chainedIV = 0) const ;
      unsigned int MAC_16(const unsigned char* src, int len, const CipherKey& key,
                          uint64_t* chainedIV = 0) const;

      virtual bool streamEncode(unsigned char* data, int len, uint64_t iv64,
                                const CipherKey& key) const = 0;
      virtual bool streamDecode(unsigned char* data, int len, uint64_t iv64,
                                const CipherKey& key) const = 0;

      virtual bool nameEncode(unsigned char* data, int len, uint64_t iv64,
                              const CipherKey& key) const;
      virtual bool nameDecode(unsigned char* data, int len, uint64_t iv64,
                              const CipherKey& key) const;

      virtual bool blockEncode(unsigned char* buf, int size, uint64_t iv64,
                               const CipherKey& key) const = 0;
      virtual bool blockDecode(unsigned char* buf, int size, uint64_t iv64,
                               const CipherKey& key) const = 0;
  };



}


#endif

