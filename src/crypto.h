#ifndef CRYPTO_H
#define CRYPTO_H

#include <windows.h>

void MD4(const unsigned char* data, size_t len, unsigned char* hash);
void sam_encrypt_hash(const unsigned char* hash, DWORD rid, unsigned char* out);

#endif // CRYPTO_H
