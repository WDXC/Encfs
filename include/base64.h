#ifndef _base64_incl_
#define _base64_incl_

#include <string>
#include <vector>

namespace encfs {
  inline int B64ToB256Bytes(int numB64Bytes) {
    return (numB64Bytes * 6) / 8;
  }

  inline int B32ToB256Bytes(int numB32Bytes) {
    return (numB32Bytes * 5) / 8;
  }

  inline int B256ToB64Bytes(int numB256Bytes) {
    return (numB256Bytes * 8 + 5) / 6;
  }

  inline int B256ToB32Bytes(int numB256Bytes) {
    return (numB256Bytes * 8 + 4) / 5;
  }

  /*
   * convert data between different bases - each being a power of 2.
   */
  void changeBase2(unsigned char* src, int srcLength, int srcPow2,
                   unsigned char* dst, int dstLength, int dstPow2);

  /*
   * same as changeBase2, but writes output over the top op input data
   */
  void changeBase2Inline(unsigned char* buf, int srcLength, int srcPow2,
                         int dst2Pow, bool outputPartialLastByte);

  // inplace translation from values [0, 2^6] ==> base64 ASCII
  void B64ToAscii(unsigned char* buf, int length);
  // inplace transaltion from values [0, 2^5] ==> base32 ASCII
  void B32ToAscii(unsigned char* buf, int length);

  // inplace translation from values base64 ASCII => [0, 2^5]
  void AsciiToB64(unsigned char* buf, int length);
  void AsciiToB64(unsigned char* out, const unsigned char* in, int length);

  // inplace transaltion from values base32 ASCII => [0, 2^5]
  void AsciiToB32(unsigned char* buf, int length);
  void AsciiToB32(unsigned char* out, const unsigned char* in, int length);


  bool B64StandardDecode(unsigned char* out, const unsigned char* in,
                         int inputLen);
  std::string B64StandardEncode(const std::vector<unsigned char>& input);

}

#endif
