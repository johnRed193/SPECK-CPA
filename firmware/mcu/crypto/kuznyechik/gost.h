#ifndef _GOST_H_
#define _GOST_H_

#include <stddef.h>
#include <stdint.h>

#define GOST_BLOCKLEN 16       // Block length in bytes

#define GOST_KEYLEN 32         // Key length in bytes
#define GOST_ROUND_KEYLEN 16   // Round key length in bytes

#define Nr 10 // The number of rounds in GOST Cipher.

#define GOST_keyExpSize 160

// my 128-bit datatype
typedef union {	
    uint64_t q[2];
    uint8_t  b[GOST_BLOCKLEN];
} w128_t;

// cipher context
typedef struct {
	w128_t k[Nr]; // round keys
} kuz_key_t;

void GOST_ECB_indp_setkey(uint8_t* key);
void GOST_ECB_indp_crypto(uint8_t* input);

#endif // _GOST_H_
