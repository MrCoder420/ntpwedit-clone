#include "crypto.h"
#include <string.h>

// MD4 transformation constants
#define S11 3
#define S12 7
#define S13 11
#define S14 19
#define S21 3
#define S22 5
#define S23 9
#define S24 13
#define S31 3
#define S32 9
#define S33 11
#define S34 15

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

#define FF(a, b, c, d, x, s) { \
    (a) += F ((b), (c), (d)) + (x); \
    (a) = ROTATE_LEFT ((a), (s)); \
  }
#define GG(a, b, c, d, x, s) { \
    (a) += G ((b), (c), (d)) + (x) + (unsigned long)0x5a827999; \
    (a) = ROTATE_LEFT ((a), (s)); \
  }
#define HH(a, b, c, d, x, s) { \
    (a) += H ((b), (c), (d)) + (x) + (unsigned long)0x6ed9eba1; \
    (a) = ROTATE_LEFT ((a), (s)); \
  }

void MD4_Transform(unsigned long state[4], const unsigned char block[64]) {
    unsigned long a = state[0], b = state[1], c = state[2], d = state[3], x[16];
    for (int i = 0, j = 0; i < 16; i++, j += 4)
        x[i] = ((unsigned long)block[j]) | (((unsigned long)block[j + 1]) << 8) |
        (((unsigned long)block[j + 2]) << 16) | (((unsigned long)block[j + 3]) << 24);

    /* Round 1 */
    FF(a, b, c, d, x[0], S11); FF(d, a, b, c, x[1], S12); FF(c, d, a, b, x[2], S13); FF(b, c, d, a, x[3], S14);
    FF(a, b, c, d, x[4], S11); FF(d, a, b, c, x[5], S12); FF(c, d, a, b, x[6], S13); FF(b, c, d, a, x[7], S14);
    FF(a, b, c, d, x[8], S11); FF(d, a, b, c, x[9], S12); FF(c, d, a, b, x[10], S13); FF(b, c, d, a, x[11], S14);
    FF(a, b, c, d, x[12], S11); FF(d, a, b, c, x[13], S12); FF(c, d, a, b, x[14], S13); FF(b, c, d, a, x[15], S14);

    /* Round 2 */
    GG(a, b, c, d, x[0], S21); GG(d, a, b, c, x[4], S22); GG(c, d, a, b, x[8], S23); GG(b, c, d, a, x[12], S24);
    GG(a, b, c, d, x[1], S21); GG(d, a, b, c, x[5], S22); GG(c, d, a, b, x[9], S23); GG(b, c, d, a, x[13], S24);
    GG(a, b, c, d, x[2], S21); GG(d, a, b, c, x[6], S22); GG(c, d, a, b, x[10], S23); GG(b, c, d, a, x[14], S24);
    GG(a, b, c, d, x[3], S21); GG(d, a, b, c, x[7], S22); GG(c, d, a, b, x[11], S23); GG(b, c, d, a, x[15], S24);

    /* Round 3 */
    HH(a, b, c, d, x[0], S31); HH(d, a, b, c, x[8], S32); HH(c, d, a, b, x[4], S33); HH(b, c, d, a, x[12], S34);
    HH(a, b, c, d, x[2], S31); HH(d, a, b, c, x[10], S32); HH(c, d, a, b, x[6], S33); HH(b, c, d, a, x[14], S34);
    HH(a, b, c, d, x[1], S31); HH(d, a, b, c, x[9], S32); HH(c, d, a, b, x[5], S33); HH(b, c, d, a, x[13], S34);
    HH(a, b, c, d, x[3], S31); HH(d, a, b, c, x[11], S32); HH(c, d, a, b, x[7], S33); HH(b, c, d, a, x[15], S34);

    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
}

void MD4(const unsigned char* data, size_t len, unsigned char* hash) {
    unsigned long state[4] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476 };
    unsigned char block[64];
    size_t i;
    
    // Simple one-block pad (works for most passwords)
    memset(block, 0, 64);
    memcpy(block, data, len > 56 ? 56 : len);
    block[len] = 0x80;
    
    unsigned long long bitlen = (unsigned long long)len * 8;
    memcpy(block + 56, &bitlen, 8);
    
    MD4_Transform(state, block);
    
    for (i = 0; i < 4; i++) {
        hash[i * 4] = (unsigned char)(state[i] & 0xFF);
        hash[i * 4 + 1] = (unsigned char)((state[i] >> 8) & 0xFF);
        hash[i * 4 + 2] = (unsigned char)((state[i] >> 16) & 0xFF);
        hash[i * 4 + 3] = (unsigned char)((state[i] >> 24) & 0xFF);
    }
}

void nt_hash(const std::wstring& password, unsigned char* out) {
    size_t len = password.length() * 2;
    MD4((const unsigned char*)password.c_str(), len, out);
}
