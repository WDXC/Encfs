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
      
      virtual CipherKey newKey

  };



}


#endif

