#include "gost-magma-independant.h"
#include "gost-magma.h"
#include "hal.h"

uint8_t enckey[32];

void gost_magma_init(void) {
    ;
}

void gost_magma_key(uint8_t* key) {
    GOST_ECB_magma_setkey(key);
}

void gost_magma_enc(uint8_t* block) {
    GOST_ECB_magma_crypto(block);
}

void gost_magma_enc_pretrigger(uint8_t* block) {
    ;
}

void gost_magma_enc_posttrigger(uint8_t* block) {
    ;
}

void gost_magma_mask(uint8_t* m, uint8_t len) {
}