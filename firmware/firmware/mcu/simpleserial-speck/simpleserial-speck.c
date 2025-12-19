
#include "hal.h"
#include "simpleserial.h"
#include <stdint.h>
#include <stdlib.h>

// SPECK 64/128 Implementation
#define ROR(x, r) ((x >> r) | (x << (32 - r)))
#define ROL(x, r) ((x << r) | (x >> (32 - r)))
#define R(x, y, k) (x = ROR(x, 8), x += y, x ^= k, y = ROL(y, 3), y ^= x)

void speck_encrypt(uint32_t pt[2], uint32_t ct[2], uint32_t key[4]) {
    uint32_t b = key[0];
    uint32_t a[3] = {key[1], key[2], key[3]};
    ct[0] = pt[0]; ct[1] = pt[1];
    for (int i = 0; i < 27; i++) {
        R(ct[1], ct[0], b);
        R(a[i % 3], b, i);
    }
}

uint32_t stored_key[4] = {0};

uint8_t set_key(uint8_t* k, uint8_t len) {
    // Копируем побайтово, чтобы избежать проблем с указателями
    uint8_t* sk_bytes = (uint8_t*)stored_key;
    for(int i=0; i<16; i++) {
        sk_bytes[i] = k[i];
    }
    return 0x00;
}

// КОМАНДА 'g': Вернуть текущий ключ
uint8_t get_key_debug(uint8_t* x, uint8_t len) {
    simpleserial_put('r', 16, (uint8_t*)stored_key);
    return 0x00;
}

uint8_t get_pt(uint8_t* pt, uint8_t len) {
    uint32_t pt32[2] = {((uint32_t*)pt)[0], ((uint32_t*)pt)[1]};
    uint32_t ct32[2];
    trigger_high();
    speck_encrypt(pt32, ct32, stored_key);
    trigger_low();
    ((uint32_t*)pt)[0] = ct32[0];
    ((uint32_t*)pt)[1] = ct32[1];
    simpleserial_put('r', 16, pt);
    return 0x00;
}

uint8_t reset(uint8_t* x, uint8_t len) { return 0x00; }

int main(void) {
    platform_init();
    init_uart();
    trigger_setup();
    simpleserial_init();
    simpleserial_addcmd('k', 16, set_key);
    simpleserial_addcmd('p', 16, get_pt);
    simpleserial_addcmd('g', 0, get_key_debug); // DEBUG COMMAND
    simpleserial_addcmd('x', 0, reset);
    while(1) simpleserial_get();
}
