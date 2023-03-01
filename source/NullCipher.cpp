#include "../include/NullCipher.h"

#include <cstring>
#include <memory>

#include "../include/Cipher.h"
#include "../include/Interface.h"
#include "../include/Range.h"

using namespace std;

namespace encfs {

static Interface NullInterface("nullCipher", 1, 0, 0);
static Range NullKeyRange(0);
static Range NullBlockRange(1, 4096, 1);

static std::shared_ptr<Cipher> NewNullCipher(const Interface& iface,
    int keyLen) {
  (void) keyLen;
  return std::shared_ptr<Cipher>(new NullCipher(iface));
}

const bool HiddenCipher = true;

static bool NullCipher_registered = Cipher::Register(
    "Null", "Non encrypting cipher. For testing only!", NullInterface,
    NullKeyRange, NullBlockRange, NewNullCipher, HiddenCipher);

class NullKey : public AbstractCipherKey {
  public:
    NullKey() = default;

    ~NullKey() override = default;

    NullKey(const NullKey& src) = delete;
    NullKey(NullKey&& other) = delete;
    NullKey& operator=(const NullKey& other) = delete;
    NullKey& operator=(NullKey&& other) = delete;
};

class NullDestructor {
  public:
    NullDestructor() = default;

    ~NullDestructor() = default;

    NullDestructor(const NullDestructor& ) = default;

    NullDestructor(NullDestructor&&) = default;

    NullDestructor& operator=(const NullDestructor&) = delete;
    NullDestructor& operator=(NullDestructor&& other) = delete;

    void operator() (NullKey *&) {}
};

// gNullkey 是接管 (new NullKey()) 对象所有权, 并且它最终会调用
// NullDestructor 的删除器
std::shared_ptr<AbstractCipherKey> gNullKey(new NullKey(), NullDestructor());

NullCipher::NullCipher(const Interface& iface_) { this->iface = iface_; }

NullCipher::~NullCipher() = default;

Interface NullCipher::interface() const { return iface; }

CipherKey NullCipher::newKey(const char*, int , int &, long,
    const unsigned char* , int) {
  return gNullKey;
}

bool NullCipher::randomize(unsigned char* buf, int len, bool) const {
  memset(buf, 0, len);
  return true;
}

CipherKey NullCipher::readKey(const unsigned char*, const CipherKey&, bool) {
  return gNullKey;
}

void NullCipher::writeKey(const CipherKey&, unsigned char* ,
                          const CipherKey&) {}

bool NullCipher::compareKey(const CipherKey& A_, const CipherKey& B_) const {
  std::shared_ptr<NullKey> A = dynamic_pointer_cast<NullKey>(A_);
  std::shared_ptr<NullKey> B = dynamic_pointer_cast<NullKey>(B_);
  return A.get() == B.get();
}

int NullCipher::encodedKeySize() const { return 0; }

int NullCipher::keySize() const { return 0; }

int NullCipher::cipherBlockSize() const { return 1; }

bool NullCipher::streamEncode(unsigned char* src, int len, uint64_t iv64,
    const CipherKey& key) const {
  (void)src;
  (void)len;
  (void)iv64;
  (void)key;
  return true;
}

bool NullCipher::streamDecode(unsigned char* src, int len, uint64_t iv64,
    const CipherKey& key) const {
  (void)src;
  (void)len;
  (void)iv64;
  (void)key;
  return true;
}

bool NullCipher::blockEncode(unsigned char*, int , uint64_t ,
    const CipherKey&) const {
  return true;
}

bool NullCipher::blockDecode(unsigned char* , int , uint64_t,
    const CipherKey& ) const {
  return true;
}

bool NullCipher::Enabled() { return true; }

}
