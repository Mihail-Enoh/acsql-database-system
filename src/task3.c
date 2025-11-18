#include "task3.h"

// Funcție pentru extinderea unui buffer (IV sau key) la o anumită dimensiune
void extend_buffer(uint8_t *dest, const uint8_t *src, size_t src_len, size_t dest_len) {
    for (size_t i = 0; i < dest_len; i++) {
        dest[i] = src[i % src_len];
    }
}

// Funcție XOR între două blocuri
void xor_blocks(uint8_t *result, const uint8_t *block1, const uint8_t *block2, size_t len) {
    for (size_t i = 0; i < len; i++) {
        result[i] = block1[i] ^ block2[i];
    }
}

// S-box: XOR cu cheia extinsă
void apply_sbox(uint8_t *block, const uint8_t *key, size_t key_len, size_t block_size) {
    uint8_t *extended_key = (uint8_t *)malloc(block_size);
    extend_buffer(extended_key, key, key_len, block_size);

    for (size_t i = 0; i < block_size; i++) {
        block[i] ^= extended_key[i];
    }

    free(extended_key);
}

// P-box: Permutare folosind formula f(i) = (i * (n-1) + 2) mod n
void apply_pbox(uint8_t *block, size_t block_size) {
    uint8_t *temp = (uint8_t *)malloc(block_size);
    memcpy(temp, block, block_size);

    // Pentru fiecare poziție j din output, găsim i pentru care f(i) = j
    // f(i) = (i * (n-1) + 2) mod n = j
    for (size_t i = 0; i < block_size; i++) {
        size_t new_pos = (i * (block_size - 1) + 2) % block_size;
        block[new_pos] = temp[i];
    }

    free(temp);
}

void cripteaza_studenti(secretariat *secretariat, void *key, size_t key_len,
                        void *iv, size_t iv_len, char *cale_output) {
    // TODO(student): Cripteaza vectorul secretariat->studenti si scrie rezultatul in fisierul cale_output
    // Calculăm dimensiunea totală a datelor (vectorul de studenți)
    size_t total_size = secretariat->nr_studenti * sizeof(student);  // Calculăm dimensiunea (împărțim în 4 blocuri)
    size_t block_size = (total_size + 3) / 4;  // Rotunjim în sus

    // Calculăm dimensiunea totală cu padding
    size_t padded_size = block_size * 4;

    // Alocăm memorie pentru datele cu padding
    uint8_t *data = (uint8_t *)calloc(padded_size, 1);  // calloc inițializează cu 0

    // Copiem datele studenților
    memcpy(data, secretariat->studenti, total_size);

    // Alocăm memorie pentru blocurile criptate
    uint8_t *encrypted_data = (uint8_t *)malloc(padded_size);

    // Pregătim IV-ul extins
    uint8_t *extended_iv = (uint8_t *)malloc(block_size);
    extend_buffer(extended_iv, (uint8_t *)iv, iv_len, block_size);

    // Criptăm primul bloc
    uint8_t *block = (uint8_t *)malloc(block_size);

    // Blocul 0: XOR cu IV
    xor_blocks(block, data, extended_iv, block_size);

    // Aplicăm S-box
    apply_sbox(block, (uint8_t *)key, key_len, block_size);

    // Aplicăm P-box
    apply_pbox(block, block_size);

    // Salvăm blocul criptat
    memcpy(encrypted_data, block, block_size);

    // Criptăm blocurile 1, 2, 3
    for (int i = 1; i < 4; i++) {
        // XOR între blocul curent și blocul precedent criptat
        xor_blocks(block, data + i * block_size, encrypted_data + (i - 1) * block_size, block_size);

        // Aplicăm S-box
        apply_sbox(block, (uint8_t *)key, key_len, block_size);

        // Aplicăm P-box
        apply_pbox(block, block_size);

        // Salvăm blocul criptat
        memcpy(encrypted_data + i * block_size, block, block_size);
    }

    // Scriem în fișier
    FILE *f = fopen(cale_output, "wb");
    if (f != NULL) {
        fwrite(encrypted_data, 1, padded_size, f);
        fclose(f);
    }

    // Eliberăm memoria
    free(data);
    free(encrypted_data);
    free(extended_iv);
    free(block);
}
