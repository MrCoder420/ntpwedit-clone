#ifndef CRYPTO_H
#define CRYPTO_H

#include <windows.h>
#include <string>

void MD4(const unsigned char* data, size_t len, unsigned char* hash);
void nt_hash(const std::wstring& password, unsigned char* out);
void sam_encrypt_hash(const unsigned char* hash, DWORD rid, unsigned char* out);

#endif // CRYPTO_H
