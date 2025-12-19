#ifndef GOST_MAGMA_H
#define GOST_MAGMA_H

#include <stdint.h>
#include <stddef.h>

#define MAGMA_BLOCKLEN 8      // Block length in bytes

#define MAGMA_KEYLEN 32         // Key length in bytes
#define MAGMA_ROUND_KEYLEN 4   // Round key length in bytes

#define Nr 32 // The number of rounds in GOST Cipher.

#define MAGMA_keyExpSize 128

// my 128-bit datatype
typedef union {
    uint64_t q[2];
    uint8_t  b[MAGMA_BLOCKLEN];
} w128_t;

// cipher context
typedef struct {
    uint32_t k[Nr]; // round keys
} kuz_key_t;

void GOST_ECB_magma_setkey(uint8_t* key);
void GOST_ECB_magma_crypto(uint8_t* input);

#endif // _GOST_H_