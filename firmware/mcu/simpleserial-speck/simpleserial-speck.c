#include "hal.h"
#include "simpleserial.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h> // Добавляем для memcpy

// --- Реализация SPECK 64/128 ---

#define ROR(x, r) ((x >> r) | (x << (32 - r)))
#define ROL(x, r) ((x << r) | (x >> (32 - r)))
#define R(x, y, k) (x = ROR(x, 8), x += y, x ^= k, y = ROL(y, 3), y ^= x)

void speck_encrypt(uint32_t pt[2], uint32_t ct[2], uint32_t key[4])
{
    uint32_t b = key[0];
    uint32_t a[3];
    a[0] = key[1];
    a[1] = key[2];
    a[2] = key[3];

    ct[0] = pt[0];
    ct[1] = pt[1];

    for (int i = 0; i < 27; i++) {
        R(ct[1], ct[0], b);
        R(a[i % 3], b, i);
    }
}

// Добавляем volatile, чтобы компилятор не оптимизировал доступ к ключу
volatile uint32_t stored_key[4] = {0};

// ИСПРАВЛЕННЫЙ обработчик записи ключа
uint8_t set_key(uint8_t* k, uint8_t len)
{
    // Безопасный способ: собираем байты вручную (Little Endian)
    // Это предотвращает HardFault из-за невыровненного доступа
    stored_key[0] = (uint32_t)k[0] | ((uint32_t)k[1] << 8) | ((uint32_t)k[2] << 16) | ((uint32_t)k[3] << 24);
    stored_key[1] = (uint32_t)k[4] | ((uint32_t)k[5] << 8) | ((uint32_t)k[6] << 16) | ((uint32_t)k[7] << 24);
    stored_key[2] = (uint32_t)k[8] | ((uint32_t)k[9] << 8) | ((uint32_t)k[10] << 16) | ((uint32_t)k[11] << 24);
    stored_key[3] = (uint32_t)k[12] | ((uint32_t)k[13] << 8) | ((uint32_t)k[14] << 16) | ((uint32_t)k[15] << 24);
    
    return 0x00;
}

// ИСПРАВЛЕННЫЙ обработчик шифрования
uint8_t get_pt(uint8_t* pt, uint8_t len)
{
    uint32_t pt32[2];
    uint32_t ct32[2];
    
    // Безопасное чтение входных данных (избегаем cast uint32*)
    pt32[0] = (uint32_t)pt[0] | ((uint32_t)pt[1] << 8) | ((uint32_t)pt[2] << 16) | ((uint32_t)pt[3] << 24);
    pt32[1] = (uint32_t)pt[4] | ((uint32_t)pt[5] << 8) | ((uint32_t)pt[6] << 16) | ((uint32_t)pt[7] << 24);

    // Копируем ключ в локальный массив (убираем volatile для скорости внутри функции)
    uint32_t key_local[4];
    key_local[0] = stored_key[0];
    key_local[1] = stored_key[1];
    key_local[2] = stored_key[2];
    key_local[3] = stored_key[3];

    // --- ТРИГГЕР ВВЕРХ ---
    trigger_high();
    
    // Шифруем
    speck_encrypt(pt32, ct32, key_local);
    
    // --- ТРИГГЕР ВНИЗ ---
    trigger_low();

    // Записываем результат обратно в буфер (Little Endian)
    // 1-е слово
    pt[0] = ct32[0] & 0xFF;
    pt[1] = (ct32[0] >> 8) & 0xFF;
    pt[2] = (ct32[0] >> 16) & 0xFF;
    pt[3] = (ct32[0] >> 24) & 0xFF;
    // 2-е слово
    pt[4] = ct32[1] & 0xFF;
    pt[5] = (ct32[1] >> 8) & 0xFF;
    pt[6] = (ct32[1] >> 16) & 0xFF;
    pt[7] = (ct32[1] >> 24) & 0xFF;

    simpleserial_put('r', 16, pt);
    return 0x00;
}

uint8_t reset(uint8_t* x, uint8_t len) { return 0x00; }

int main(void)
{
    platform_init();
    init_uart();
    trigger_setup();
    simpleserial_init();

    simpleserial_addcmd('k', 16, set_key);
    simpleserial_addcmd('p', 16, get_pt);
    simpleserial_addcmd('x', 0, reset);

    while(1) simpleserial_get();
}