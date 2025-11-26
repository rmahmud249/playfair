#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ================================
// DES PERMUTATION AND S-BOX TABLES
// ================================
int DES_IP[64] = {
    58,50,42,34,26,18,10,2,
    60,52,44,36,28,20,12,4,
    62,54,46,38,30,22,14,6,
    64,56,48,40,32,24,16,8,
    57,49,41,33,25,17,9,1,
    59,51,43,35,27,19,11,3,
    61,53,45,37,29,21,13,5,
    63,55,47,39,31,23,15,7
};

int DES_FP[64] = {
    40,8,48,16,56,24,64,32,
    39,7,47,15,55,23,63,31,
    38,6,46,14,54,22,62,30,
    37,5,45,13,53,21,61,29,
    36,4,44,12,52,20,60,28,
    35,3,43,11,51,19,59,27,
    34,2,42,10,50,18,58,26,
    33,1,41,9,49,17,57,25
};

int DES_EXPAND[48] = {
    32,1,2,3,4,5,
    4,5,6,7,8,9,
    8,9,10,11,12,13,
    12,13,14,15,16,17,
    16,17,18,19,20,21,
    20,21,22,23,24,25,
    24,25,26,27,28,29,
    28,29,30,31,32,1
};

int DES_PBOX[32] = {
    16,7,20,21,29,12,28,17,
    1,15,23,26,5,18,31,10,
    2,8,24,14,32,27,3,9,
    19,13,30,6,22,11,4,25
};

int DES_PC1[56] = {
    57,49,41,33,25,17,9,1,
    58,50,42,34,26,18,10,2,
    59,51,43,35,27,19,11,3,
    60,52,44,36,63,55,47,39,
    31,23,15,7,62,54,46,38,
    30,22,14,6,61,53,45,37,
    29,21,13,5,28,20,12,4
};

int DES_PC2[48] = {
    14,17,11,24,1,5,3,28,
    15,6,21,10,23,19,12,4,
    26,8,16,7,27,20,13,2,
    41,52,31,37,47,55,30,40,
    51,45,33,48,44,49,39,56,
    34,53,46,42,50,36,29,32
};

int DES_SHIFTS[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

int DES_SBOX[8][4][16] = {
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
// ===== Helper Functions =====
void permute(const int* input,int* output,const int* table,int size_a){
    for(int i=0;i<size_a;i++){
        output[i]=input[table[i]-1];
    }
}

void rotate_left28bits(int* arr,int n){
    int temp[28];
    for(int i=0;i<28;i++){
        temp[i]=arr[i];
    }
    for(int i=0;i<28;i++){
        arr[i]=temp[(i+n)%28];
    }
}

void xor_bits(const int* a,const int* b,int* res,int size_b){
    for(int i=0;i<size_b;i++){
        res[i]=a[i]^b[i];
    }
}

void ascii_to_bits(const char* ascii,int* bits){
    for(int i=0;i<8;i++){
        unsigned char c=(i<strlen(ascii))?ascii[i]:0;
        for(int j=0;j<8;j++){
            bits[i*8+j]=(c>>(7-j))&1;
        }
    }
}

void bits_to_ascii(const int* bits,char* ascii){
    for(int i=0;i<8;i++){
        ascii[i]=0;
        for(int j=0;j<8;j++){
            ascii[i]|=bits[i*8+j]<<(7-j);
        }
    }
    ascii[8]='\0';
}

void bits_to_hex(const int* bits,char* hex){
    for(int i=0;i<16;i++){
        int val=0;
        for(int j=0;j<4;j++){
            val=(val<<1)|bits[i*4+j];
        }
        hex[i]=(val<=9)?'0'+val:'A'+val-10;
    }
    hex[16]='\0';
}

void hex_to_bits(const char* hex,int* bits){
    for(int i=0;i<16;i++){
        char c = hex[i];
        int val;
        if(c>='0' && c<='9') val=c-'0';
        else if(c>='A' && c<='F'){
            val=c-'A'+10;
        }

        else if(c>='a' && c<='f'){
           val=c-'a'+10;
        }
        else{
          val=0;
        }
        for(int j=0;j<4;j++){
            bits[i*4+j] = (val>>(3-j))&1;
        }
    }
}

// ===== DES Key Schedule and Feistel =====
void generate_des_subkeys(const int* key64,int subkeys[16][48]){
    int key56[56],C[28],D[28];
    permute(key64,key56,DES_PC1,56);
    for(int i=0;i<28;i++){
        C[i]=key56[i];
        D[i]=key56[i+28];
    }
    for(int r=0;r<16;r++){
        rotate_left28bits(C,DES_SHIFTS[r]);
        rotate_left28bits(D,DES_SHIFTS[r]);
        int CD[56];
        for(int i=0;i<28;i++){
            CD[i]=C[i];
            CD[i+28]=D[i];
        }
        permute(CD,subkeys[r],DES_PC2,48);
    }
}

void des_feistel(const int* R32,const int* K48,int* out32){
    int exp48[48],xor48[48],s32[32];
    permute(R32,exp48,DES_EXPAND,48);
    xor_bits(exp48,K48,xor48,48);
    for(int i=0;i<8;i++){
        int b[6];
        for(int j=0;j<6;j++){
            b[j]=xor48[i*6+j];
        }
        int row=(b[0]<<1)|b[5];
        int col=(b[1]<<3)|(b[2]<<2)|(b[3]<<1)|b[4];
        int val=DES_SBOX[i][row][col];
        for(int j=0;j<4;j++){
            s32[i*4+j]=(val>>(3-j))&1;
        }
    }
    permute(s32,out32,DES_PBOX,32);
}

void des_encrypt(const int* plain64,const int* key64,int* cipher64){
    int subkeys[16][48],perm64[64],L[32],R[32];
    generate_des_subkeys(key64,subkeys);
    permute(plain64,perm64,DES_IP,64);
    for(int i=0;i<32;i++){
        L[i]=perm64[i];
        R[i]=perm64[i+32];
    }
    for(int r=0;r<16;r++){
        int tempR[32],f32[32];
        for(int i=0;i<32;i++){
                tempR[i]=R[i];
        }
        des_feistel(R,subkeys[r],f32);
        xor_bits(L,f32,R,32);
        for(int i=0;i<32;i++){
                L[i]=tempR[i];
         }
    }
    int combined[64];
    for(int i=0;i<32;i++){
        combined[i]=R[i];
        combined[i+32]=L[i];
    }
    permute(combined,cipher64,DES_FP,64);
}

void des_decrypt(const int* cipher64,const int* key64,int* plain64){
    int subkeys[16][48],perm64[64],L[32],R[32];
    generate_des_subkeys(key64,subkeys);
    permute(cipher64,perm64,DES_IP,64);
    for(int i=0;i<32;i++){
        L[i]=perm64[i];
        R[i]=perm64[i+32];
    }
    for(int r=15;r>=0;r--){
        int tempR[32],f32[32];
        for(int i=0;i<32;i++){
                tempR[i]=R[i];
        }
        des_feistel(R,subkeys[r],f32);
        xor_bits(L,f32,R,32);
        for(int i=0;i<32;i++){
                L[i]=tempR[i];
        }
    }
    int combined[64];
    for(int i=0;i<32;i++){
        combined[i]=R[i];
        combined[i+32]=L[i];
    }
    permute(combined,plain64,DES_FP,64);
}

// ===== MAIN =====
int main(){
    char input[1024], key_input[9], output_hex[17];
    int input_bits[64], key_bits[64], output_bits[64];

    printf("=== DES ENCRYPTION DECRYPTION ===\n\n");
    printf("Enter 8-character key: ");
    scanf("%8s", key_input);

    ascii_to_bits(key_input, key_bits);

    int op;
    printf("Choose operation: 1-Encrypt, 2-Decrypt: ");
    scanf("%d", &op);

    if(op==1){
        printf("Enter plain text: ");
        scanf("%8s", input);
        ascii_to_bits(input, input_bits);
        des_encrypt(input_bits, key_bits, output_bits);
        bits_to_hex(output_bits, output_hex);
        printf("Cipher text: %s\n", output_hex);
    }
    else if(op==2){
        printf("Enter cipher text: ");
        scanf("%16s", input);
        hex_to_bits(input, input_bits);
        des_decrypt(input_bits, key_bits, output_bits);
        char decrypted_text[9];
        bits_to_ascii(output_bits, decrypted_text);
        printf("Decrypted text: %s\n", decrypted_text);
    }
    else{
        printf("Invalid operation!\n");
        return 1;
    }

    return 0;
}

