#include "gost-independant.h"
#include "hal.h"
#include "gost.h"

uint8_t enckey[32];

void gost_indep_init(void)
{
  ;
}

void gost_indep_key(uint8_t * key)
{
  GOST_ECB_indp_setkey(key);
}

void gost_indep_enc(uint8_t * pt)
{
  GOST_ECB_indp_crypto(pt);
}

void gost_indep_enc_pretrigger(uint8_t * pt)
{
    ;
}

void gost_indep_enc_posttrigger(uint8_t * pt)
{
    ;
}

void gost_indep_mask(uint8_t * m, uint8_t len)
{
}

