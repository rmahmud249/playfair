#include <stdio.h>
//============Convert Lower Case To Upper Case========
char toUpper(char ch) {
    if (ch >= 'a' && ch <= 'z')
        return ch - 32;
    return ch;
}
//============Check Alphabet=================
int isAlpha(char ch) {
    if ((ch >= 'A' && ch <= 'Z'))
        return 1;
    if ((ch >= 'a' && ch <= 'z'))
        return 1;
    return 0;
}
//==================Generate Key Table===================
void generateKeyTable(char key[], char keyTable[5][5]){
    int i, j, k = 0;
    char used[26] = {0};
    char current;

    for (i = 0; key[i] != '\0'; i++) {
        current = toUpper(key[i]);
        if (current == 'J') current = 'I';
        if (isAlpha(current) && !used[current - 'A']) {
            keyTable[k / 5][k % 5] = current;
            used[current - 'A'] = 1;
            k++;
        }
    }
    for (i = 0; i < 26; i++) {
        if (i + 'A' == 'J')
            continue;
        if (!used[i]) {
            keyTable[k / 5][k % 5] = i + 'A';
            k++;
        }
    }
}
//================Print Key Table===============
void printKeyTable(char keyTable[5][5]) {
    int i, j;
    printf("\nPlay fair Key Table:\n");
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 5; j++)
            printf("%c ", keyTable[i][j]);
        printf("\n");
    }
}
//==============Search Place for Alphabet===================
void findPosition(char keyTable[5][5], char ch, int *row, int *col){
    int i, j;
    if (ch == 'J') ch = 'I';
    for (i = 0; i < 5; i++)
        for (j = 0; j < 5; j++)
            if (keyTable[i][j] == ch) {
                *row = i;
                *col = j;
                return;
    }
}
//=================Prepare Plain Text===============
void prepareText(char text[], char prepared[]){
    int i, j = 0;
    for (i = 0; text[i] != '\0'; i++)
        if (isAlpha(text[i]))
            prepared[j++] = toUpper(text[i]);
    prepared[j] = '\0';
}

//===============Make Pair======================
int makePairs(char prepared[], char pairs[][2]){
    int i = 0, j = 0;
    while (prepared[i] != '\0') {
        char a = prepared[i];
        char b;
        if (prepared[i + 1] == '\0') {
            b = 'X';
            i++;
        } else {
            b = prepared[i + 1];
            if (a == b) {
                b = 'X';
                i++;
            } else {
                i += 2;
            }
        }
        pairs[j][0] = a;
        pairs[j][1] = b;
        j++;
    }
    return j;
}
//=============Encryption Function===================
void encrypt(char pairs[][2], int pairCount, char keyTable[5][5], char cipher[]) {
    int i, row1, col1, row2, col2, k = 0;
    for (i = 0; i < pairCount; i++) {
        findPosition(keyTable, pairs[i][0], &row1, &col1);
        findPosition(keyTable, pairs[i][1], &row2, &col2);

        //======= Alphabet Same Row=============
        if (row1 == row2) {
            cipher[k++] = keyTable[row1][(col1 + 1) % 5];
            cipher[k++] = keyTable[row2][(col2 + 1) % 5];
        }

        //========Alphabet Same Column==========
        else if (col1 == col2) {
            cipher[k++] = keyTable[(row1 + 1) % 5][col1];
            cipher[k++] = keyTable[(row2 + 1) % 5][col2];
        }
        //=======Alphabet different Row and Column========
        else {
            cipher[k++] = keyTable[row1][col2];
            cipher[k++] = keyTable[row2][col1];
        }
    }
    cipher[k] = '\0';
}
//=================Decryption Function=====================
void decrypt(char pairs[][2], int pairCount, char keyTable[5][5], char plain[]) {
    int i, row1, col1, row2, col2, k = 0;
    for (i = 0; i < pairCount; i++) {
        findPosition(keyTable, pairs[i][0], &row1, &col1);
        findPosition(keyTable, pairs[i][1], &row2, &col2);

        //======= Alphabet Same Row=============
        if (row1 == row2) {
            plain[k++] = keyTable[row1][(col1 + 5 - 1) % 5];
            plain[k++] = keyTable[row2][(col2 + 5 - 1) % 5];
        }
        //========Alphabet Same Column==========
        else if (col1 == col2) {
            plain[k++] = keyTable[(row1 + 5 - 1) % 5][col1];
            plain[k++] = keyTable[(row2 + 5 - 1) % 5][col2];
        }
        //=======Alphabet different Row and Column========
        else {
            plain[k++] = keyTable[row1][col2];
            plain[k++] = keyTable[row2][col1];
        }
    }
    plain[k] = '\0';
}
int main() {
    char key[100], text[100], prepared[100], cipher[200], decrypted[200];
    char keyTable[5][5];
    char pairs[100][2];
    int pairCount;

    //==========Get Plain Text From User========
    printf("Enter plain text: ");
    fgets(text, sizeof(text), stdin);

    //==========Get Key Text From User==========
    printf("Enter key: ");
    fgets(key, sizeof(key), stdin);

    generateKeyTable(key, keyTable);
    printKeyTable(keyTable);

    prepareText(text, prepared);
    pairCount = makePairs(prepared, pairs);


    //===========Encryption================
    encrypt(pairs, pairCount, keyTable, cipher);
    printf("\nEncrypted text: %s\n", cipher);

    //===========Decryption================
    pairCount = makePairs(cipher, pairs);
    decrypt(pairs, pairCount, keyTable, decrypted);
    printf("Decrypted text: %s\n", decrypted);

    return 0;
}
