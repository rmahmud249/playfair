#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
char round_keys[16][49]; /* 48 bits + null */

/* Plaintext buffer (64 bits as chars '0'/'1') */
char pt[65] = {0};

/* Utility: convert decimal 0..15 to 4-bit binary string (out must be >=5 bytes) */
void dec_to_bin4(int decimal, char *out) {
    out[4] = '\0';
    for (int i = 3; i >= 0; --i) {
        out[i] = (decimal & 1) ? '1' : '0';
        decimal >>= 1;
    }
}

/* Utility: convert a binary string of length n to decimal */
int bin_to_dec(const char *bin, int n) {
    int val = 0;
    for (int i = 0; i < n; ++i) {
        val = (val << 1) + (bin[i] - '0');
    }
    return val;
}

/* XOR two bit-strings (a XOR b) of length len; result stored in out (len+1 bytes) */
void xor_bits(const char *a, const char *b, char *out, int len) {
    for (int i = 0; i < len; ++i) {
        out[i] = (a[i] != b[i]) ? '1' : '0';
    }
    out[len] = '\0';
}

/* Circular left shift once (len is number of bits; out must have len+1) */
void shift_left_once(const char *in, char *out, int len) {
    for (int i = 0; i < len - 1; ++i) out[i] = in[i + 1];
    out[len - 1] = in[0];
    out[len] = '\0';
}

/* Circular left shift twice */
void shift_left_twice(const char *in, char *out, int len) {
    char tmp[64];
    shift_left_once(in, tmp, len);
    shift_left_once(tmp, out, len);
}

/* substring: copy src[start..start+len-1] into dest (dest must have len+1) */
void substr(const char *src, int start, int len, char *dest) {
    memcpy(dest, src + start, len);
    dest[len] = '\0';
}

/* Generate 16 round keys from 64-bit key (as 64-char '0'/'1' string) */
void generate_keys(const char *key) {
    int pc1[56] = {
        57,49,41,33,25,17,9,
        1,58,50,42,34,26,18,
        10,2,59,51,43,35,27,
        19,11,3,60,52,44,36,
        63,55,47,39,31,23,15,
        7,62,54,46,38,30,22,
        14,6,61,53,45,37,29,
        21,13,5,28,20,12,4
    };
    int pc2[48] = {
        14,17,11,24,1,5,
        3,28,15,6,21,10,
        23,19,12,4,26,8,
        16,7,27,20,13,2,
        41,52,31,37,47,55,
        30,40,51,45,33,48,
        44,49,39,56,34,53,
        46,42,50,36,29,32
    };

    char perm_key[57]; perm_key[56] = '\0';
    for (int i = 0; i < 56; ++i) perm_key[i] = key[pc1[i] - 1];

    char left[29], right[29];
    substr(perm_key, 0, 28, left);
    substr(perm_key, 28, 28, right);

    for (int round = 0; round < 16; ++round) {
        /* rounds 1,2,9,16 (indexes 0,1,8,15) -> shift by 1 */
        char new_left[29], new_right[29];
        if (round == 0 || round == 1 || round == 8 || round == 15) {
            shift_left_once(left, new_left, 28);
            shift_left_once(right, new_right, 28);
        } else {
            shift_left_twice(left, new_left, 28);
            shift_left_twice(right, new_right, 28);
        }
        strcpy(left, new_left);
        strcpy(right, new_right);

        /* combine and apply PC2 */
        char combined[57]; combined[56] = '\0';
        strcpy(combined, left);
        strcat(combined, right);

        char round_key[49]; round_key[48] = '\0';
        for (int i = 0; i < 48; ++i) round_key[i] = combined[pc2[i] - 1];

        strcpy(round_keys[round], round_key);
        /* optional: print key */
        /* printf("Key %2d: %s\n", round + 1, round_keys[round]); */
    }
}

/* The DES function: uses global round_keys and global pt, returns ciphertext in out (must be 65 bytes) */
void DES_encrypt_block(const char *plain, char *out_cipher) {
    int initial_permutation[64] = {
        58,50,42,34,26,18,10,2,
        60,52,44,36,28,20,12,4,
        62,54,46,38,30,22,14,6,
        64,56,48,40,32,24,16,8,
        57,49,41,33,25,17,9,1,
        59,51,43,35,27,19,11,3,
        61,53,45,37,29,21,13,5,
        63,55,47,39,31,23,15,7
    };
    int expansion_table[48] = {
        32,1,2,3,4,5,4,5,
        6,7,8,9,8,9,10,11,
        12,13,12,13,14,15,16,17,
        16,17,18,19,20,21,20,21,
        22,23,24,25,24,25,26,27,
        28,29,28,29,30,31,32,1
    };
    int permutation_tab[32] = {
        16,7,20,21,29,12,28,17,
        1,15,23,26,5,18,31,10,
        2,8,24,14,32,27,3,9,
        19,13,30,6,22,11,4,25
    };
    int inverse_permutation[64] = {
        40,8,48,16,56,24,64,32,
        39,7,47,15,55,23,63,31,
        38,6,46,14,54,22,62,30,
        37,5,45,13,53,21,61,29,
        36,4,44,12,52,20,60,28,
        35,3,43,11,51,19,59,27,
        34,2,42,10,50,18,58,26,
        33,1,41,9,49,17,57,25
    };

    /* S-boxes */
    static const int sbox[8][4][16] = {
        {
            {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
            {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
            {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
            {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
        },
        {
            {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
            {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
            {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
            {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
        },
        {
            {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
            {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
            {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
            {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
        },
        {
            {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
            {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
            {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
            {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
        },
        {
            {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
            {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
            {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
            {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
        },
        {
            {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
            {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
            {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
            {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
        },
        {
            {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
            {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
            {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
            {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
        },
        {
            {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
            {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
            {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
            {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
        }
    };

    /* 1. Initial permutation */
    char perm[65]; perm[64] = '\0';
    for (int i = 0; i < 64; ++i) perm[i] = plain[initial_permutation[i] - 1];

    /* 2. Split */
    char L[33], R[33];
    substr(perm, 0, 32, L);
    substr(perm, 32, 32, R);

    /* 16 rounds */
    for (int round = 0; round < 16; ++round) {
        char R_expanded[49]; R_expanded[48] = '\0';
        for (int i = 0; i < 48; ++i) R_expanded[i] = R[expansion_table[i] - 1];

        char xored[49]; xor_bits(round_keys[round], R_expanded, xored, 48);

        /* S-box substitution: process 8 blocks of 6 bits -> 4 bits each */
        char s_out[33]; s_out[32] = '\0';
        char six_bits[7]; six_bits[6] = '\0';
        char four_bits[5];
        for (int box = 0; box < 8; ++box) {
            substr(xored, box * 6, 6, six_bits);
            char row_bits[3]; row_bits[0] = six_bits[0]; row_bits[1] = six_bits[5]; row_bits[2] = '\0';
            char col_bits[5]; memcpy(col_bits, six_bits + 1, 4); col_bits[4] = '\0';
            int row = bin_to_dec(row_bits, 2);
            int col = bin_to_dec(col_bits, 4);
            int val = sbox[box][row][col];
            dec_to_bin4(val, four_bits);
            memcpy(s_out + box * 4, four_bits, 4);
        }
        s_out[32] = '\0';

        /* permutation P */
        char perm2[33]; perm2[32] = '\0';
        for (int i = 0; i < 32; ++i) perm2[i] = s_out[permutation_tab[i] - 1];

        /* XOR with left */
        char newR[33]; xor_bits(perm2, L, newR, 32);
        newR[32] = '\0';

        /* swap for next round */
        strcpy(L, R);
        strcpy(R, newR);
    }

    /* combine (note: final swap already performed by rounds) */
    char combined[65];
    strcpy(combined, L);
    strcat(combined, R);

    /* inverse initial permutation */
    for (int i = 0; i < 64; ++i) out_cipher[i] = combined[inverse_permutation[i] - 1];
    out_cipher[64] = '\0';
}

int main(void) {
    /* 64-bit key and plaintext (as bit-strings) */
    const char *key = "1010101010111011000010010001100000100111001101101100110011011101";
    const char *plain = "1010101111001101111001101010101111001101000100110010010100110110";

    /* copy plaintext to global pt if needed by other functions */
    strncpy(pt, plain, 64);
    pt[64] = '\0';

    /* generate round keys */
    generate_keys(key);

    printf("Plain text: %s\n", plain);

    /* encrypt */
    char cipher[65];
    DES_encrypt_block(plain, cipher);
    printf("Ciphertext: %s\n", cipher);

    /* reverse round keys for decryption */
    for (int i = 0, j = 15; i < j; ++i, --j) {
        char tmp[49];
        strcpy(tmp, round_keys[i]);
        strcpy(round_keys[i], round_keys[j]);
        strcpy(round_keys[j], tmp);
    }

    /* decrypt (encrypt with reversed keys) */
    char decrypted[65];
    DES_encrypt_block(cipher, decrypted);
    printf("Decrypted text: %s\n", decrypted);

    if (strcmp(decrypted, plain) == 0) {
        printf("Plain text encrypted and decrypted successfully.\n");
    } else {
        printf("Mismatch after decryption.\n");
    }

    return 0;
}
