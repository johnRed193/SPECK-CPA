#include "gost-magma.h"
#include <string.h>

static kuz_key_t ctx;

// Стандартные S-блоки ГОСТ 28147-89
static const uint8_t SBOX[8][16] = {
    {12, 4, 6, 2, 10, 5, 11, 9, 14, 8, 13, 7, 0, 3, 15, 1},
    {6, 8, 2, 3, 9, 10, 5, 12, 1, 14, 4, 7, 11, 13, 0, 15},
    {11, 3, 5, 8, 2, 15, 10, 13, 14, 1, 7, 4, 12, 9, 6, 0},
    {12, 8, 2, 1, 13, 4, 15, 6, 7, 0, 10, 5, 3, 14, 9, 11},
    {7, 15, 5, 10, 8, 1, 6, 13, 0, 9, 3, 14, 11, 4, 2, 12},
    {5, 13, 15, 6, 9, 2, 12, 10, 11, 7, 8, 1, 4, 3, 14, 0},
    {8, 14, 2, 5, 6, 9, 1, 12, 15, 4, 11, 0, 13, 10, 3, 7},
    {1, 7, 14, 13, 0, 5, 8, 3, 4, 15, 10, 6, 9, 12, 11, 2}
};

// Быстрое преобразование F
static inline uint32_t magma_F(uint32_t data, uint32_t key) {
    uint32_t x = data + key;
    uint32_t result = 0;

    // Применение S-блоков
    for (int i = 0; i < 8; i++) {
        uint8_t nibble = (x >> (4 * i)) & 0xF;
        result |= (uint32_t)SBOX[i][nibble] << (4 * i);
    }

    // Циклический сдвиг на 11 бит влево
    return (result << 11) | (result >> (32 - 11));
}

void GOST_ECB_magma_setkey(uint8_t* key) {
    // Генерация раундовых ключей (32 раунда)
    for (int i = 0; i < 8; i++) {
        ctx.k[i] = ((uint32_t)key[4 * i] << 24) |
            ((uint32_t)key[4 * i + 1] << 16) |
            ((uint32_t)key[4 * i + 2] << 8) |
            (uint32_t)key[4 * i + 3];
    }

    // Повторяем ключи 3 раза (24 раунда)
    for (int i = 8; i < 24; i++) {
        ctx.k[i] = ctx.k[i % 8];
    }

    // Обратный порядок для последних 8 раундов
    for (int i = 0; i < 8; i++) {
        ctx.k[24 + i] = ctx.k[7 - i];
    }
}

void GOST_ECB_magma_crypto(uint8_t* block) {
    uint32_t left, right, temp;

    // Разделение блока на две части
    left = ((uint32_t)block[0] << 24) | ((uint32_t)block[1] << 16) |
        ((uint32_t)block[2] << 8) | block[3];
    right = ((uint32_t)block[4] << 24) | ((uint32_t)block[5] << 16) |
        ((uint32_t)block[6] << 8) | block[7];

    // 31 раунд преобразований
    for (int i = 0; i < 31; i++) {
        temp = right;
        right = left ^ magma_F(right, ctx.k[i]);
        left = temp;
    }

    // Финальный раунд (без перестановки)
    left ^= magma_F(right, ctx.k[31]);

    // Сборка результата
    block[0] = (left >> 24) & 0xFF;
    block[1] = (left >> 16) & 0xFF;
    block[2] = (left >> 8) & 0xFF;
    block[3] = left & 0xFF;
    block[4] = (right >> 24) & 0xFF;
    block[5] = (right >> 16) & 0xFF;
    block[6] = (right >> 8) & 0xFF;
    block[7] = right & 0xFF;
}