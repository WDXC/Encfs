#ifndef _NullNameIO_incl_
#define _NullNameIO_incl_

#include <stdint.h>

#include "Interface.h"
#include "NameIO.h"

namespace encfs {

class NullNameIO : public NameIO {
 public:
  static Interface CurrentInterface();

  NullNameIO();

  virtual ~NullNameIO();

  virtual Interface interface() const;

  virtual int maxEncodedNameLen(int plaintextNameLen) const;
  virtual int maxDecodedNameLen(int encodedNameLen) const;

  // hack to help with static builds
  static bool Enabled();

 protected:
  virtual int encodeName(const char *plaintextName, int length, uint64_t *iv,
                         char *encodedName, int bufferLength) const;
  virtual int decodeName(const char *encodedName, int length, uint64_t *iv,
                         char *plaintextName, int bufferLength) const;

 private:
};

}  // namespace encfs

#endif
