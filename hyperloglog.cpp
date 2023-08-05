#include <math.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <string>

using namespace std;

size_t least_significant_set_bit(unsigned char *arr, size_t size) {
  size_t ix = 0;
  for (auto i = size - 1; i >= 0; --i) {
    unsigned char a = arr[i];
    for (auto j = 0; j < 8; ++j) {
      if (a & 0x1)
        return ix;
      a >>= 1;
      ++ix;
    }
  }
  return ix;
}

class HyperLogLog {
private:
  unsigned char *M;
  size_t m;
  const unsigned int b;

public:
  HyperLogLog(size_t m) : m(m), b(log2(m)) {
    M = new unsigned char[m];
    for (auto i = 0; i < m; ++i) {
      M[i] = 0;
    }
  }

  bool add(const unsigned char *v, size_t size) {
    unsigned char x[SHA_DIGEST_LENGTH];
    SHA1(v, size, x);
    size_t j = 0;
    int a = b;
    size_t i = 0;
    while (a >= 8) {
      j <<= 8;
      j |= x[i++];
      a -= 8;
    }
    j <<= a;
    j |= x[i] >> (8 - a);

    M[j] = max(size_t(M[j]), least_significant_set_bit(x, SHA_DIGEST_LENGTH));

    return false;
  }

  double count() {
    double den = 0.0;
    for (auto i = 0; i < m; ++i) {
      den += 1.0 / double(M[i]);
      printf("%d\t", M[i]);
    }
    printf("\n");
    return pow(2.0, double(m) / den);
  }
};

int main() {
  srand(time(nullptr));
  unsigned char s[sizeof(int)];
  HyperLogLog hll(4);
  for (auto i = 0; i < 0xffffffff; ++i) {
    int r = rand();
    s[0] = (unsigned char)(r >> 24);
    s[1] = (unsigned char)(r >> 16);
    s[2] = (unsigned char)(r >> 8);
    s[3] = (unsigned char)(r);
    hll.add(s, sizeof(s));
  }
  printf("Finished: %f\n", hll.count());
}